#include <algorithm>
#include <vector>
#include <iostream>
#include <climits>

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"
#include <gzstream.h>
#include <fstream>



#include "trie.h"
#include "fastaReader.h"



#define DATABASE_defline_MAXSIZE             1000000000
#define DATABASE_defline_length_MAXSIZE      10000000
#define DATABASE_sequence_MAXSIZE            10000000000
#define DATABASE_sequence_length_MAXSIZE     10000000

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) > (b) ? (a) : (b))

//#define WITH_COMMENTS

//#define	DEBUG_EACH_WORD
// #define	DEBUG_TRIE
//#define	DEBUG_KEEP_ON_DISK

#define	ALLOW_EMPTY_LINES true

int NUMBER_OF_MISMATCHES;
int SIZE_RESTRICTION_SITE=4;


using namespace std;




/*
 * Checks to see if [bp] is a valid DNA base pair
 *
 * \param bp The character to check 
 * \return 1 if [bp] is either ACGT either lowercase or uppercase 
 */
inline bool validDNABp(char bp){
    if (bp == 'A') {
	return true;
    }
    if (bp == 'T') {
	return true;
    }
    if (bp == 'G') {
	return true;
    }
    if (bp == 'C') {
	return true;
    }
    if (bp == 'N') {
	return true;
    }

    if (bp == 'a') {
	return true;
    }
    if (bp == 't') {
	return true;
    }
    if (bp == 'g') {
	return true;
    }
    if (bp == 'c') {
	return true;
    }
    if (bp == 'n') {
	return true;
    }

    return false;
}



/*
 * Checks to see if each character of [substring1] is a valid DNA base pair
 * over a length of lengthString1
 *
 * \param substring1 A pointer toward the string to check
 * \param lengthString1 The length over which to check
 * \return 1 if [bp] is either ACGT either lowercase or uppercase 
 */
inline bool validDNAString(const char * substring1,
			   int lengthString1){
    for(int i=0;i<lengthString1;i++)
	if(!validDNABp(substring1[i]))
	    return false;
    return true;
}




/*
 * Checks to see if [bp] is a valid DNA base pair
 *
 * \param bp The character to check 
 * \return 1 if [bp] is either ACGT either lowercase or uppercase 
 */
inline bool validDNABpNoNs(char bp){
    if (bp == 'A') {
	return true;
    }
    if (bp == 'T') {
	return true;
    }
    if (bp == 'G') {
	return true;
    }
    if (bp == 'C') {
	return true;
    }

    if (bp == 'a') {
	return true;
    }
    if (bp == 't') {
	return true;
    }
    if (bp == 'g') {
	return true;
    }
    if (bp == 'c') {
	return true;
    }

    return false;
}



/*
 * Checks to see if each character of [substring1] is a valid DNA base pair
 * over a length of lengthString1
 *
 * \param substring1 A pointer toward the string to check
 * \param lengthString1 The length over which to check
 * \return 1 if [bp] is either ACGT either lowercase or uppercase 
 */
inline bool validDNAStringNoNs(const char * substring1,
			       int lengthString1){
    for(int i=0;i<lengthString1;i++)
	if(!validDNABpNoNs(substring1[i]))
	    return false;
    return true;
}



/*
 * Tries to match the [lengthString1] first characters of [substring1] to [substring2] with at most [maxNumberMismatches]
 *
 * \param substring1           A pointer to the string representing the restriction site
 * \param lengthString1        Length of the string representing the restriction site
 * \param substring2           A pointer to the string in the database file
 * \param shiftForString2      Shift for the string in the database file
 * \param maxNumberMismatches  Maximum number of mismatches allowed.
 * \return 1 if [substring1] matches [substring2] (starting at [shiftForString2]) over [lengthString1] with at most [maxNumberMismatches], return 0 otherwise
 */
