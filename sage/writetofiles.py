import sys
import os
from sage.all import *
from subprocess import Popen, PIPE 

for line in sys.stdin:
  l = line.split(" ")
  ind = int(l[0])
  g = l[1]
  G = Graph(g[:-1])
  filename = "files/real-3-" + str(ind + 1) + "-" + str(G.order()) + "-" + str(G.size()) + ".g6"
  f = open(filename, "a")
  f.write(g)
