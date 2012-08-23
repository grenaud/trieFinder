// Modified from Data structures and algorithms in C++, Boston: PWS 1996; Pacific Grove: Brooks/Cole 2001 (2nd ed.); 
// Sao Paulo: Pioneira Thomson Learning 2002; Beijing: Tsinghua University Press 2003; Gliwice: Helion 2004; Boston: Course Technology 2005 (3rd ed.)
// http://www.mathcs.duq.edu/drozdek/DSinCpp/


//************************  trie.cpp  *********************************
#ifdef __TRIE_H__

#include <iostream>
#include <cstring>
#include <cstdlib>
//#include "trie.h"

using namespace std;


/** 
 * This constructor for TrieLeafNode creates a leaf node
 *
 * \param suffix The remaining characters
 * \param listOfDeflines A reference to a list of defline
 */
template <typename typeOfVariableToHoldInTrieNode>
TrieLeafNode<typeOfVariableToHoldInTrieNode>::TrieLeafNode(const char *suffix,
			   list<typeOfVariableToHoldInTrieNode> * listOfDeflines) {
#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"constr TrieLeafNode "<<endl;
    cout<<"suffix "<<suffix<<endl;
    cout<<"listOfDeflines: "<<endl;
    list<typeOfVariableToHoldInTrieNode>::const_iterator iter;		    
    for (iter  = listOfDeflines->begin(); 
	 iter != listOfDeflines->end(); 
	 iter++){
	cout<<(*iter) << "\t";
    }
    cout<<endl;
#endif

    leaf = true;
    word = new char[strlen(suffix)+1];
    if (word == 0) {
        cerr << "Out of memory2.\n";
        exit(-1);
    }
    strcpy(word,suffix);
    this->listOfDeflines = listOfDeflines;
}


/** 
 * This constructor for TrieNonLeafNode creates a non-leaf node 
 * using a character
 *
 * \param ch The character represented by this non-leaf node
 */
template <typename typeOfVariableToHoldInTrieNode>
TrieNonLeafNode<typeOfVariableToHoldInTrieNode>::TrieNonLeafNode(char ch) {

#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"constr TrieNonLeafNode ch="<<ch<<endl;
#endif


    ptrs = new TrieNonLeafNode*;
    letters = new char[2];
    if (ptrs == 0 || letters == 0) {
        cerr << "Out of memory3.\n";
        exit(1);
    }
    leaf = false;
    endOfWord = false;
    *ptrs = 0;
    *letters = ch;
    *(letters+1) = '\0';
}









/** 
 * This constructor initializes the root 
 * using a character
 *
 * \param word The first string to add
 * \param defline The defline associated with the string
 */

template <typename typeOfVariableToHoldInTrieNode>
Trie<typeOfVariableToHoldInTrieNode>::Trie(const char* word,typeOfVariableToHoldInTrieNode defline) : notFound(-1) {

#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"constr Trie "<<endl;
    cout<<"word "<<word<<endl;
    cout<<"defline "<<defline<<endl;
#endif

    root = new TrieNonLeafNode<typeOfVariableToHoldInTrieNode>(*word); // initialize the root
    //createLeaf(*word,word+1,root);     // to avoid later tests;
    list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
    newListOfDeflines->push_back(defline);
    createLeaf(*word,word+1,root,newListOfDeflines); // to avoid later tests;
}




/** 
 * Looking for ch among the pointers for node p
 * 
 *
 * \param p   The pointer to the TrieNonLeafNode for which we want the pointer 
 * \param ch  The character we are looking after
 */
template <typename typeOfVariableToHoldInTrieNode>
int Trie<typeOfVariableToHoldInTrieNode>::position(TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p, char ch) {
    int i;
    for (i = 0; i < strlen(p->letters) && p->letters[i] != ch; i++);
    if (i < strlen(p->letters))
	return i;
    else 
	return notFound;
}


template <typename typeOfVariableToHoldInTrieNode>
bool Trie<typeOfVariableToHoldInTrieNode>::wordFound(char *word) {
    TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p = root;
    TrieLeafNode<typeOfVariableToHoldInTrieNode> *lf;
    int pos;
    while (true)
        if (p->leaf) {                      // node p is a leaf
            lf = (TrieLeafNode<typeOfVariableToHoldInTrieNode>*) p;         // where the matching
            if (strcmp(word,lf->word) == 0) // suffix of word
		return true;               // should be found;
            else 
		return false;
        }else{
	    if (*word == '\0'){             // the end of word has
		if (p->endOfWord)              // to correspond with
		    return true;              // the endOfWord marker
		else 
		    return false;             // in node p set to true;
	    }else{ 
		if ((pos = position(p,*word)) != notFound &&
		    p->ptrs[pos] != 0) {       // continue
		    p = p->ptrs[pos];              // path, if possible,
		    word++;
		}else{ 
		    return false;                  // otherwise failure;
		}
	    }
	}
}



