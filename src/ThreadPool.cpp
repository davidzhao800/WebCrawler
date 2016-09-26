/*
 * ThreadPool.cpp
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#include "ThreadPool.h"
#include <iostream>
#include <sqlite3.h>

using namespace std;

pthread_mutex_t ThreadPool::mutexProgressQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexUrlQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexUrlHash = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexProgressedCounter = PTHREAD_MUTEX_INITIALIZER;

queue<URLNode> *ThreadPool::UrlQueue=new queue<URLNode>;
queue<URLNode> ThreadPool::UrlQueueTemp;

bool noWork;				// used to notify the end of crawler
int N_inProgress;			// counter for currently progressing url
int N_Progressed;			// counter for progressed url
int max_depth = 4;

ThreadPool::ThreadPool() {
	this->number_of_threads = 2;
	this->maxDepth = 4;
	N_inProgress = 0;
	N_Progressed = 0;
}

ThreadPool::~ThreadPool() {

}

void ThreadPool::destroy()
{

	pthread_mutex_destroy(&mutexUrlQueue);
	pthread_mutex_destroy(&mutexUrlHash);
	pthread_mutex_destroy(&mutexProgressQueue);
	pthread_mutex_destroy(&mutexProgressedCounter);
	delete UrlQueue;

}

/**
    sql callback function

*/
int ThreadPool::callback(void *exist, int argc, char **argv, char **azColName){
   int i;
   bool *tm=(bool*)exist;
   for(i=0; i<argc; i++){
	   *tm = true;
   }
   return 0;
}

/**
    check whether the url is visited before in sql database

    @param string url
    @return whether the url is visited before
*/
bool ThreadPool::isURLVisited(string url) {
	sqlite3 * db;
	char *zErrMsg = 0;
	int rc;
	char* sql;

	bool exist = false;

	rc = sqlite3_open("urlDB.db", &db);
	if (rc) {
		cout << "Can't open Database, quit!" << endl;
		return false;
	}

	string hah = "SELECT URL from URLHASH WHERE URL='" + url + "'";
	sql=(char*)hah.c_str();

	rc = sqlite3_exec(db, sql, callback, (char *) &exist, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
	}

	sqlite3_close(db);
	return exist;
}

/**
    insert the string url into database for duplication check

    @param string url
    @return successful insert?
*/
bool ThreadPool::insertURL(string url) {
	sqlite3 * db;
	char *zErrMsg = 0;
	int rc;
	char* sql;

	bool exist = false;

	rc = sqlite3_open("urlDB.db", &db);
	if (rc) {
		cout << "Can't open Database, quit!" << endl;
		return false;
	}

	string hah = "INSERT INTO URLHASH (URL) VALUES ('" + url + "'); ";
	sql=(char*)hah.c_str();

	rc = sqlite3_exec(db, sql, callback, (char *) &exist, &zErrMsg);
	if (rc != SQLITE_OK) {

		sqlite3_free(zErrMsg);
		return false;
	}

	sqlite3_close(db);
	return true;
}

/**
    insert the domain with response time into database
    domain is not unique, because I want to get average response time

    @param domain
    @param response time
    @return successful insert the domain with response time
*/
bool ThreadPool::insertResponseTime(string domain, double time) {
	sqlite3 * db;
	char *zErrMsg = 0;
	int rc;
	char* sql;

	bool exist = false;

	rc = sqlite3_open("urlDB.db", &db);
	if (rc) {
		cout << "Can't open Database, quit!" << endl;
		return false;
	}

	string hah = "INSERT INTO URLTIME (DOMAIN,TIME) VALUES ('" + domain + "',"+ to_string(time) +"); ";
	sql=(char*)hah.c_str();

	rc = sqlite3_exec(db, sql, callback, (char *) &exist, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return false;
	}

	sqlite3_close(db);
	return true;
}

