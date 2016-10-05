/*
 * ThreadPool.h
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_

#include <pthread.h>
#include <queue>
#include <unordered_set>
#include <string>
#include <iostream>
#include <fstream>
#include "unistd.h"
#include "WebCrawler.h"
#include "URLNode.h"

using namespace std;

class ThreadPool {
public:
	ThreadPool(); 										// by default number of thread is 2
	virtual ~ThreadPool();

	void destroy();

	void initializeThread();

	static void *executeThread(void *param);

	static pthread_mutex_t mutexUrlQueue;                // mutex for URL Queue
	static pthread_mutex_t mutexUrlHash;                 // mutex for sql database
	static pthread_mutex_t mutexProgressQueue;           // mutex for counter of current progressing url
	static pthread_mutex_t mutexProgressedCounter;       // mutex for counter of progressed URL

private:
	int number_of_threads;
	int timeout;
	int maxDepth;

	static queue<URLNode> *UrlQueue;					// URL Queue, Singleton pattern
	static queue<URLNode> UrlQueueTemp;					// temp queue for one worker

	static bool isURLVisited(string url);
	static bool insertURL(string url);
	static bool insertResponseTime(string domain, double time);
	static int callback(void *exist, int argc, char **argv, char **azColName);


};

#endif /* THREADPOOL_H_ */

