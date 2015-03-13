#!/usr/bin/python

import sys
import numpy as np
import math

def writecount( t, N, i ):
    x0=(1.33*t-1)*(i-1)+1
    wc=np.log(N)-np.log(x0)
    wc=1.5*t*wc/(1+1/(1.33*t))
    return round(wc,0)

if len(sys.argv) != 5:
    print " arg 1 : t "
    print " arg 2 : N "
    print " arg 3 : log file "
    print " arg 4 : # lines in log "
    sys.exit()

t=int(sys.argv[1])
N=int(sys.argv[2])
log_file = open( sys.argv[3], 'w' )
n=int(sys.argv[4])

print "t =",t
print "N =",N
print "n =",n
for i in range(1,n):
    print>>log_file, i, writecount(t,N,i)

log_file.close