/** 
 * Creates a new TrieNonLeafNode in the child pointers of p 
 * 
 *
 * \param ch   
 * \param p    
 * \param stop
 */
template <typename typeOfVariableToHoldInTrieNode>
void Trie<typeOfVariableToHoldInTrieNode>::addCell(char ch, TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p, int stop) {

#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"addCell "<<endl;
    cout<<"ch "<<ch<<endl;
#endif

    int i, len = strlen(p->letters);
    char *s = p->letters;
    TrieNonLeafNode<typeOfVariableToHoldInTrieNode> **tmp = p->ptrs;
    p->letters = new char[len+2];
    p->ptrs    = new TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*[len+1];

    if (p->letters == 0 || p->ptrs == 0) {
        cerr << "Out of memory1.\n";
        exit(1);
    }
    for (i = 0; i < len+1; i++)
        p->ptrs[i] = 0;
    if (stop < len){                // if ch does not follow all letters in p,
	for (i = len; i >= stop+1; i--) { // copy from tmp letters > ch;
	    p->ptrs[i]    = tmp[i-1];
	    p->letters[i] = s[i-1];
	}
    }
    p->letters[stop] = ch;
    for (i = stop-1; i >= 0; i--) {           // and letters < ch;
        p->ptrs[i]    = tmp[i];
        p->letters[i] = s[i];
    }
    p->letters[len+1] = '\0';
    delete [] s;
}


/** 
 * Creates a TrieLeafNode with the character ch and the remaining characters suffix 
 * on p
 * 
 *
 * \param ch      The character we will add in the children of p
 * \param suffix  The remaining characters  
 * \param p       The TrieNonLeafNode will have the new TrieLeafNode as child
 */
template <typename typeOfVariableToHoldInTrieNode>
void Trie<typeOfVariableToHoldInTrieNode>::createLeaf(char ch, const char *suffix, TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p,list<typeOfVariableToHoldInTrieNode> * listOfDeflines) {

#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"createLeaf "<<endl;
    cout<<"ch "<<ch<<endl;
    cout<<"suffix "<<suffix<<endl;
    cout<<"listOfDeflines: "<<endl;
    list<typeOfVariableToHoldInTrieNode>::const_iterator iter;		    
    for (iter  = listOfDeflines->begin(); 
	 iter != listOfDeflines->end(); 
	 iter++){
	cout<<(*iter) << "\t";
    }
    cout<<endl;
#endif

    int pos = position(p,ch);
    if (pos == notFound) {
        for (pos = 0; pos < strlen(p->letters) && p->letters[pos] < ch; pos++);  //find the appropriate position for the letter (children are alphabetical order)
        addCell(ch,p,pos); // add the character to the children of TrieNonLeafNode *p
    }

    p->ptrs[pos] = (TrieNonLeafNode<typeOfVariableToHoldInTrieNode>*) new TrieLeafNode<typeOfVariableToHoldInTrieNode>(suffix,listOfDeflines);

}


