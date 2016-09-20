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

using namespace std;

class WebCrawler {
public:
	WebCrawler(URLNode node);
	virtual ~WebCrawler();
	bool downloadHTML();
	bool extractURLs();
	queue<URLNode> HtmlUrlQueue;
private:
	void _extractURLs(GumboNode* node);
	URLNode node;
	string htmlString;
	bool isRelativeURL(string str);
	bool isHttpURL(string str);

};

#endif /* WEBCRAWLER_H_ */
