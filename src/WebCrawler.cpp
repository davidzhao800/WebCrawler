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

/**
    download HTML file

    @return whether there is err downloading html
*/
bool WebCrawler::downloadHTML() {

	int sock, bytes_recieved;
	char send_data[1024],recv_data[1024];
	struct hostent *host;
	struct sockaddr_in server_addr;

	cout << threadID << ": Resolving " + node.getDomain() << endl;
	host = gethostbyname( node.getDomain().c_str());

	// handle dns resolve err
	if (host == NULL) {
		cout<< threadID << ": dns resolving failed"<<endl;
		return false;
	}

	//create a Socket structure   - "Client Socket"
	// handle socket create err
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket Creation Error!");
		cout << threadID << ": Socket Creation Error!" << endl;
		return false;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr.sin_zero),8);

	//handle connect error
	if (connect(sock, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
	    perror("Connect Error");
	    cout << threadID << ": Connect Error" << endl;
	    return false;
	}

	char strRequest[1000];

	sprintf(strRequest,"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\nUser-Agent: Mozilla/5.0\r\nCache-Control: max-age=0\r\nAccept: text/*;q=0.3, text/html;q=0.7, text/html;level=1\r\nAccept-Language: en-US,en;q=0.8\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.3\r\n\r\n",
			(node.getPath() + node.getAttributeURL()).c_str(), node.getDomain().c_str());

	clock_t t1 = clock();
	int ret = send(sock,strRequest,sizeof(strRequest),0);

	int flag=1;
	bool flag2 = true;

	while (flag) {
		char buf[8192];
		bzero(buf, sizeof(buf));
		ret = recv(sock, buf, 8191, 0);
		if (ret > 0) {
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

	return false;
}

double WebCrawler::getResponseTime() {

	return responseTime;
}

/**
    check whether the url is relative: check whether the url contains "//"

    @param string url
    @return whether the url is relative
*/
bool WebCrawler::isRelativeURL(string url){
	if(url.find("mailto:") != string::npos)
		return false;
	return ( url.find("//") == string::npos);
}

/**
    check whether the url use http protocol

    @param string url
    @return whether the url use http protocol
*/
bool WebCrawler::isHttpURL(string url){
	if(url.substr(url.length() -4) == ".pdf" || url.substr(url.length() -4) == ".jpg"){
		return false;
	}
    const std::string urlRegexStr = "(http)://([\\w_-]+(?:(?:\\.[\\w_-]+)+))"
                                    "([\\w.,@?^=%&:/~+#-]*[\\w@?^=%&/~+#-])?";
    const regex urlRegex(urlRegexStr.c_str());
    return regex_match(url,urlRegex);
}

/**
    extract url using gumbo

    @return whether there is error
*/
bool WebCrawler::extractURLs() {

	GumboOutput* output = gumbo_parse(htmlString.c_str());

	_extractURLs(output->root);

	delete output;
	return true;
}

/**
    parse relative url to absolute url
    this will handle three kind of relative url:
    	href = "/something/index.html"
    	href = "./index.html"
    	href = "../index.html"
    and any mix use of them

	@param string url
    @return whether there is error
*/
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
				re_url = re_url.substr(3);
			} else {
				re_url = re_url.substr(2);
			}
			i = re_url.find("./");
		}

		re_url = root + '/' + re_url;
	}

	return re_url;
}

/**
    extract url recursively
*/
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

	//delete gumboNode;

}
