import sys
import os
import re
import glob
from sage.all import *
from subprocess import Popen, PIPE 

def getpair(x):
  ss = re.split("[\-\.]",x)
  return (int(ss[3]),int(ss[4]))

def gettriple(x): 
  ss = re.split("[\-\.]",x)
  num_lines = sum(1 for line in open(x))
  return (int(ss[3]),int(ss[4]),num_lines)

def combin(x,y):
  return [el0+[el1] for el0 in x for el1 in y] 

def combrec(x,y):
  if len(y) == 0:
    return x
  else:
    return combrec(combin(x,y[0]),y[1:])

def make_unique_set(original_list):
    unique_list = []
    map(lambda x: unique_list.append(sorted(x)) if (sorted(x) not in unique_list) else False, original_list)
    return unique_list

assert(len(sys.argv) == 2)
ind = int(sys.argv[1])-1

countslist = []
for p in Partitions(ind):
  pos = dict()
  for pn in p: 
    pos[pn] = []
    for f in glob.glob("proper/files/real-3-"+str(pn+1)+"-*.g6"):
      pos[pn] += [f]
  valid = True
  for pn in p:
    if pos[pn] == []:
      valid = False
  if valid: 
    pd = list()
    for pn in p:
      pd += [map(lambda x: gettriple(x), pos[pn])] 
    for cd in make_unique_set(combrec([[]],pd)):
      counts = [0,0,1]
      for c in cd:
        counts[0] += c[0]
        counts[1] += c[1]
        counts[2] = counts[2]*c[2]
      print str(counts) 
      countslist += [counts]

rows = set()
cols = set()
for c in countslist:
  cols.add(c[0])
  rows.add(c[1])
ncols = len(cols)
nrows = len(rows)
print "\\begin{center}\n\\begin{table}\n\\tiny"
print "\\begin{tabular}{c|"+("l "*ncols)[:-1]+"}"
print "edges & \\multicolumn{"+str(ncols)+"}{c}{number of vertices $n$} \\\\"
sys.stdout.write("$e$")
for c in sorted(cols):
  sys.stdout.write(" & " + str(c))
print "\\\\\n\\hline"
for r in sorted(rows):
  sys.stdout.write(str(r))
  for c in sorted(cols):
    sys.stdout.write(" & ")
    s = 0
    for elt in countslist:
      if elt[0] == c and elt[1] == r:
        s += elt[2]
    if s != 0:
      sys.stdout.write(str(s)) 
  sys.stdout.write(" \\\\\n")
print "\\end{tabular}"
print "\\caption{Number of $H_{13}$-patterned $(3,"+sys.argv[1]+")$-graphs.}"
print "\\end{table}\n\\end{center}"