template <typename typeOfVariableToHoldInTrieNode>
void Trie<typeOfVariableToHoldInTrieNode>::insert(const char *word,typeOfVariableToHoldInTrieNode defline) {

#ifdef DEBUG_CONSTRUCTOR
    cout<<endl<<"insert "<<endl;
    cout<<"word "<<word<<endl;
    cout<<"defline "<<defline<<endl;
#endif

    TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p = root;
    TrieLeafNode<typeOfVariableToHoldInTrieNode> *lf;
    int offset, pos;
    const char *hold = word;



    while (true) {
        if (*word == '\0') {            // if the end of word reached,

#ifdef DEBUG_INSERT
    cout<<endl<<"insert end of word reached"<<endl<<endl;
#endif


	    if (p->endOfWord){
		cout << "Duplicate entry1 " << hold << endl;
		p->listOfDeflines->push_back(defline);		 
	    }else{ 
		p->endOfWord = true;  // set endOfWord to true;
		list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
		newListOfDeflines->push_back(defline);
		p->listOfDeflines=newListOfDeflines;
	    }

#ifdef DEBUG_INSERT
    cout<<endl<<"EXIT insert 1"<<endl<<endl;
#endif

	    return;
        }                               // if position in p indicated
        pos = position(p,*word);
	

        if (pos == notFound) {          // by the first letter of word
#ifdef DEBUG_INSERT
    cout<<endl<<"insert creating new leaf"<<endl<<endl;
#endif

	    list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
	    newListOfDeflines->push_back(defline);
	    //createLeaf(*word,word+1,p);// does not exist, create
	    createLeaf(*word,word+1,p,newListOfDeflines); // does not exist, create

#ifdef DEBUG_INSERT
    cout<<endl<<"EXIT insert 2"<<endl<<endl;
#endif

	    return;                    // a leaf and store in it the
        }else{                               // unprocessed suffix of word;
	    if (pos != notFound &&     // if position *word is
		p->ptrs[pos]->leaf) {  // occupied by a leaf,

#ifdef DEBUG_INSERT
		cout<<endl<<"insert found and occupied by leaf"<<endl;
#endif

		lf = (TrieLeafNode<typeOfVariableToHoldInTrieNode>*) p->ptrs[pos];    // hold this leaf;
		if (strcmp(lf->word,word+1) == 0) {
		    cout << "Duplicate entry2 " << hold << endl;
		    lf->listOfDeflines->push_back(defline);		 
#ifdef DEBUG_INSERT
    cout<<endl<<"EXIT insert 3"<<endl<<endl;
#endif
		    return;
		}
		offset = 0;

		do {
		    pos = position(p,word[offset]);
		    
#ifdef DEBUG_INSERT
		    cout<<endl<<"insert creating non-leaf node word["<<offset<<"]="<<word[offset]<<"#"<<endl;
		    cout<<(strlen(lf->word) == offset)<<endl;
		    cout<<(strlen(word) == offset+1)<<endl;

#endif

		    if (strlen(word) == offset+1) { // word == "ABC", leaf = "ABCDEF" => leaf = "DEF";

 			p->endOfWord = true;			
			list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
			newListOfDeflines->push_back(defline);
			p->listOfDeflines=newListOfDeflines;

			p->ptrs[pos] = new TrieNonLeafNode<typeOfVariableToHoldInTrieNode>(lf->word[offset]);


			createLeaf(lf->word[offset],lf->word + offset+1,p->ptrs[pos],lf->listOfDeflines);
#ifdef DEBUG_INSERT
			cout<<endl<<"EXIT insert 4"<<endl<<endl;
#endif
			return;
		    }else { // word == "ABCDEF", leaf = "ABC" => leaf = "DEF";
			if (strlen(lf->word) == offset) {

			    list<typeOfVariableToHoldInTrieNode> * pointerTemp=((TrieLeafNode<typeOfVariableToHoldInTrieNode>*)p->ptrs[pos])->listOfDeflines;
			    p->ptrs[pos] = new TrieNonLeafNode<typeOfVariableToHoldInTrieNode>(word[offset+1]);
			    p->ptrs[pos]->endOfWord = true;
			    p->ptrs[pos]->listOfDeflines=pointerTemp;
			    

			    list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
			    newListOfDeflines->push_back(defline);
			    createLeaf(word[offset+1],word+offset+2,p->ptrs[pos],newListOfDeflines);


#ifdef DEBUG_INSERT
			    cout<<endl<<"EXIT insert 5"<<endl<<endl;
#endif

			    return;
			}
		    }
		    
		    p->ptrs[pos] = new TrieNonLeafNode<typeOfVariableToHoldInTrieNode>(word[offset+1]);
		    
		    p = p->ptrs[pos];
		    offset++;
		} while (word[offset] == lf->word[offset-1]);
#ifdef DEBUG_INSERT
		cout<<endl<<"out of do loop"<<endl<<endl;
#endif

		offset--;

		// word = "ABCDEF", leaf = "ABCPQR" =>
		//    leaf('D') = "EF", leaf('P') = "QR";
		// check whether there is a suffix left:
		// word = "ABCD", leaf = "ABCPQR" =>
		//    leaf('D') = null, leaf('P') = "QR";
		list<typeOfVariableToHoldInTrieNode> * oldListDefline = new list<typeOfVariableToHoldInTrieNode> (*(lf->listOfDeflines));
		list<typeOfVariableToHoldInTrieNode> * newListOfDeflines= new list<typeOfVariableToHoldInTrieNode>();
		newListOfDeflines->push_back(defline);

		const char *s ;

		if (strlen(word) > offset+2){
		    s = word+offset+2;
		    //strcpy(s,word+offset+2);
		}else{
		    s = "";
		}

		createLeaf(word[offset+1],s,p,newListOfDeflines);
		
		// check whether there is a suffix left:
		// word = "ABCDEF", leaf = "ABCP" =>
		//    leaf('D') = "EF", leaf('P') = null;
		if (strlen(lf->word) > offset+1){
		    s = lf->word+offset+1;
		}else{ 
		    s = "";
		}

		createLeaf(lf->word[offset],s,p,oldListDefline);  //oldDefline);

		delete [] lf->word;
		delete lf;

#ifdef DEBUG_INSERT
    cout<<endl<<"EXIT insert 6"<<endl<<endl;
#endif

		return;
	    }else {

#ifdef DEBUG_INSERT
    cout<<"Insert next non-leaf "<<" char = "<<p->letters[pos]<<endl;
#endif
		
		p = p->ptrs[pos];
		word++;
	    }
	}
    }

#ifdef DEBUG_INSERT
    cout<<endl<<"EXIT insert 7"<<endl<<endl;
#endif

}


