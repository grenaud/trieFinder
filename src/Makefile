SHELL := /bin/bash

CXXFLAGS =   -lm -O3 -lz  -Igzstream/  -c
CXX      = g++ #-g -pg
LDFLAGS  = -lz


all:	searchtags  allTag #trieTag


%.o: %.cpp
	${CXX} -Wall ${CXXFLAGS} $^ -o $@



#trieTag:	fastaReader.o	trie.o
#	g++  -lm -O3 -g -o trieTag trieTag.cpp fastaReader.o trie.o


allTag:	fastaReader.o	trie.o allTag.o gzstream/gzstream.o
	${CXX}  -o $@ $^ $(LDLIBS) $(LDFLAGS)
	if [ -f allTag.exe ]; 	then	mv -f allTag.exe allTag; fi

searchtags:	fastaReader.o	trie.o searchtags.o gzstream/gzstream.o
	${CXX} -o $@ $^ $(LDLIBS) $(LDFLAGS)
	if [ -f searchtags.exe ]; then	mv -f searchtags.exe searchtags; fi

clean:
	rm -f  trieTag allTag searchtags *.o

