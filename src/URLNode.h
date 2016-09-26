/*
 * URLNode.h
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 *
 *  URL is parsed in this way:
 *  string_url    http://domain.com:80/path/to/index.html
 *  domain        domain.com
 *  port_number   80
 *  path          /path/to/
 *  attribute_url index.html
 *
 */

#ifndef URLNODE_H_
#define URLNODE_H_

#include<string>
using namespace std;

class URLNode {
private:
	int port_number;
	int depth;
	string string_url;
	string domain;
	string attribute_url;
	string path;

public:

	URLNode();
	virtual ~URLNode();

	void setURL(string url, int depth, int port);
	void setURL(string url, int depth);
	string getRawString();
	string getDomain();
	string getPath();
	string getAttributeURL();
	int getDepth();
	int getPortNumber();

	void parseURL();
};

#endif /* URLNODE_H_ */