template <typename typeOfVariableToHoldInTrieNode>
vector<struct matches<typeOfVariableToHoldInTrieNode> > * 
Trie<typeOfVariableToHoldInTrieNode>::searchForWordMismatch(const char *word,int numberOfMismatches) {
    TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p = root;
    TrieLeafNode<typeOfVariableToHoldInTrieNode> *lf;
    vector<struct matches<typeOfVariableToHoldInTrieNode> > * toReturn=new vector<struct matches<typeOfVariableToHoldInTrieNode> >();
#ifdef DEBUG_SEARCH
    cout<<"word "<<word<<endl;
#endif    
    searchMismatch(word,
		   p,
		   0,
		   numberOfMismatches,
		   toReturn);
    return toReturn;
}

template <typename typeOfVariableToHoldInTrieNode>
void Trie<typeOfVariableToHoldInTrieNode>::addAllSubtree(TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p,
							 int numberOfMismatches,     //number of mismatches we found so far
							 vector<struct matches<typeOfVariableToHoldInTrieNode> > * vectorOfDeflines){

#ifdef DEBUG_SEARCH
    cout<<"addAllSubtree begin on "<<endl;
#endif    

    if (p->leaf) {                      // node p is a leaf

#ifdef DEBUG_SEARCH
	cout<<"addAllSubtree leaf matches "<< vectorOfDeflines->size()<<endl;
#endif    
	TrieLeafNode<typeOfVariableToHoldInTrieNode> *lf;
	lf = (TrieLeafNode<typeOfVariableToHoldInTrieNode>*) p;         // where the matching
	struct matches<typeOfVariableToHoldInTrieNode> myNewMatch;

#ifdef DEBUG_SEARCH
	cout<<"size def "<<lf->listOfDeflines->size()<<endl;
#endif    
	myNewMatch.listOfDeflinesOfMatches = lf->listOfDeflines;
	myNewMatch.numberOfMismatches=(numberOfMismatches);
	vectorOfDeflines->push_back( myNewMatch );
	return;    

#ifdef DEBUG_SEARCH
	cout<<"addAllSubtree leaf matches "<< vectorOfDeflines->size()<<endl;
#endif    


    }else{
	if (p->endOfWord){

#ifdef DEBUG_SEARCH
	    cout<<"addAllSubtree end of word "<<endl;
	    cout<<"size def "<<p->listOfDeflines->size()<<endl;
#endif    

	    struct matches<typeOfVariableToHoldInTrieNode> myNewMatch;
	    myNewMatch.listOfDeflinesOfMatches = p->listOfDeflines;
	    myNewMatch.numberOfMismatches=(numberOfMismatches);
	    vectorOfDeflines->push_back( myNewMatch );
	}
	for (int i = 0; i < strlen(p->letters) ; i++){
#ifdef DEBUG_SEARCH
	    cout<<"addAllSubtree recursive p->letters["<<i<<"]="<<p->letters[i]<<endl;
#endif    
	    addAllSubtree(p->ptrs[i],
			  numberOfMismatches,
			  vectorOfDeflines);
	}
    }
}

/*
 * Recursevely searches the trie for 
 *
 *
 * \param word Word left to find
 * \param p    Current node
 * \param numberOfMismatches     Number of mismatches we found so far
 * \param numberOfMismatchesMAX  Number of mismatches we allow
 * \param vectorOfDeflines Vector of matches we have found so far
 */
