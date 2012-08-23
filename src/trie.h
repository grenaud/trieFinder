


//************************  trie.h  *********************************
#include <string>
#include <list>
#include <vector>



using namespace std;

template <typename typeOfVariableToHoldInTrieNode> 
struct matches{ 
    list<typeOfVariableToHoldInTrieNode> * listOfDeflinesOfMatches;    
    int numberOfMismatches;
};


template <typename typeOfVariableToHoldInTrieNode> 
class Trie;

template <typename typeOfVariableToHoldInTrieNode>
class TrieNonLeafNode {
public:
     TrieNonLeafNode(char); 

private:
     bool leaf; //if node is leaf
     bool endOfWord; //if node contains a final character up to that point
    char *letters;
    TrieNonLeafNode **ptrs;
    list<typeOfVariableToHoldInTrieNode> * listOfDeflines;    
    friend class Trie<typeOfVariableToHoldInTrieNode>;
};

template <typename typeOfVariableToHoldInTrieNode>
class TrieLeafNode {
public:
    TrieLeafNode(const char*,list<typeOfVariableToHoldInTrieNode> * listOfDeflines);
private:
    bool leaf;
    char *word;
    list<typeOfVariableToHoldInTrieNode> * listOfDeflines;    
    friend class Trie<typeOfVariableToHoldInTrieNode>;
};

template <typename typeOfVariableToHoldInTrieNode>
class Trie {
public:
    Trie(const char*,typeOfVariableToHoldInTrieNode defline);

    void printTrie() {
        *prefix = '\0';
        printTrie(0,root,prefix);
    }
    //void insert(char*);
    void insert(const char*,typeOfVariableToHoldInTrieNode defline);

    bool wordFound(char*);
    vector<struct matches<typeOfVariableToHoldInTrieNode> > * searchForWordMismatch(const char *word,int numberOfMismatches) ;

private:
    void searchMismatch(const char *word,
			TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p,
			int numberOfMismatches,
			int numberOfMismatchesMAX,
			vector<struct matches<typeOfVariableToHoldInTrieNode> > * vectorOfDeflines);


    void addAllSubtree(TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p,
		       int numberOfMismatches,     //number of mismatches we found so far
		       vector<struct matches<typeOfVariableToHoldInTrieNode> > * vectorOfDeflines);


    inline int stringCmp(const char * firstString,
			 const char * secondString,
			 int numberOfMismatches);

    TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *root;
    const int notFound;
    char prefix[80];
    int  position(TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*,char);
    void addCell(char,TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*,int);
    void createLeaf(char,const  char*,TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*,list<typeOfVariableToHoldInTrieNode> * listOfDeflines);
    void printTrie(int,TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*,char*);

};



#define __TRIE_H__
#include "trie.cpp"
#undef  __TRIE_H__

