/*
 * WebCrawler.h
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#ifndef WEBCRAWLER_H_
#define WEBCRAWLER_H_

#include <queue>
#include <regex>
#include <string>
#include "URLNode.h"
#include "gumbo.h"
#include <sqlite3.h>
#include <pthread.h>

using namespace std;

class WebCrawler {
public:
	WebCrawler(URLNode node, pthread_t threadid);
	virtual ~WebCrawler();
	bool downloadHTML();
	bool extractURLs();
	double getResponseTime();
	string relativeToAbsolute(string str);
	queue<URLNode> HtmlUrlQueue;
private:
	void _extractURLs(GumboNode* node);
	pthread_t threadID;
	URLNode node;
	string htmlString;
	double responseTime;
	bool isRelativeURL(string str);
	bool isHttpURL(string str);


};

#endif /* WEBCRAWLER_H_ */
