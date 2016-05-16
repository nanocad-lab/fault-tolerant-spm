#!/bin/bash

temp_file=$(mktemp)
shuf -n $1 -i 536870912-536936448 > $temp_file
( echo "obase=16" ; cat $temp_file ) | bc > addresses.txt
sed -i -e 's/^/0x/' addresses.txt
rm ${temp_file}