inline int matchString(const char * substring1,
		       int lengthString1,
		       const char * substring2,
		       int shiftForString2,
		       int maxNumberMismatches){
    int i;
    int numberMismatches=0;

    for(i=0;i<lengthString1;i++){
	if(!validDNABpNoNs(substring2[i+shiftForString2]))
	    return -1;

	if(substring1[i] != substring2[i+shiftForString2] ){
	    numberMismatches++;
	    if(numberMismatches > maxNumberMismatches){
		return -1;
	    }
	}
    }

    return numberMismatches;
}


/*
 * Returns the DNA complement of [bp] 
 *
 * \param bp The character for which the complement will be returned 
 * \return The complement of [bp]
 */
inline char dnaComplement(char bp){
    if (bp == 'A') {
	return 'T';
    }
    if (bp == 'T') {
	return 'A';
    }

    if (bp == 'C') {
	return 'G';
    }
    if (bp == 'G') {
	return 'C';
    }
    if (bp == 'N') {
	return 'N';
    }


    if (bp == 'a') {
	return 't';
    }
    if (bp == 'c') {
	return 'g';
    }
    if (bp == 'g') {
	return 'c';
    }
    if (bp == 't') {
	return 'a';
    }
    if (bp == 'n') {
	return 'n';
    }
	
    return bp;
    
}

/*
 * Returns the reverse complement of [stringToReverse]
 *
 * \param [stringToReverse] A pointer to the string to reverse
 * \param [lengthOfString]  Number of characters in the string to reverse
 * \return A pointer to a string (on the heap) containing the reverse complement of [stringToReverse] of length [lengthOfString]
 */
char * computeReverseComplement(const char * stringToReverse,int offset,int lengthOfString){
    int i;
    //char * stringToReturn=(char *)malloc(sizeof(char)*(lengthOfString+1));
    char * stringToReturn=new char [lengthOfString+1];
    for(i=(lengthOfString-1);i>=0;i--){
	// cout<<stringToReverse[offset+i]<<endl;
	// cout<<(offset+i)<<endl;
	stringToReturn[ (lengthOfString-1)-i  ] = dnaComplement(stringToReverse[offset+i]);
    }
    stringToReturn[ lengthOfString ]='\0';
    return stringToReturn;
}



/*
 * Prints [lengthOfString] characters of the string [stringToPrint] (on cout)
 *
 * \param [stringToPrint]  A pointer to the string to print
 * \param [lengthOfString] Number of characters to print
 */
inline void printNChar(ostream & outstream,char * stringToPrint,int lengthOfString){
    int i;
    char * pointStr=stringToPrint;

    for(i=0;i<lengthOfString;i++){
	outstream<<*(pointStr++);
    }
}








