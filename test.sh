#!/bin/bash

<<<<<<< HEAD
while [ 1 ]; do leaks ${1} | grep "total leaked bytes."; sleep 1; done
=======
result=$(ps | grep "./webserv" | awk 'NR==1 {print $1; exit}')

while [ 1 ]; do leaks ${result} | grep "total leaked bytes."; sleep 1; done
>>>>>>> 54a03cb892bbd11427fefb987c9f45b34b5a180e
