/*
 * URLNode.h
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
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

public:

	URLNode();
	virtual ~URLNode();

	void setURL(string url, int depth, int port);
	void setURL(string url, int depth);
	string getRawString();
	string getDomain();
	string getAttributeURL();
	int getDepth();
	int getPortNumber();


//	bool isRelativeURL();
//	void handleRelative();
	void parseURL();
	//string getFirstPiece(std::string& url);
	//string toBaseURL(std::string& url);
	//bool isValidAbsolute();
	//string toString();
	//void toAbsolute(std::string& relativeToUrl);
};

#endif /* URLNODE_H_ */
