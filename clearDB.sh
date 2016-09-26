#!/bin/bash
sqlite3 urlDB.db <<EOS
     delete from urlHash;
     delete from urlTime;
EOS
