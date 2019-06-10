import sys 
from sage.all import *

sum = 0
for line in sys.stdin:
  G = Graph(line) 
  print line[:-1] + " has " + str(len(G.automorphism_group().orbits())) + " orbits."
  sum += len(G.automorphism_group().orbits())

print "totally : " + str(sum)
