# Optimized flags:
FLAGS=-O3 -DNDEBUG -I$(NAUTYDIR) -I./
# Debug-flags
#FLAGS=-g -I$(NAUTYDIR) -I./

all: proper generator

graph.o: graph.c graph.h
	cc -c $(FLAGS) -o graph.o graph.c

pattern.o: pattern.c pattern.h
	cc -c $(FLAGS) -o pattern.o pattern.c

essentialinfo.o: essentialinfo.c essentialinfo.h
	cc -c $(FLAGS) -o essentialinfo.o essentialinfo.c

stitching.o: stitching.c stitching.h
	cc -c $(FLAGS) -o stitching.o stitching.c

proper.o: proper.c
	cc -c $(FLAGS) -o proper.o proper.c

proper: graph.o pattern.o essentialinfo.o stitching.o proper.o
	cc $(FLAGS) -o proper graph.o pattern.o essentialinfo.o stitching.o proper.o \
	$(NAUTYDIR)/naututil.o \
	$(NAUTYDIR)/nauty.o \
	$(NAUTYDIR)/nautil.o \
	$(NAUTYDIR)/naugraph.o \
	$(NAUTYDIR)/schreier.o $(NAUTYDIR)/naurng.o \
	$(NAUTYDIR)/gtools.o \
	$(NAUTYDIR)/nausparse.o

generator: generator.c
	cc $(FLAGS) -c -o generator.o generator.c 
	cc $(FLAGS) -o generator generator.o $(NAUTYDIR)/naututil.o \
	$(NAUTYDIR)/nauty.o \
	$(NAUTYDIR)/nautil.o \
	$(NAUTYDIR)/naugraph.o \
	$(NAUTYDIR)/schreier.o $(NAUTYDIR)/naurng.o \
	$(NAUTYDIR)/gtools.o \
	$(NAUTYDIR)/nausparse.o

clean:
	rm -f graph.o pattern.o essentialinfo.o stitching.o proper.o proper generator.o generator
