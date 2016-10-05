This Multiple thread webcrawler is for school project. 

Additional features:
Memory friendly, low memory leaks, visiting 1000URL less than 100M
Smartly Recognise relative path eg. "./.././../../haha.html"
Store Average response time

Files in this directory:
	Debug/        		store the compiled binaries
	README.md
	clearDB.sh		script to clear the database
	getResponseTime.sh	script to generate a txt file called 
				AverageResponseTime.txt containing all the
				average response time
	depth.conf		config how deep crawler should go
	timeout.conf		config timeout value between requests
	thread_number.conf	config number of threads
	seed.txt		seed urls
	urlDB.db		sqlite3 database file
	src/
		ThreadPool.cpp
		ThreadPool.h
		WebCrawler.cpp
		WebCrawler.h
		URLNode.cpp
		URLNode.h
		main.cpp


This project use gumbo-parser library. For Mac, just use brew to install it
	$ brew install gumbo
Otherwise, you may change the makefile to modify the path of this library. Currently, the path is:
	-L/usr/local/Cellar/gumbo-parser/0.10.1/lib


How to compile and run the crawler:

1. Change config files and add seed urls.
2. Compile the source file, run
	$ cd Debug
	$ make all
	$ cd ..
3. Run the Web Crawler:
	$ ./Debug/WebCrawler
4. Get average response time
	$ ./GetResponseTime.sh
5. Clear the database
	$ ./clearDB.sh
