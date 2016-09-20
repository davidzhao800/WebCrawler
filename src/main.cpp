//============================================================================
// Name        : main.cpp
// Author      : hzhao
// Version     :
// Copyright   : WebCrawler is for CS3103 Assignment 2
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "ThreadPool.h"
#include "unistd.h"
#include "WebCrawler.h"
#include "URLNode.h"

#include <iostream>

using namespace std;

int main() {

	static queue<URLNode> *UrlQueue;
	static queue<URLNode> UrlQueueTemp;
	UrlQueue=new queue<URLNode>;
	static std::unordered_set<string> *UrlHash=new unordered_set <string>;

//	cout << "Begin!" << endl; // prints !!!Hello World!!!
//	ThreadPool* myPool = new ThreadPool(5);
//	myPool->initializeThread();
//	time_t t1 = time(NULL);
//	//while(1);
//	sleep(5);
//	//myPool->destroyPool(10);
//	time_t t2 = time(NULL);
//	cout << t2 - t1 << " seconds elapsed\n" << endl;
//	delete myPool;

	URLNode firstnode;
	firstnode.setURL("www.nus.edu.sg", 1);

	UrlHash->insert(firstnode.getRawString());

	UrlQueue->push(firstnode);

	while (1) {
		if (UrlQueue->size() <= 0) {
			break;
//			sleep(5);
//			continue;
		}

		URLNode node =UrlQueue->front();
		UrlQueue->pop();

		WebCrawler* webcrawlerwork = new WebCrawler(node);

		if(!webcrawlerwork->downloadHTML()) {
			//cout << "Downloading error, deleting worker" << endl;
			delete webcrawlerwork;
			continue;
		}

		cout << "extracting: " +  node.getRawString() << endl;

		if(!webcrawlerwork->extractURLs()) {
			delete webcrawlerwork;
			continue;
		}
		cout << "extracting done" << endl;

		UrlQueueTemp = webcrawlerwork->HtmlUrlQueue;
		while (UrlQueueTemp.size() > 0) {

			URLNode NodeTemp = UrlQueueTemp.front();
			UrlQueueTemp.pop();
			unordered_set<std::string>::const_iterator got = UrlHash->find(
					NodeTemp.getRawString());
			if (got == UrlHash->end()) {
				cout << NodeTemp.getRawString() << endl;
				UrlHash->insert(NodeTemp.getRawString());

				UrlQueue->push(NodeTemp);
			}
		}
		delete webcrawlerwork;
	}

	return 0;
}


//	URLNode second;
//	second.setURL("https://www.nus.edu.sg:8080/haha/hahah.html", 2);
//	cout << firstnode.getRawString() << endl;
//	cout << firstnode.getDomain() << endl;
//	cout << firstnode.getDepth() << endl;
//	cout << firstnode.getAttributeURL() << endl;

//	cout << webcrawlerwork->HtmlUrlQueue.size() << endl;
//	URLNode node = webcrawlerwork->HtmlUrlQueue.front();
//	cout << node.getRawString() << endl;
//	webcrawlerwork->HtmlUrlQueue.pop();
//	node = webcrawlerwork->HtmlUrlQueue.front();
//	cout << node.getRawString() << endl;
//	webcrawlerwork->HtmlUrlQueue.pop();
//	node = webcrawlerwork->HtmlUrlQueue.front();
//	cout << node.getRawString() << endl;

