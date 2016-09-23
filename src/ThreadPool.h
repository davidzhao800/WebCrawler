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
#include "unistd.h"
#include "WebCrawler.h"
#include "URLNode.h"

using namespace std;

class ThreadPool {
public:
	ThreadPool(); // by default number of thread is 2
	ThreadPool(int number_of_threads);
	virtual ~ThreadPool();

	void destroyPool();

	void initializeThread();

	static void *executeThread(void *param);

	static pthread_mutex_t mutexUrlQueue;
	static pthread_mutex_t mutexUrlHash;
	static pthread_mutex_t mutexProgressQueue;
	static pthread_mutex_t mutexProgressedCounter;
//	static pthread_cond_t hasWork;
//	static pthread_cond_t workDone;

private:
	int number_of_threads;

	static unordered_set<string> *UrlHash;
	static queue<URLNode> *UrlQueue;
	static queue<URLNode> *ProgressQueue;
	static queue<URLNode> UrlQueueTemp;

};

#endif /* THREADPOOL_H_ */

