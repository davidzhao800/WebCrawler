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
#include <chrono>
#include "gumbo.h"

using namespace std;
using namespace std::chrono;

WebCrawler::WebCrawler(URLNode urlnode, pthread_t threadid) {
	node = urlnode;
	threadID = threadid;
	htmlString.clear();
	responseTime=0;
}

WebCrawler::~WebCrawler() {
	htmlString.clear();
	std::queue<URLNode> empty;
	std::swap( HtmlUrlQueue, empty );
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

//	if (node.getDepth() >= 2) {
//		cout << threadID << ": Depth " << node.getDepth() << " exceed max depth." << endl;
//		return false;
//	}

	cout << threadID << ": Resolving " + node.getDomain() << endl;
	host = gethostbyname( node.getDomain().c_str());

	if (host == NULL) {
		cout<< threadID << ": dns resolving failed"<<endl;
		return false;
	}
	//cout<<"Host name :"<< h->h_name<<endl;
	//cout<<"IP Address :"<<inet_ntoa(*((struct in_addr *)h->h_addr))<<endl;

	//create a Socket structure   - "Client Socket"
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Creation Error!");
		cout << threadID << ": Socket Creation Error!" << endl;
		return false;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	int value = 1;

	//if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
	int opt=1;
//	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
//		perror("setsockopt(SO_REUSEADDR) failed");
//		cout << threadID << ": setsockopt(SO_REUSEADDR) failed" << endl;
//		return false;
//	}

//	if (fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0)|O_NONBLOCK) == -1) {
//		perror("Set non-blocking socket Error");
//		return false;
//	}


	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	    perror("Connect Error");
	    cout << threadID << ": Connect Error" << endl;
	    return false;
	}
//	cout << threadID << ": Downloading " + node.getRawString() << endl;
//	cout << (node.getPath() + node.getAttributeURL()).c_str() << endl;
//	cout << node.getDomain().c_str()  << endl;
	char strRequest[1000];
	//cout << "GET " << (node.getPath() + node.getAttributeURL()).c_str() << endl;
	//cout << "Host: " << node.getDomain().c_str() << endl;
	sprintf(strRequest,"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: Mozilla/5.0\r\nCache-Control: max-age=0\r\nAccept: text/*;q=0.3, text/html;q=0.7, text/html;level=1\r\nAccept-Language: en-US,en;q=0.8\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n",
			(node.getPath() + node.getAttributeURL()).c_str(), node.getDomain().c_str());

	//	cout << t2 - t1 << " seconds elapsed\n" << endl;
	clock_t t1 = clock();
	int ret = send(sock,strRequest,sizeof(strRequest),0);

	int flag=1;
	bool flag2 = true;

	while (flag) {
		char buf[8192];
		bzero(buf, sizeof(buf));
		ret = recv(sock, buf, 8191, 0);
		if (ret > 0) {
//			if(htmlString.size() + 8191 > htmlString.max_size()) {
//				cout << htmlString.size() << endl;
//			}
			if (flag2){
				double t = clock() - t1;
				responseTime = 1.0 * t / (CLOCKS_PER_SEC / 1000);
				cout << threadID << ": response time of " << node.getDomain()
						<< " is " << t / (CLOCKS_PER_SEC / 1000) << endl;
				flag2 = false;
			}

			htmlString += buf;
		} else if (ret == 0) {
			flag = 0;
			close (sock);
			return true;

		} else {
			flag = 0;
		}
	}
	cout << threadID << "should not see this!" << endl;
	return false;
}

double WebCrawler::getResponseTime() {

	return responseTime;
}

bool WebCrawler::isRelativeURL(string url){
	if(url.find("mailto:") != string::npos)
		return false;
	return ( url.find("//") == string::npos);
}

bool WebCrawler::isHttpURL(string url){
	if(url.substr(url.length() -4) == ".pdf" || url.substr(url.length() -4) == ".jpg"){
		return false;
	}
    const std::string urlRegexStr = "(http)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))"
                                    "([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?";
    const regex urlRegex(urlRegexStr.c_str());
    return regex_match(url,urlRegex);
}

bool WebCrawler::extractURLs() {

	GumboOutput* output = gumbo_parse(htmlString.c_str());

	_extractURLs(output->root);

	delete output;
	return true;
}

string WebCrawler::relativeToAbsolute(string url){
	string ab_url = "http://" + node.getDomain() + node.getPath();
	string re_url = url;
	string root = "";
	size_t i;

	if('/' == ab_url[ab_url.size()-1])
	    root = ab_url.substr(0,ab_url.size()-1);
	else
		root = ab_url;

	if (url[0] == '/') {
		re_url = "http://" + node.getDomain() + url;
	} else {
		i = re_url.find("./");
		while ( i != string::npos) {
			if(i != 0 && re_url[i-1] == '.'){
				root = root.substr(0, root.rfind('/'));
//				cout << root << endl;
				re_url = re_url.substr(3);
//				cout << re_url << endl;
			} else {
				re_url = re_url.substr(2);
//				cout << re_url << endl;
			}
			i = re_url.find("./");
		}

		re_url = root + '/' + re_url;
	}

	return re_url;
}

void WebCrawler::_extractURLs(GumboNode* gumboNode) {
	if (gumboNode->type != GUMBO_NODE_ELEMENT)
		return;
	GumboAttribute* href;
	if (gumboNode->v.element.tag == GUMBO_TAG_A
			&& (href = gumbo_get_attribute(&gumboNode->v.element.attributes,
					"href"))) {

		URLNode newURL;
		try {
			string url = string(href->value);
			delete href;
			if (isRelativeURL(url)) {
				url = relativeToAbsolute(url);
			}
			//cout << url << endl;

			if (isHttpURL(url)) {

				newURL.setURL(url, node.getDepth() + 1);
				HtmlUrlQueue.push(newURL);
			}
		} catch (exception& e) {
			cout << e.what() << '\n';
		}
	}

	GumboVector* children = &gumboNode->v.element.children;
	for (unsigned int i = 0; i < children->length; ++i){
		_extractURLs(static_cast<GumboNode*>(children->data[i]));
	}

	delete gumboNode;

}
