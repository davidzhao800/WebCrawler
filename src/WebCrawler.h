/*
 * WebCrawler.h
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 *
 *  This is the main logic of crawler, basically consists of two parts:
 *  	download URL
 *  	extract URL
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
	bool downloadHTML();							// download URL, socket programming is here
	bool extractURLs();								// parse html string and extract url using gumbo-parser lib
	double getResponseTime();
	string relativeToAbsolute(string str);			// parse relative url to absolute url
	queue<URLNode> HtmlUrlQueue;					// temporary queue for all found urls
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
