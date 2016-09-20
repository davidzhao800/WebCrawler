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

int URLNode::getDepth() {
	return depth;
}

int URLNode::getPortNumber() {
	return port_number;
}


void URLNode::parseURL(){
	int index = string_url.find("www.");
	if (index != string::npos) {
		string_url.erase(index,4);
	}
	if(string_url.back() == '/'){
		string_url.erase(string_url.length()-1,1);
	}

	int start = 0;
    int found = string_url.find("//");
    if(found == string::npos){
        found = string_url.find("/");
    }
    else{
    	start = found+2;
        found = string_url.find("/",found+2);
    }

    if(found == string::npos) {
    	this->attribute_url = "/";
    	found = string_url.find(":",start);
    	if(found == string::npos) {
    		this->domain = string_url.substr(start);
    	} else {
    		this->domain = string_url.substr(start,found);
    		this->port_number = stoi(string_url.substr(found+1));
    	}
    } else {
    	this->attribute_url = string_url.substr(found);
		string temp = string_url.substr(start, found - start);

		found = temp.find(":");
		if (found == string::npos) {
			this->domain = temp;
		} else {
			this->domain = temp.substr(0, found);

			this->port_number = stoi(temp.substr(found + 1));
		}
    }
}