template <typename typeOfVariableToHoldInTrieNode>
void Trie<typeOfVariableToHoldInTrieNode>::searchMismatch(const char *word,
							  TrieNonLeafNode<typeOfVariableToHoldInTrieNode> *p,
							  int numberOfMismatches,     //number of mismatches we found so far
							  int numberOfMismatchesMAX,  //number of 
							  vector<struct matches<typeOfVariableToHoldInTrieNode> > * vectorOfDeflines) {


    TrieLeafNode<typeOfVariableToHoldInTrieNode> *lf;
#ifdef DEBUG_SEARCH
    cout<<endl<<"word "<<word<<endl;
    cout<<"nom    "<<numberOfMismatches<<endl;
    cout<<"nomMAX "<<numberOfMismatchesMAX<<endl;

#endif    

    if (p->leaf) {                      // node p is a leaf
#ifdef DEBUG_SEARCH
    cout<<"leaf "<<endl;
#endif    

	lf = (TrieLeafNode<typeOfVariableToHoldInTrieNode>*) p;         // where the matching
	int resultOfCMP=stringCmp(word,lf->word,numberOfMismatchesMAX-numberOfMismatches);
#ifdef DEBUG_SEARCH
	cout<<"resultOfCMP "<<resultOfCMP<<endl;
#endif    

	if ( resultOfCMP != -1){
	    struct matches<typeOfVariableToHoldInTrieNode> myNewMatch;
	    //myNewMatch.deflinesOfMatches = string(*(lf->deflineAssociatedWithString));
	    myNewMatch.listOfDeflinesOfMatches = lf->listOfDeflines;
	    myNewMatch.numberOfMismatches=(numberOfMismatches+resultOfCMP);
	    vectorOfDeflines->push_back( myNewMatch );
	}
	return;
    }

    //recursive step
    else{ 
	if (*word == '\0') {

#ifdef DEBUG_SEARCH
	    cout<<"end of word p->endOfWord="<<p->endOfWord<<endl;
#endif    
	    addAllSubtree(p,numberOfMismatches,vectorOfDeflines);
	    return;
	}

	char ch=word[0];
	int i;

	for (i = 0; i < strlen(p->letters) ; i++){
#ifdef DEBUG_SEARCH
	    cout<<"word "<<word<<endl;
	    cout<<"p->letters["<<i<<"]="<<p->letters[i]<<endl;
#endif 

	    if( p->letters[i] == ch 
#ifdef USE_NS
		|| 
		(p->letters[i] == 'N' )//|| ch == 'N' )
#endif
		){
		searchMismatch(word+1,
			       p->ptrs[i],
			       numberOfMismatches,
			       numberOfMismatchesMAX,
			       vectorOfDeflines);
	    }else{
		if(numberOfMismatches != numberOfMismatchesMAX ){
		    searchMismatch(word+1,
				   p->ptrs[i],
				   (numberOfMismatches+1),
				   numberOfMismatchesMAX,
				   vectorOfDeflines);			       			       
		}
	    }
	}
    }
}


/*
 * 
 *
 *
 * \param firstString  The first string
 * \param secondString The second string
 * \param numberOfMismatches Maximum number 
 * \return The number of mismatches if the string is found, -1 otherwise
 */
template <typename typeOfVariableToHoldInTrieNode>
inline int Trie<typeOfVariableToHoldInTrieNode>::stringCmp(const char * firstString,  //from the "chromosome"
							   const char * secondString, //from the tree
							   int numberOfMismatches){
    int i=0;
    int mismatchesFound=0;

#ifdef DEBUG_STRING_CMP
    cout<<"firstString  "<<firstString<<endl;
    cout<<"secondString "<<secondString <<endl;
    cout<<"numberOfMismatches "<<numberOfMismatches<<endl;
#endif

    while(true){
#ifdef DEBUG_STRING_CMP
	cout<<"firstString["<<i<<"] "<<firstString[i]<<endl;
	cout<<"secondString["<<i<<"] "<<secondString[i]<<endl;
	cout<<"mismatchesFound "<<mismatchesFound<<endl<<endl;
#endif

	if(firstString[i] == '\0' ){ //matches prefix
	    return mismatchesFound;
	}

	if(firstString[i] == '\0' &&
	   secondString[i] == '\0'){
	    return mismatchesFound;
	}

	if(firstString[i] == '\0' ||
	   secondString[i] == '\0'){
	    return -1;
	}

	if(firstString[i] != secondString[i]  
#ifdef USE_NS
	   &&
	   !( secondString[i] == 'N' )
#endif
 ){
	    mismatchesFound++;
	}

	if( mismatchesFound > numberOfMismatches){
	    return -1;
	}

	i++;
    }

}

#endif