/**
    Entry function for each thread

    @param
    @return null
*/
void *ThreadPool::executeThread(void *param) {

	pthread_t self_id;
	self_id = pthread_self();  // thread id

	while (1) {
		if (UrlQueue->size() <= 0) {
			sleep(5);
			cout << "thread " << self_id << " is waiting..." << endl;

			// this is to signal the termination of program
			if (UrlQueue->size() == 0 && N_inProgress == 0) {
				noWork = true;
			}
			continue;
		}

		// get a node from URL queue
		pthread_mutex_lock(&mutexUrlQueue);
		URLNode node = UrlQueue->front();
		UrlQueue->pop();
		pthread_mutex_unlock(&mutexUrlQueue);

		pthread_mutex_lock(&mutexProgressedCounter);
		N_Progressed +=1;
		pthread_mutex_unlock(&mutexProgressedCounter);

		if (node.getDepth() >= max_depth) {
			cout << self_id << ": Depth " << node.getDepth() << " exceed max depth." << endl;
			continue;
		}
		pthread_mutex_lock(&mutexProgressQueue);
		N_inProgress += 1;
		pthread_mutex_unlock(&mutexProgressQueue);

		cout << "UrlQueue size is: " <<UrlQueue->size() << " URL in progress is: " <<  N_inProgress << " URL progressed is: " <<  N_Progressed << " Depth: " << node.getDepth()  << endl;

		WebCrawler* webcrawlerwork = new WebCrawler(node, self_id);

		cout << self_id << ": Downloading " + node.getRawString() << " Depth: " << node.getDepth() << endl;
		// Download html
		if (!webcrawlerwork->downloadHTML()) {
			cout << self_id  << ": Downloading error, delete worker" << endl;
			pthread_mutex_lock(&mutexProgressQueue);
			N_inProgress -=1;
			pthread_mutex_unlock(&mutexProgressQueue);
			delete webcrawlerwork;
			continue;
		}

		// insert response time into database
		pthread_mutex_lock(&mutexUrlHash);
		insertResponseTime(node.getDomain(),webcrawlerwork->getResponseTime());
		pthread_mutex_unlock(&mutexUrlHash);

		cout << self_id  << ": Extracting " + node.getRawString() << endl;

		// extract url from html response
		if (!webcrawlerwork->extractURLs()) {
			cout << self_id  << ": Extracting error, delete worker" << endl;
			pthread_mutex_lock(&mutexProgressQueue);
			N_inProgress -=1;
			pthread_mutex_unlock(&mutexProgressQueue);
			delete webcrawlerwork;
			continue;
		}

		UrlQueueTemp = webcrawlerwork->HtmlUrlQueue;
		while (UrlQueueTemp.size() > 0) {

			URLNode NodeTemp = UrlQueueTemp.front();
			UrlQueueTemp.pop();

			pthread_mutex_lock(&mutexUrlHash);
			bool visited = isURLVisited(NodeTemp.getRawString());
			pthread_mutex_unlock(&mutexUrlHash);


			if (!visited) {
				cout << self_id  << ": Got this new url: "<<NodeTemp.getRawString() << endl;

				pthread_mutex_lock(&mutexUrlHash);
				if (!insertURL(NodeTemp.getRawString())){
					pthread_mutex_unlock(&mutexUrlHash);
					continue;
				}
				pthread_mutex_unlock(&mutexUrlHash);

				pthread_mutex_lock(&mutexUrlQueue);
				UrlQueue->push(NodeTemp);
				pthread_mutex_unlock(&mutexUrlQueue);


			}
		}
		pthread_mutex_lock(&mutexProgressQueue);
		N_inProgress -=1;
		pthread_mutex_unlock(&mutexProgressQueue);

		sleep(3);
		delete webcrawlerwork;
	}
	return NULL;
}

/**
    Create threads here, insert some seeds URLs

    @param null
    @return null
*/
void ThreadPool::initializeThread() {

	noWork = false;

	string thread = "";
	ifstream infile("thread_number.conf");

	// handle file error
	if (!infile.is_open()) {
		cerr << "thread_number config is not opened correctly. Use 2 thread" << endl;
	} else {
		getline(infile, thread);
		cout << "thread_number: " << thread << endl;
		number_of_threads = std::stoi(thread);

	}

	string depth = "";
	ifstream infile1("depth.conf");

	// handle file error
	if (!infile1.is_open()) {
		cerr << "depth config is not opened correctly. Use 4 depth" << endl;
	} else {
		getline(infile1, depth);
		cout << "maxDepth: " << depth << endl;
		maxDepth = std::stoi(depth);
		max_depth = maxDepth;
	}

	string line = "";
	ifstream infile2("seed.txt");

	// handle file error
	if (!infile2.is_open()){
		cerr << "Seed file is not opened correctly." << endl;
		return;
	}

	while (getline(infile2, line)) {
		if (line.length() == 0) continue;
		URLNode seedNode;

		seedNode.setURL(line, 1);

		pthread_mutex_lock(&mutexUrlHash);
		insertURL(seedNode.getRawString());
		pthread_mutex_unlock(&mutexUrlHash);

		pthread_mutex_lock(&mutexUrlQueue);
		UrlQueue->push(seedNode);
		pthread_mutex_unlock(&mutexUrlQueue);

	}

	infile.close();

	pthread_t* threadArray = new pthread_t[number_of_threads];

   	for(int i = 0; i<number_of_threads; i++) {
		int temp = pthread_create(&threadArray[i], NULL, &ThreadPool::executeThread, (void *) this );

		// handle create thread err
		if (temp != 0) {
			cerr << "Thread cannot be created!" << endl;
		}


		cout << "Created thread " << threadArray[i] << endl;
		sleep(3);
	}

	while (!noWork);

   	delete[] threadArray;
}

