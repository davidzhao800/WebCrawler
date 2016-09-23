/*
 * ThreadPool.cpp
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#include "ThreadPool.h"
#include <iostream>

using namespace std;

pthread_mutex_t ThreadPool::mutexProgressQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexUrlQueue = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexUrlHash = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t ThreadPool::mutexProgressedCounter = PTHREAD_MUTEX_INITIALIZER;
//pthread_cond_t  ThreadPool::hasWork;
//pthread_cond_t  ThreadPool::workDone;

unordered_set<string> *ThreadPool::UrlHash=new unordered_set <string>;
queue<URLNode> *ThreadPool::UrlQueue=new queue<URLNode>;
queue<URLNode> *ThreadPool::ProgressQueue=new queue<URLNode>;
queue<URLNode> ThreadPool::UrlQueueTemp;

bool noWork;
int N_inProgress;
int N_Progressed;

ThreadPool::ThreadPool(int num_of_threads) {
	if ( num_of_threads <= 0 )  number_of_threads=1;
   	this->number_of_threads = num_of_threads;
   	N_inProgress = 0;
   	N_Progressed = 0;
}

ThreadPool::ThreadPool() {
	this->number_of_threads = 2;
	N_inProgress = 0;
	N_Progressed = 0;
}

ThreadPool::~ThreadPool() {

}

void ThreadPool::destroyPool()
{
//	while( UrlQueue->size()>0 )
//	{
//	        cout << "Work is still incomplete=" << UrlQueue->size() << endl;
//		sleep(maxPollSecs);
//	}
//	cout << "All Done!! Wow! That was a lot of work!" << endl;
	pthread_mutex_destroy(&mutexUrlQueue);
	pthread_mutex_destroy(&mutexUrlHash);
	delete UrlHash;
	delete UrlQueue;
	cout << "END   Work is still incomplete=" << UrlQueue->size() << endl;

}


void *ThreadPool::executeThread(void *param) {

	pthread_t self_id;
	self_id = pthread_self();

	//cout << UrlQueue->size() << endl;
	while (1) {
		//cout << "ahahahhahha" << endl;
//		pthread_mutex_lock (&thelock);
//		while (UrlQueue->empty()) {
//			pthread_cond_wait(&hasWork, &thelock);
//		}
//
//		URLNode node = UrlQueue->front();
//		UrlQueue->pop();
//
//		pthread_mutex_unlock (&thelock);
		if (UrlQueue->size() <= 0) {
			sleep(5);
			cout << "thread " << self_id << " is waiting..." << endl;
			if (UrlQueue->size() == 0 && N_inProgress == 0) {
				noWork = true;
			}
			continue;
		}
		//cout << "thread " << self_id << " executing" << endl;
		pthread_mutex_lock(&mutexUrlQueue);
		URLNode node = UrlQueue->front();
		UrlQueue->pop();
		pthread_mutex_unlock(&mutexUrlQueue);

		pthread_mutex_lock(&mutexProgressedCounter);
		N_Progressed +=1;
		pthread_mutex_unlock(&mutexProgressedCounter);

		cout << "UrlQueue size is: " <<UrlQueue->size() << " URL in progress is: " <<  N_inProgress << " URL progressed is: " <<  N_Progressed << " Depth: " << node.getDepth()  << endl;

		if (node.getDepth() >= 4) {
			cout << self_id << ": Depth " << node.getDepth() << " exceed max depth." << endl;
			continue;
		}
		pthread_mutex_lock(&mutexProgressQueue);
		N_inProgress += 1;
		pthread_mutex_unlock(&mutexProgressQueue);

		WebCrawler* webcrawlerwork = new WebCrawler(node, self_id);

		cout << self_id << ": Downloading " + node.getRawString() << " Depth: " << node.getDepth() << endl;
		if (!webcrawlerwork->downloadHTML()) {
			cout << self_id  << ": Downloading error, delete worker" << endl;
			pthread_mutex_lock(&mutexProgressQueue);
			N_inProgress -=1;
			pthread_mutex_unlock(&mutexProgressQueue);
			delete webcrawlerwork;
			continue;
		}

		cout << self_id  << ": Extracting " + node.getRawString() << endl;

		if (!webcrawlerwork->extractURLs()) {
			cout << self_id  << ": Extracting error, delete worker" << endl;
			pthread_mutex_lock(&mutexProgressQueue);
			N_inProgress -=1;
			pthread_mutex_unlock(&mutexProgressQueue);
			delete webcrawlerwork;
			continue;
		}
//		cout << self_id  << ": Extracting done" << endl;

		UrlQueueTemp = webcrawlerwork->HtmlUrlQueue;
		while (UrlQueueTemp.size() > 0) {

			URLNode NodeTemp = UrlQueueTemp.front();
			UrlQueueTemp.pop();

			unordered_set<std::string>::const_iterator got = UrlHash->find(
					NodeTemp.getRawString());
			if (got == UrlHash->end()) {
				cout << self_id  << ": Got this new url: "<<NodeTemp.getRawString() << endl;

				pthread_mutex_lock(&mutexUrlHash);
				UrlHash->insert(NodeTemp.getRawString());
				pthread_mutex_unlock(&mutexUrlHash);

				pthread_mutex_lock(&mutexUrlQueue);
				UrlQueue->push(NodeTemp);
				pthread_mutex_unlock(&mutexUrlQueue);

//				pthread_mutex_lock(&thelock);
//				UrlQueue->push(NodeTemp);
//				pthread_mutex_unlock(&thelock);

//				(void)pthread_cond_signal( &hasWork );
			}
		}
		pthread_mutex_lock(&mutexProgressQueue);
		N_inProgress -=1;
		pthread_mutex_unlock(&mutexProgressQueue);
//		pthread_mutex_lock(&thelock);
//		if(UrlQueue->size() == 0 && N_inProgress == 0) {
//			noWork = true;
//		}
//		pthread_mutex_unlock(&thelock);
		sleep(3);
		delete webcrawlerwork;
	}
	return NULL;
}

void ThreadPool::initializeThread() {
	noWork = false;

	URLNode firstnode;
	//firstnode.setURL("http://www.comp.nus.edu.sg/~zhaojin/index.html", 1);
	//firstnode.setURL("http://www.nus.edu.sg/oam/scholarships.html", 1);
	firstnode.setURL("http://www.comp.nus.edu.sg/", 1);

	pthread_mutex_lock(&mutexUrlHash);
	UrlHash->insert(firstnode.getRawString());
	pthread_mutex_unlock(&mutexUrlHash);
	pthread_mutex_lock(&mutexUrlQueue);
	UrlQueue->push(firstnode);
	pthread_mutex_unlock(&mutexUrlQueue);
//	pthread_mutex_lock(&thelock);
//	UrlQueue->push(firstnode);
//	pthread_mutex_unlock(&thelock);

	//cout << UrlQueue->size()  << endl;

	pthread_t* threadArray = new pthread_t[number_of_threads];

   	for(int i = 0; i<number_of_threads; i++) {
		int temp = pthread_create(&threadArray[i], NULL, &ThreadPool::executeThread, (void *) this );
		cout << "Created thread " << threadArray[i] << endl;
		sleep(3);
	}

	while (!noWork);
//   	pthread_mutex_lock(&thelock);
//
//   	pthread_cond_wait(&workDone, &thelock);
//   	pthread_mutex_unlock(&thelock);


    delete[] threadArray;
	pthread_mutex_destroy(&mutexUrlQueue);
	pthread_mutex_destroy(&mutexUrlHash);
	delete UrlHash;
	delete UrlQueue;


}

