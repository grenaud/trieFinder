#include <algorithm>
#include <vector>
#include <iostream>
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"



#include "trie.h"
#include "fastaReader.h"



#define DATABASE_defline_MAXSIZE             1000000000
#define DATABASE_defline_length_MAXSIZE      10000000
#define DATABASE_sequence_MAXSIZE            10000000000
#define DATABASE_sequence_length_MAXSIZE     10000000

#define MIN(a,b)  ((a) < (b) ? (a) : (b))
#define MAX(a,b)  ((a) > (b) ? (a) : (b))

//#define WITH_COMMENTS

// #define	DEBUG_EACH_WORD
// #define	DEBUG_TRIE
// #define	DEBUG_KEEP_ON_DISK

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
char * computeReverseComplement(const char * stringToReverse,int lengthOfString){
    int i;
    //char * stringToReturn=(char *)malloc(sizeof(char)*(lengthOfString+1));
    char * stringToReturn=new char [lengthOfString+1];
    for(i=(lengthOfString-1);i>=0;i--){
	stringToReturn[ (lengthOfString-1)-i  ] = dnaComplement(stringToReverse[i]);
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
inline void printNChar(char * stringToPrint,int lengthOfString){
    int i;
    char * pointStr=stringToPrint;

    for(i=0;i<lengthOfString;i++){
	cout<<*(pointStr++);
    }
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
    unsigned long int counterOfChar=0;

    char inputFileTags [1000];

    int    returnCodeByMatchingFunction;
              
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

    ofstream fhPerfect;
    ofstream fhMismatchA;
    ofstream fhMismatchC;
    ofstream fhMismatchG;
    ofstream fhMismatchT;

    fhPerfect.open (filePerfect);
    if (!fhPerfect.is_open()){
	cout << "Unable to open file perfect matches = "<< filePerfect<<endl;
	return 1;
    }

    fhMismatchA.open (fileMismatchA);
    if (!fhMismatchA.is_open()){
	cout << "Unable to open file with mismatches = "<< fileMismatchA<<endl;
	return 1;
    }

    fhMismatchC.open (fileMismatchC);
    if (!fhMismatchC.is_open()){
	cout << "Unable to open file with mismatches = "<< fileMismatchC<<endl;
	return 1;
    }
    
    fhMismatchG.open (fileMismatchG);
    if (!fhMismatchG.is_open()){
	cout << "Unable to open file with mismatches = "<< fileMismatchG<<endl;
	return 1;
    }
    
    fhMismatchT.open (fileMismatchT);
    if (!fhMismatchT.is_open()){
	cout << "Unable to open file with mismatches = "<< fileMismatchT<<endl;
	return 1;
    }




    if( argc < 2 ||  strlen(databaseFileName) < 2 || strcmp(argv[1],"-h") == 0){
	cout<<"Usage:"<<endl;
	cout<<"./allTag -l [size tag] -m [number of mismatches] -r [restriction sequence] -d [database file] -o [output file for perfect matches] -p [output file for matches with mismatches]"<<endl;
	cout<<endl<<endl;
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
    /*      READING INPUT FILE                */
    /*                                        */
    /******************************************/

//     bool firstLine=true;

// #ifdef WITH_COMMENTS
//     cout<<endl<<"Input file : "<<inputFileTags<<endl;
// #endif

//     fastaReader myFastaReader (inputFileTags);
//     char * sequenceFromInput=new char[1000];
//     char * sequenceFromInputRC;
//     char deflineFromInput[1000];
//     //char restrictionSiteSequence[SIZE_RESTRICTION_SITE +1 ];

//     while(myFastaReader.hasNextRecord()){
	
// 	record * currentRecord=myFastaReader.getNextRecord();
// 	transform(currentRecord->sequence->begin(), 
//  		  currentRecord->sequence->end(), 
//  		  currentRecord->sequence->begin(), 
//  		  (int(*)(int)) toupper);

// 	sequenceFromInput[0]='\0';
// 	strncpy(sequenceFromInput,
// 		(*currentRecord->sequence).c_str()+SIZE_RESTRICTION_SITE, 
// 		(*currentRecord->sequence).length()- SIZE_RESTRICTION_SITE);
// 	sequenceFromInput[(*currentRecord->sequence).length()- SIZE_RESTRICTION_SITE]='\0';
// 	sequenceFromInputRC=computeReverseComplement(sequenceFromInput, 
// 						     (*currentRecord->sequence).length() - SIZE_RESTRICTION_SITE);	
// 	//sequenceFromInputRC[(*currentRecord->sequence).length()- SIZE_RESTRICTION_SITE]='\0';

// 	if(firstLine){		    
// 	    SIZE_TAGSIZE=(*currentRecord->sequence).length();
// 	    trieOftagSequences=new Trie<string>(  sequenceFromInput , (*currentRecord->defline));
// 	    trieOftagSequencesRC=new Trie<string>(sequenceFromInputRC , (*currentRecord->defline));
// 	    strncpy(restrictionSiteSequence,(*currentRecord->sequence).c_str(),SIZE_RESTRICTION_SITE);
// 	    restrictionSiteSequence[SIZE_RESTRICTION_SITE]='\0';
// 	    firstLine=false;
// 	}else{
// 	    //check length
// 	    if((*currentRecord->sequence).length() != SIZE_TAGSIZE){
// 		cerr<<"The length of the sequence "<<(*currentRecord->defline)<<" does not match the length of the others"<<endl;
// 		exit(1);
// 	    }
// 	    //check prefix
// 	    if(strncmp(restrictionSiteSequence,(*currentRecord->sequence).c_str(),SIZE_RESTRICTION_SITE) != 0){
// 		cerr<<"The prefix of the sequence "<<(*currentRecord->defline)<<" does not match the prefixes previously found"<<endl;
// 		exit(1);
// 	    }
		       

// 	    trieOftagSequences->insert(sequenceFromInput   , (*currentRecord->defline));
// 	    trieOftagSequencesRC->insert(sequenceFromInputRC , (*currentRecord->defline));
// 	}

// 	delete(currentRecord->defline);
// 	delete(currentRecord->sequence);
// 	delete(currentRecord);

//     }
    
//     numberOfLinesInTheInputFile=indexInTheInputFile;

    /******************************************/
    /*                                        */
    /*      END READING INPUT FILE            */
    /*                                        */
    /******************************************/






    /*   Checking if RC of restrictionSiteSequence is still restrictionSiteSequence  */
    
    char * testRC=computeReverseComplement(restrictionSiteSequence,SIZE_RESTRICTION_SITE);
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

    string buffer;
    string previousLines="";
    string pPreviousLines="";
    string deflineInput;
    //    int endCoordinatesForNextString;
    vector<struct matches<string> > * vectorOfDeflines;
    vector<int>    indicesOfMatch;
    vector<int>    basesFoundMatch;
    vector<string> stringOfMatches;
    vector<int>    mmFoundInRestSite;

    int indexInSeq=0;
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
		previousLines="";
		pPreviousLines="";

		counterOfChar=0;
		indexInSeq=0;
		indicesOfMatch.clear();
		basesFoundMatch.clear();
		stringOfMatches.clear();
		mmFoundInRestSite.clear();

	    }else{

		for(int i=0;i<lengthOfLine;i++){
		    line[i]=toupper(line[i]);
		    if(int(line[i]) < 65 || 
		       int(line[i]) > 90 ){
			cout<<"Invalid character in line :"<<line<<endl;
			exit(1);
		    }
		}


		buffer=string(pPreviousLines+previousLines+line);
		// if(previousLines.length() == 0 )
		//     endCoordinatesForNextString=-1;
		// else
		//     endCoordinatesForNextString=buffer.length()-SIZE_TAGSIZE;
		int firstIndex = MAX(0,previousLines.length() +pPreviousLines.length() );
		if(firstIndex != 0){
		    firstIndex=firstIndex-SIZE_RESTRICTION_SITE+1;
		}
		
#ifdef DEBUG_KEEP_ON_DISK
		cout<<"line           "<<line<<endl;
		cout<<"buffer         "<<buffer<<endl;
		cout<<"previousLines  "<<previousLines<<endl;
		cout<<"pPreviousLines "<<pPreviousLines<<endl;
		cout<<"firstIndex     "<<firstIndex<<endl<<endl;
#endif

		// cout<<"begin correction"<<endl;
		vector<int>::iterator    itix = indicesOfMatch.begin();
		vector<int>::iterator    itbs = basesFoundMatch.begin();
		vector<string>::iterator itsm = stringOfMatches.begin();
		vector<int>::iterator itmm = mmFoundInRestSite.begin();


		while(itbs<basesFoundMatch.end()  || 
		      itsm<stringOfMatches.end()  || 
		      itix<indicesOfMatch.end()   || 
		      itmm<mmFoundInRestSite.end() ){

		    // cout<<"itbs "<<*itbs<<endl;
		    // cout<<"itsm "<<*itsm<<endl;
		    // cout<<"itix "<<*itix<<endl;
		    // cout<<"itmm "<<*itmm<<endl;

		    // cout<<indicesOfMatch.size()<<endl;
		    // cout<<basesFoundMatch.size()<<endl;
		    // cout<<stringOfMatches.size()<<endl;
		    // cout<<mmFoundInRestSite.size()<<endl;

		    if( (SIZE_TAGSIZE-*itbs) > line.length()){
			*itbs=*itbs+line.length();
			*itsm=string(*itsm+line);
			//cout<<"keeping"<<endl;
			itix++;
			itbs++;
			itsm++;
			itmm++;

		    }else{
			*itsm=string(*itsm + line.substr(0,SIZE_TAGSIZE-*itbs) );
			//cout<<"printing"<<endl;
			if( (*itmm) == 0 ){
			    fhPerfect<<deflineInput<<"\t";
			    fhPerfect<<*itix<<"\t";
			    fhPerfect<<*itsm<<"\t+"<<endl;
			}else{
			    //switch(char( (*itsm).substr(0,1) ) ){
			    switch(char( (*itsm)[0] ) ){
			    case 'A':
				fhMismatchA<<deflineInput<<"\t";
				fhMismatchA<<*itix<<"\t";
				fhMismatchA<<*itsm<<"\t+"<<endl;
				break;
			    case 'C':
				fhMismatchC<<deflineInput<<"\t";
				fhMismatchC<<*itix<<"\t";
				fhMismatchC<<*itsm<<"\t+"<<endl;
				break;
			    case 'G':
				fhMismatchG<<deflineInput<<"\t";
				fhMismatchG<<*itix<<"\t";
				fhMismatchG<<*itsm<<"\t+"<<endl;
				break;
			    case 'T':
				fhMismatchT<<deflineInput<<"\t";
				fhMismatchT<<*itix<<"\t";
				fhMismatchT<<*itsm<<"\t+"<<endl;
				break;
			    default:
				cerr<<"ERROR "<<*itsm<<endl;
				return 1;
			    }
			}
			itix=indicesOfMatch.erase(itix);
			itbs=basesFoundMatch.erase(itbs);
			itsm=stringOfMatches.erase(itsm);
			itmm=mmFoundInRestSite.erase(itmm);
		    }
		 
		}
		// cout<<"end correction"<<endl;		    
		

		for(int j=firstIndex;j<=(buffer.length()-SIZE_RESTRICTION_SITE);j++){		
		    
		    // fflush(stdout);
		    returnCodeByMatchingFunction=matchString(restrictionSiteSequence, 
		    					     SIZE_RESTRICTION_SITE,
		    					     buffer.c_str(),
		    					     j,
		    					     NUMBER_OF_MISMATCHES);
		    
		    if(returnCodeByMatchingFunction != -1 ){
		    	//cout <<"j "<<j<<" "<<returnCodeByMatchingFunction<<endl;
			if(j>=SIZE_TAGSIZE-SIZE_RESTRICTION_SITE){
			    if(returnCodeByMatchingFunction == 0){
				fhPerfect<<deflineInput<<"\t";
				fhPerfect<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				fhPerfect<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				fhPerfect<<"-"<<endl;
			    }else{
				switch( computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)[0] ){
				case 'A':
				    fhMismatchA<<deflineInput<<"\t";
				    fhMismatchA<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				    fhMismatchA<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				    fhMismatchA<<"-"<<endl;
				    break;
				case 'C':
				    fhMismatchC<<deflineInput<<"\t";
				    fhMismatchC<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				    fhMismatchC<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				    fhMismatchC<<"-"<<endl;

				    break;
				case 'G':
				    fhMismatchG<<deflineInput<<"\t";
				    fhMismatchG<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				    fhMismatchG<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				    fhMismatchG<<"-"<<endl;

				    break;
				case 'T':
				    fhMismatchT<<deflineInput<<"\t";
				    fhMismatchT<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				    fhMismatchT<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				    fhMismatchT<<"-"<<endl;

				    break;
				default:
				    cerr<<"ERROR "<<*itsm<<endl;
				    return 1;
				}

				// fhMismatch<<deflineInput<<"\t";
				// fhMismatch<<indexInSeq +(j-previousLines.length()-pPreviousLines.length()) - (SIZE_TAGSIZE-SIZE_RESTRICTION_SITE)<<"\t";
				// fhMismatch<<computeReverseComplement((buffer.substr(j-SIZE_TAGSIZE+SIZE_RESTRICTION_SITE,SIZE_TAGSIZE)).c_str(),SIZE_TAGSIZE)<<"\t";
				// fhMismatch<<"-"<<endl;
			    }
			}

			// cout<<indicesOfMatch.size()<<endl;
			// cout<<basesFoundMatch.size()<<endl;
			// cout<<stringOfMatches.size()<<endl;
			// cout<<mmFoundInRestSite.size()<<endl;
			// cout<<"add itbs "<<(indexInSeq+ (j-previousLines.length()-pPreviousLines.length()))<<endl;
			// cout<<"add itsm "<<(buffer.length()-j)<<endl;
			// cout<<"add itix "<<(buffer.substr(j,buffer.length()-j))<<endl;
			// cout<<"add itmm "<<(returnCodeByMatchingFunction)<<endl;
			
			indicesOfMatch.push_back(indexInSeq+ (j-previousLines.length()-pPreviousLines.length()));
			basesFoundMatch.push_back(buffer.length()-j);
			stringOfMatches.push_back(buffer.substr(j,buffer.length()-j));
			mmFoundInRestSite.push_back(returnCodeByMatchingFunction);

			// cout<<indicesOfMatch.size()<<endl;
			// cout<<basesFoundMatch.size()<<endl;
			// cout<<stringOfMatches.size()<<endl;
			// cout<<mmFoundInRestSite.size()<<endl;			
		    }
		    		    
		}

		    
		counterOfChar+=(endCoordinatesForNextString+1);

		pPreviousLines=previousLines;
		previousLines=line;
		indexInSeq+=line.length();
	    } //end if the line in the input file does not begin with >
	} //end if the line in the input file does not begin with #
    } //end for each line in the input file

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



