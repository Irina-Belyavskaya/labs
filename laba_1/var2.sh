#!/bin/bash

find "$1" -type f -printf "%M %s %p \n"
find "$1"  -type f | wc -l
	

