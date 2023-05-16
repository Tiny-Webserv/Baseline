#!/bin/bash

result=$(ps | grep "./webserv" | awk 'NR==1 {print $1; exit}')

while [ 1 ]; do leaks ${result} | grep "total leaked bytes."; sleep 1; done