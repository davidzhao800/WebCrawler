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

	cout << "Begin!" << endl; // prints !!!Hello World!!!
	ThreadPool* myPool = new ThreadPool(5);
	myPool->initializeThread();

	delete myPool;


	return 0;
}

