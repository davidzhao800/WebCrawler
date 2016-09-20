/*
 * ThreadPool.cpp
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int num_of_threads) {
	if ( num_of_threads <= 0 )  number_of_threads=1;
   	this->number_of_threads = num_of_threads;
}

ThreadPool::ThreadPool() {
	this->number_of_threads = 2;
}

ThreadPool::~ThreadPool() {

}

void *ThreadPool::executeThread(void *param) {
	cout << "Hello world!" << endl;
	return NULL;
}

void ThreadPool::initializeThread() {
   	for(int i = 0; i<number_of_threads; i++) {
		pthread_t threadId;
		int temp = pthread_create(&threadId, NULL, &ThreadPool::executeThread, (void *) this );

	}

}