inline void processLines(char * seqbuffer, 
			 unsigned int positionInBuffer,
			 int SIZE_TAGSIZE,
			 int SIZE_RESTRICTION_SITE,  
			 char * restrictionSiteSequence,
			 string & deflineInput,
			 ogzstream & fhPerfect,
			 ogzstream & fhMismatchA,
			 ogzstream & fhMismatchC,
			 ogzstream & fhMismatchG,
			 ogzstream & fhMismatchT){
    int    returnCodeByMatchingFunction;

    for(unsigned int j=0;
	j<=(positionInBuffer-SIZE_RESTRICTION_SITE);
	j++){		
	returnCodeByMatchingFunction=matchString(restrictionSiteSequence, 
						 SIZE_RESTRICTION_SITE,
						 seqbuffer,
						 j,
						 NUMBER_OF_MISMATCHES);
	if(returnCodeByMatchingFunction != -1 ){			
#ifdef DEBUG_EACH_WORD
	    cout<<seqbuffer[j]<<seqbuffer[j+1]<<seqbuffer[j+2]<<seqbuffer[j+3]<<endl;
#endif
	    //printing forward strand
	    if( j<=(positionInBuffer-SIZE_TAGSIZE) ){
		// cout<<"forward\t"<<returnCodeByMatchingFunction<<endl;

		if(returnCodeByMatchingFunction == 0){
		    fhPerfect<<deflineInput<<"\t";
		    fhPerfect<<( j  )<<"\t";
		    printNChar(fhPerfect,seqbuffer+j,SIZE_TAGSIZE);
		    // cout<<endl;
		    // printNChar(cout,seqbuffer+j,SIZE_TAGSIZE);
		    // cout<<endl;
		    //				    fhPerfect<<computeReverseComplement(seqbuffer,(j-SIZE_TAGSIZE),SIZE_TAGSIZE)<<"\t";
		    fhPerfect<<"\t+"<<endl;
		    // break;
		}else{
				    
				    
		    // cout<<"test"<<endl;
		    // printNChar(cout,seqbuffer+j,SIZE_TAGSIZE);
		    // cout<<endl;
		    switch( seqbuffer[j] ){
		    case 'A':
			fhMismatchA<<deflineInput<<"\t";
			fhMismatchA<<( j  )<<"\t";
			//fhMismatchA<<computeReverseComplement(seqbuffer,j-SIZE_TAGSIZE,SIZE_TAGSIZE)<<"\t";
			printNChar(fhMismatchA,seqbuffer+j,SIZE_TAGSIZE);
			fhMismatchA<<"\t+"<<endl;
			break;
		    case 'C':
			fhMismatchC<<deflineInput<<"\t";
			fhMismatchC<<( j  )<<"\t";
			//fhMismatchC<<computeReverseComplement(seqbuffer,j-SIZE_TAGSIZE,SIZE_TAGSIZE)<<"\t";
			printNChar(fhMismatchC,seqbuffer+j,SIZE_TAGSIZE);
			fhMismatchC<<"\t+"<<endl;
			break;
		    case 'G':
			fhMismatchG<<deflineInput<<"\t";
			fhMismatchG<<( j  )<<"\t";
			//fhMismatchG<<computeReverseComplement(seqbuffer,j-SIZE_TAGSIZE,SIZE_TAGSIZE)<<"\t";
			printNChar(fhMismatchG,seqbuffer+j,SIZE_TAGSIZE);
			fhMismatchG<<"\t+"<<endl;
			break;
		    case 'T':
			fhMismatchT<<deflineInput<<"\t";
			fhMismatchT<<( j  )<<"\t";
			//fhMismatchT<<computeReverseComplement(seqbuffer,j-SIZE_TAGSIZE,SIZE_TAGSIZE)<<"\t";
			printNChar(fhMismatchT,seqbuffer+j,SIZE_TAGSIZE);
			fhMismatchT<<"\t+"<<endl;
			break;
		    default:
			cerr<<( j  )<<"\t";
			cerr<<"ERROR in sequence: "<<endl;
			printNChar(cerr,seqbuffer+j,SIZE_TAGSIZE);
			cerr<<"\t"<<returnCodeByMatchingFunction<<endl;
			exit(1);
		    }
		}
	
	    }

	    //printing reverse complement
	    if(j>=SIZE_TAGSIZE-SIZE_RESTRICTION_SITE){
		if(returnCodeByMatchingFunction == 0){
		    fhPerfect<<deflineInput<<"\t";
		    fhPerfect<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
		    fhPerfect<<computeReverseComplement(seqbuffer,(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE),SIZE_TAGSIZE)<<"\t";
		    fhPerfect<<"-"<<endl;
		    // break;
		}else{
				    
		    char * toprint=computeReverseComplement(seqbuffer,j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE);
		    // printNChar(cout,seqbuffer+(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE),SIZE_TAGSIZE);
		    // cout<<endl;
		    // // printNChar(seqbuffer+j,SIZE_TAGSIZE);
		    // // cout<<endl;
		    //cout<<toprint<<endl;

		    switch( toprint[0] ){
		    case 'A':
			fhMismatchA<<deflineInput<<"\t";
			fhMismatchA<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
			fhMismatchA<<toprint<<"\t";
			fhMismatchA<<"-"<<endl;
			break;
		    case 'C':
			fhMismatchC<<deflineInput<<"\t";
			fhMismatchC<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
			fhMismatchC<<toprint<<"\t";
			fhMismatchC<<"-"<<endl;
			break;
		    case 'G':
			fhMismatchG<<deflineInput<<"\t";
			fhMismatchG<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
			fhMismatchG<<toprint<<"\t";
			fhMismatchG<<"-"<<endl;
			break;
		    case 'T':
			fhMismatchT<<deflineInput<<"\t";
			fhMismatchT<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
			fhMismatchT<<toprint<<"\t";
			fhMismatchT<<"-"<<endl;
			break;
		    default:
			cerr<<( j - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE) )<<"\t";
			cerr<<"ERROR in sequence: "<<toprint<<"\t"<<returnCodeByMatchingFunction<<endl;
			exit(1);
		    }
		}

	    }//else not perfect
	}//if sufficient distance j

			
    }//end for loop
		    
}


