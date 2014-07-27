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
	mv -f allTag.exe allTag

searchtags:	fastaReader.o	trie.o searchtags.o gzstream/gzstream.o
	${CXX} -o $@ $^ $(LDLIBS) $(LDFLAGS)
	mv -f searchtags.exe searchtags

clean:
	rm -f  trieTag allTag searchtags *.o

