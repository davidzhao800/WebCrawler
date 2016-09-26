#!/bin/bash
sqlite3 urlDB.db > AverageResponseTime.txt <<EOS
	Select urltime.domain,AVG(urltime.time) AS time from urltime group by urltime.domain;
EOS
