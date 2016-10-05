/*
 * URLNode.cpp
 *
 *  Created on: 14 Sep 2016
 *      Author: hzhao
 */
#include <string>
#include <regex>
#include <iostream>
#include "URLNode.h"

using namespace std;

URLNode::URLNode() {

}

URLNode::~URLNode() {

}


void URLNode::setURL(string url, int depth, int port) {
	this->string_url = url;
	this->depth = depth;
	this->port_number = port;
	parseURL();
}

void URLNode::setURL(string url, int depth) {
	this->string_url = url;
	this->depth = depth;
	this->port_number = 80;
	parseURL();
}

string URLNode::getRawString() {
	return string_url;
}

string URLNode::getDomain() {
	return domain;
}

string URLNode::getAttributeURL() {
	return attribute_url;
}

string URLNode::getPath() {
	return path;
}

int URLNode::getDepth() {
	return depth;
}

int URLNode::getPortNumber() {
	return port_number;
}

/**
 *   Parse the URL and store them in URLNode
 *
 *  URL is parsed in this way:
 *  string_url    http://domain.com:80/path/to/index.html
 *  domain        domain.com
 *  port_number   80
 *  path          /path/to/
 *  attribute_url index.html
 *
 *   @param null
 *   @return null
*/
void URLNode::parseURL(){

	int start = 0;
	int first_index = 0;
	int last_index = 0;
	int colon_index = 0;
    int found = string_url.find("//");

    if(found != string::npos && found <= 7){
    	start = found + 2;
    }

	first_index = string_url.find("/", start);
	last_index = string_url.find_last_of("/");
	if (first_index == string::npos) {
		this->attribute_url = "";
		this->path = "/";
		colon_index = string_url.find(":", start);
		if (colon_index == string::npos) {
			this->domain = string_url.substr(start);
		} else {
			this->domain = string_url.substr(start, colon_index - start);
			this->port_number = stoi(string_url.substr(colon_index + 1));
		}
	} else {

		this->attribute_url = string_url.substr(last_index + 1);
		this->path = string_url.substr(first_index,
				last_index - first_index + 1);
		colon_index = string_url.find(":", start);
		if (colon_index == string::npos) {
			this->domain = string_url.substr(start, first_index - start);
		} else {
			this->domain = string_url.substr(start, colon_index - start);
			this->port_number = stoi(string_url.substr(colon_index + 1));
		}
	}

	string_url = domain+path+attribute_url;


}

