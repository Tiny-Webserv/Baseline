#!/bin/bash

while [ 1 ]; do leaks ${1} | grep "total leaked bytes."; sleep 1; done