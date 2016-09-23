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


//void URLNode::parseURL(){
//
//	int index = string_url.find("www.");
//	if (index != string::npos) {
//		string_url.erase(index,4);
//	}
//	if(string_url.back() == '/'){
//		string_url.erase(string_url.length()-1,1);
//	}
//
//	int start = 0;
//    int found = string_url.find("//");
//    if(found == string::npos){
//        found = string_url.find("/");
//    }
//    else{
//    	start = found+2;
//        found = string_url.find("/",found+2);
//    }
//
//    if(found == string::npos) {
//    	this->attribute_url = "/";
//    	found = string_url.find(":",start);
//    	if(found == string::npos) {
//    		this->domain = string_url.substr(start);
//    	} else {
//    		this->domain = string_url.substr(start,found);
//    		this->port_number = stoi(string_url.substr(found+1));
//    	}
//    } else {
//    	this->attribute_url = string_url.substr(found);
//		string temp = string_url.substr(start, found - start);
//
//		found = temp.find(":");
//		if (found == string::npos) {
//			this->domain = temp;
//		} else {
//			this->domain = temp.substr(0, found);
//
//			this->port_number = stoi(temp.substr(found + 1));
//		}
//    }
//}

void URLNode::parseURL(){

//	int index = string_url.find("www.");
//	if (index != string::npos) {
//		string_url.erase(index,4);
//	}
//	if(string_url.back() == '/'){
//		string_url.erase(string_url.length()-1,1);
//	}
	//http://nus.edu.sg/haha/hahah.html

	int start = 0;
	int first_index = 0;
	int last_index = 0;
	int colon_index = 0;
    int found = string_url.find("//");

	start = found + 2;
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

