#!/bin/bash
echo "开始删除!"
rm -rf /home/helen/Desktop/ssesearch/odxt1/test
rm -rf /home/helen/Desktop/ssesearch/odxt1/sdb
rm -rf /home/helen/Desktop/ssesearch/odxt1/cdb
rm -rf /home/helen/Desktop/ssesearch/odxt1/2.txt
mkdir  /home/helen/Desktop/ssesearch/odxt1/test
mkdir  /home/helen/Desktop/ssesearch/odxt1/sdb
mkdir  /home/helen/Desktop/ssesearch/odxt1/cdb
touch  /home/helen/Desktop/ssesearch/odxt1/test/client.txt
touch  /home/helen/Desktop/ssesearch/odxt1/sdb/server1.txt
touch  /home/helen/Desktop/ssesearch/odxt1/cdb/server2.txt
touch  /home/helen/Desktop/ssesearch/odxt1/2.txt
make clean 
make exe 