int main(int argc, char *argv[]){
    char  databaseFileName [1000];
    int i;
    unsigned long j;


    int SIZE_TAGSIZE = 0 ;

    //char line[1000];
    string line;

    // FILE *fDatabase;
    ifstream fDatabase;
    

    int endCoordinatesForNextString;
    // unsigned long int counterOfChar=0;

    char inputFileTags [1000];

              
    char * DATABASE_defline;
    int  * DATABASE_defline_length;

    char * DATABASE_sequence;
    int  * DATABASE_sequence_length;


    unsigned long DATABASE_defline_ptr;
    unsigned long DATABASE_sequence_ptr;


    Trie<string> * trieOftagSequences;
    Trie<string> * trieOftagSequencesRC;

    char tagSequence[1000];
    char tagSequenceRC[1000];

    char tagDefline[1000];




    int indexOfLineInDatabase;
    int numberOfLineInDatabase;

    int indexInTheInputFile=0;
    int numberOfLinesInTheInputFile=0;
    char restrictionSiteSequence[ SIZE_RESTRICTION_SITE ];
    char filePerfect  [ 1000 ];
    char fileMismatchA [ 1000 ];
    char fileMismatchC [ 1000 ];
    char fileMismatchG [ 1000 ];
    char fileMismatchT [ 1000 ];

    // bool keepOnDisk=true;

    NUMBER_OF_MISMATCHES=-1;

    /******************************************/
    /*                                        */
    /*      PROCESSING COMMAND LINE ARGUMENTS */
    /*                                        */
    /******************************************/

    for (i = 1; i < argc; i++){
	//cout<<"i "<<i<<endl;
	//database
	if(strcmp(argv[i],"-d") == 0){
	    if( i < argc ){
		strcpy(databaseFileName,  argv[i+1]);
	    }else{
		cerr<<"please enter a file name"<<endl;
		exit(1);
	    }
	}



	//number of mismatches
	if(strcmp(argv[i],"-m") == 0){
	    if( i < argc ){
		NUMBER_OF_MISMATCHES=atoi(argv[i+1]);
	    }else{
		cerr<<"please enter the number of mismatches"<<endl;
		exit(1);
	    }
	}

	if(strcmp(argv[i],"-l") == 0){
	    if( i < argc ){
		SIZE_TAGSIZE=atoi(argv[i+1]);
	    }else{
		cerr<<"please enter the number of mismatches"<<endl;
		exit(1);
	    }
	}

	//number of mismatches
	if(strcmp(argv[i],"-r") == 0){
	    if( i < argc ){
		//cout<<"#" <<argv[i+1]<<"#"<<endl;
		if(strlen(argv[i+1]) != SIZE_RESTRICTION_SITE){
		    cerr<<"The size of the restriction site is not equal to the one defined"<<endl;
		    exit(1);
		}		    
		strcpy(restrictionSiteSequence,argv[i+1]);
	    }else{
		cerr<<"please enter the number of mismatches"<<endl;
		exit(1);
	    }
	}


	if(strcmp(argv[i],"-o") == 0){
	    if( i < argc ){
		strcpy(filePerfect,argv[i+1]);
	    }else{
		cerr<<"please enter the name of the file"<<endl;
		exit(1);
	    }
	}

	if(strcmp(argv[i],"-p1") == 0){
	    if( i < argc ){
		strcpy(fileMismatchA,argv[i+1]);
	    }else{
		cerr<<"please enter the name of the file"<<endl;
		exit(1);
	    }
	}

	if(strcmp(argv[i],"-p2") == 0){
	    if( i < argc ){
		strcpy(fileMismatchC,argv[i+1]);
	    }else{
		cerr<<"please enter the name of the file"<<endl;
		exit(1);
	    }
	}

	if(strcmp(argv[i],"-p3") == 0){
	    if( i < argc ){
		strcpy(fileMismatchG,argv[i+1]);
	    }else{
		cerr<<"please enter the name of the file"<<endl;
		exit(1);
	    }
	}

	if(strcmp(argv[i],"-p4") == 0){
	    if( i < argc ){
		strcpy(fileMismatchT,argv[i+1]);
	    }else{
		cerr<<"please enter the name of the file"<<endl;
		exit(1);
	    }
	}


    }

    ogzstream fhPerfect;
    ogzstream fhMismatchA;
    ogzstream fhMismatchC;
    ogzstream fhMismatchG;
    ogzstream fhMismatchT;

    if( argc < 2 ||  strlen(databaseFileName) < 2 || strcmp(argv[1],"-h") == 0){
	cout<<"This program will built a list of all the tags found in a genome file (fasta) beginning with a restriction enzyme sequence.\nUsage:"<<endl;
	cout<<"./allTag -l [size tag] -m [number of mismatches] -r [restriction sequence] -d [database file] -o [output file for perfect matches] -p [output file for matches with mismatches]"<<endl;
	cout<<endl<<endl;
	return 1;
    }

    fhPerfect.open (filePerfect);
    if (!fhPerfect.good()){
	cout << "Unable to open file perfect matches = "<< filePerfect<<endl;
	return 1;
    }

    fhMismatchA.open (fileMismatchA);
    if (!fhMismatchA.good()){
	cout << "Unable to open file with mismatches = "<< fileMismatchA<<endl;
	return 1;
    }

    fhMismatchC.open (fileMismatchC);
    if (!fhMismatchC.good()){
	cout << "Unable to open file with mismatches = "<< fileMismatchC<<endl;
	return 1;
    }
    
    fhMismatchG.open (fileMismatchG);
    if (!fhMismatchG.good()){
	cout << "Unable to open file with mismatches = "<< fileMismatchG<<endl;
	return 1;
    }
    
    fhMismatchT.open (fileMismatchT);
    if (!fhMismatchT.good()){
	cout << "Unable to open file with mismatches = "<< fileMismatchT<<endl;
	return 1;
    }





    if(SIZE_RESTRICTION_SITE < 0 ){
	cout<<"Please enter a positive integer for the the [size rest. site]"<<endl;
	exit(1);
    }

    if(SIZE_TAGSIZE < 0 ){
	cout<<"Please enter a positive integer for the the [size of the tag]"<<endl;
	exit(1);
    }

    if(NUMBER_OF_MISMATCHES < 0 ){
	cout<<"Please enter a positive integer for the the [number of mismatches]"<<endl;
	exit(1);
    }

    /******************************************/
    /*                                        */
    /*  END PROCESSING COMMAND LINE ARGUMENTS */
    /*                                        */
    /******************************************/





















    /******************************************/
    /*                                        */
    /*      END READING INPUT FILE            */
    /*                                        */
    /******************************************/






    /*   Checking if RC of restrictionSiteSequence is still restrictionSiteSequence  */
    
    char * testRC=computeReverseComplement(restrictionSiteSequence,0,SIZE_RESTRICTION_SITE);
    // cout<<"test"<<endl;
    // cout<<testRC<<endl;
    // exit(1);

    if( strcmp(restrictionSiteSequence,testRC) != 0){
	cerr<<"The reverse complement of the common prefix \""<<restrictionSiteSequence<<"\" is not the sequence itself"<<endl;
	exit(1);
    }


#ifdef WITH_COMMENTS
    cout<<endl<<"Done reading input file "<<endl;
#endif


    char sequenceToCheckForTag  [SIZE_TAGSIZE  - SIZE_RESTRICTION_SITE+1];
    char sequenceToCheckForTagRC[SIZE_TAGSIZE  - SIZE_RESTRICTION_SITE+1];
    sequenceToCheckForTag[0]='\0';
    sequenceToCheckForTagRC[0]='\0';









    /******************************************/
    /*                                        */
    /*  GOING THROUGH DATABASE SEQUENCES      */
    /*                                        */
    /******************************************/
    // cout<<INT_MAX<<endl;
    // exit(1);
    char * seqbuffer;
    try{
	seqbuffer =new char[INT_MAX];	
    }catch(bad_alloc& exc){
	cerr<<"allocating memory failed"<<endl;
	return 1;
    }


    string buffer;
    string previousLines="";
    string pPreviousLines="";
    string deflineInput;
    //    int endCoordinatesForNextString;
    // vector<struct matches<string> > * vectorOfDeflines;
    // vector<int>    indicesOfMatch;
    // vector<int>    basesFoundMatch;
    // vector<string> stringOfMatches;
    // vector<int>    mmFoundInRestSite;

    //    int indexInSeq=0;
    bool foundPreviousSeq=false;
    unsigned int positionInBuffer=0;
    fDatabase.open(databaseFileName);
    if ( ! fDatabase || !(fDatabase.is_open()) ){
	cerr<<endl<<"Error, database file "<<databaseFileName<<" cannot be opened"<<endl;
	exit(1);	
    }
	
    while( getline(fDatabase, line) ){
	int lengthOfLine=line.length();

	if( lengthOfLine == 0){
	    if(!ALLOW_EMPTY_LINES){
		cout<<"Error: empty line in database file "<<endl;
		exit(1);		
	    }else{
		continue;
	    }
	}

	if(line[0] != '#'){
	    //Detecting a defline
	    if(line[0] == '>'){
		
		deflineInput=string(line);		


		if(foundPreviousSeq){//analyze buffer
		    // cerr<<"done "<<endl;

		    processLines(seqbuffer, 
				 positionInBuffer,
				 SIZE_TAGSIZE,
				 SIZE_RESTRICTION_SITE,  
				 restrictionSiteSequence,
				 deflineInput,
				 fhPerfect,
				 fhMismatchA,
				 fhMismatchC,
				 fhMismatchG,
				 fhMismatchT);

			
		}else{//if not previously found a seq, meaning first seq
		    
		}

		// cerr<<"loading "<<deflineInput<<endl;

		positionInBuffer=0;	
		// counterOfChar=0;
		// indexInSeq=0;
		
	    }else{

		//checking line for invalid characters
		for(int i=0;i<lengthOfLine;i++){
		    line[i]=toupper(line[i]);
		    if(int(line[i]) < 65 || 
		       int(line[i]) > 90 ){
			cout<<"Invalid character in line :"<<line<<endl;
			exit(1);
		    }
		    seqbuffer[positionInBuffer++] =   line[i];
		}

		if(positionInBuffer > INT_MAX){
		    cerr<<"buffer overflow, sequence "<<deflineInput<<" is too long "<<endl;
		    exit(1);
		}
		foundPreviousSeq=true;

		
	    } //end if the line in the input file does not begin with >
	} //end if the line in the input file does not begin with #
    } //end for each line in the input file
    //last record
    processLines(seqbuffer, 
		 positionInBuffer,
		 SIZE_TAGSIZE,
		 SIZE_RESTRICTION_SITE,  
		 restrictionSiteSequence,
		 deflineInput,
		 fhPerfect,
		 fhMismatchA,
		 fhMismatchC,
		 fhMismatchG,
		 fhMismatchT);

    fDatabase.close();
 
#ifdef WITH_COMMENTS
    cout<<endl<<"Succes ! Found annotated "<<numberOfLinesInTheInputFile<<" records"<<endl;
#endif

    fhPerfect.close();
    fhMismatchA.close();
    fhMismatchC.close();
    fhMismatchG.close();
    fhMismatchT.close();


    return 0;
}



