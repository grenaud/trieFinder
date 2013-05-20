all:	searchtags 	fastaReader.o	trie.o allTag #trieTag

trie.o:
	g++ -lm -O3 -c trie.cpp

fastaReader.o:
	g++ -lm -O3 -c fastaReader.cpp

#trieTag:	fastaReader.o	trie.o
#	g++  -lm -O3 -g -o trieTag trieTag.cpp fastaReader.o trie.o

allTag:	fastaReader.o	trie.o
	g++  -lm -O3 -Igzstream/ -g -o allTag allTag.cpp fastaReader.o trie.o gzstream/gzstream.o -lz

searchtags:	fastaReader.o	trie.o
	g++  -lm -O3 -g -o searchtags searchtags.cpp fastaReader.o trie.o


clean:
	rm -f  trieTag allTag searchtags	fastaReader.o	trie.o

