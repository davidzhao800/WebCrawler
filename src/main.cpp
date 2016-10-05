//============================================================================
// Name        : main.cpp
// Author      : hzhao
// Version     :
// Copyright   : WebCrawler is for CS3103 Assignment 2
// Description : Entry of crawler program, initialize thread pool and start
//			     thread there.
//============================================================================

#include "ThreadPool.h"
#include "unistd.h"
#include "WebCrawler.h"
#include "URLNode.h"

#include <iostream>

using namespace std;

int main() {

	cout << "Crawler started!" << endl;
	ThreadPool* myPool = new ThreadPool();
	myPool->initializeThread();
	myPool->destroy();
	delete myPool;

	return 0;
}

