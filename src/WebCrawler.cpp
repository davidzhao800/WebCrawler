/*
 * WebCrawler.cpp
 *
 *  Created on: 13 Sep 2016
 *      Author: hzhao
 */

#include "WebCrawler.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include "gumbo.h"

using namespace std;

WebCrawler::WebCrawler(URLNode urlnode) {
	node = urlnode;
}

WebCrawler::~WebCrawler() {

}

bool WebCrawler::downloadHTML() {
//	ofstream HtmlFile;
//	const char * filept=FileName.c_str();
//	HtmlFile.open(filept,ios::trunc);
//	struct epoll_event ev;
//	struct epoll_event events[5],eventsrecv[5];


	int sock, bytes_recieved;
	char send_data[1024],recv_data[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;

	//cout << "Resolving " + node.getDomain() << endl;
	host = gethostbyname( node.getDomain().c_str());

	if (host == NULL) {
		cout<<"get dns failed"<<endl;
		return false;
	}
	//cout<<"Host name :"<< h->h_name<<endl;
	//cout<<"IP Address :"<<inet_ntoa(*((struct in_addr *)h->h_addr))<<endl;

	//create a Socket structure   - "Client Socket"
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Creation Error!");
		cout << "Socket Creation Error!" << endl;
		return false;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	int opt=1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		cout << "setsockopt(SO_REUSEADDR) failed" << endl;
		return false;
	}

//	if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0)|O_NONBLOCK) == -1) {
//		perror("Set non-blocking socket Error");
//		return false;
//	}

	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	    perror("Connect Error");
	    cout << "Connect Error" << endl;
	    return false;
	}

	char strRequest[1000];
	sprintf(strRequest,"GET %s HTTP/1.1\r\nHost: %s\r\nAccept: */*\r\n\r\n\r\n",node.getAttributeURL().c_str(),node.getDomain().c_str());
	int ret = send(sock,strRequest,sizeof(strRequest),0);

	int flag=1;

	while (flag) {
		char buf[2048];
		bzero(buf, sizeof(buf));
		ret = recv(sock, buf, 2047, 0);
		if (ret > 0) {
			htmlString += buf;
		} else if (ret == 0) {
			flag = 0;

			close (sock);
			if (node.getDepth() < 10) {
				//printf("\n%s ", htmlString.c_str());
				return true;
			} else {
				return false;
			}
		} else {
			flag = 0;
		}
	}

	return true;
}

bool WebCrawler::isRelativeURL(string url){
	return url.at(0) == '/';
}

bool WebCrawler::isHttpURL(string url){
    const std::string urlRegexStr = "(http)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))"
                                    "([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?";
    const regex urlRegex(urlRegexStr.c_str());
    return regex_match(url,urlRegex);
}

bool WebCrawler::extractURLs() {
	GumboOutput* output = gumbo_parse(htmlString.c_str());
	_extractURLs(output->root);
	return true;
}

void WebCrawler::_extractURLs(GumboNode* gumboNode) {
	if (gumboNode->type != GUMBO_NODE_ELEMENT)
		return;
	GumboAttribute* href;
	if (gumboNode->v.element.tag == GUMBO_TAG_A
			&& (href = gumbo_get_attribute(&gumboNode->v.element.attributes, "href"))) {

		URLNode newURL;

		string url = string(href->value);
		if (isRelativeURL(url)){
			url =  "http://" + node.getDomain() + url;
		}

		if (isHttpURL(url)) {
			//cout << url << endl;
			newURL.setURL(url, node.getDepth() +1);

			HtmlUrlQueue.push(newURL);
		}
	}
	GumboVector* children = &gumboNode->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i)
		_extractURLs(static_cast<GumboNode*>(children->data[i]));
}


