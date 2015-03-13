#!/usr/bin/python
# This script expects 'standard' disksim trace format and sorts update counts of unique address in reverse order

from collections import Counter
import sys

arg1=sys.argv[1]
arg2=sys.argv[2]
file = open( arg1, 'r')
output = open( arg2, 'w')

addr=[]

for line in file.readlines():
    addr.append(line)

sorted_list =  Counter(addr).most_common()
#addr.sort( key = lambda x:-x[1])

rank=1
for item in sorted_list:
      print>>output, rank, item[1] ,item[0],
      rank=rank+1

file.close
output.close

