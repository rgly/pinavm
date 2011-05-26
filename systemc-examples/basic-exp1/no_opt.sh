#!/bin/bash
loop=100
sum=0
for ((i = 0; i < $loop ; i += 1))
do
	result=$(../../toplevel/pinavm -b run main.linked.bc -dis-dbg-msg -dis-opt-msg)
	sum=$(($sum + $result))
done 
echo $(($sum / $loop))