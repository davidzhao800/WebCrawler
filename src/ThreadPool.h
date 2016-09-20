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

using namespace std;

class ThreadPool {
public:
	ThreadPool(); // by default number of thread is 2
	ThreadPool(int number_of_threads);
	virtual ~ThreadPool();

	//void destroyPool(int maxPollSecs);

	void initializeThread();

	static void *executeThread(void *param);

private:
	int number_of_threads;
	static int FileIndex;
	static string FileName;

};

#endif /* THREADPOOL_H_ */

