#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <math.h>
#include <stdio.h>
#include <gzstream.h>

#include "trie.h"
#include "fastaReader.h"

using namespace std;

int lengthTagDatabase=76;

typedef struct{
    unsigned int idseq;
    unsigned int coord;
    char strand;
} singlematch;


typedef struct{
    vector<singlematch>  * refseq;
    vector<singlematch>  * unigene;
    vector<singlematch>  * genome;
} allmatches;



typedef struct{
    string defline;
    int coord;
    char strand;
} matchtodb;

matchtodb parseMatch(string toparse){
    matchtodb toreturn;
    //cout<<"toparse "<<toparse<<endl;
    toreturn.strand=toparse[toparse.length()-1];
    toparse.erase(toparse.length()-2,2);
    
    size_t found;
    found=toparse.find_last_of("#");
    toreturn.coord= atoi( toparse.substr(found+1,(toparse.length()-found) ).c_str() );
    toreturn.defline=toparse.substr(0,found);


    // cout<<toreturn.defline <<endl;
    // cout<<toreturn.coord  <<endl;
    // cout<<toreturn.strand   <<endl;   

    return toreturn;
}

inline string convertInt(int number){
    stringstream ss;
    ss << number;
    return ss.str();
}



string returnFirstToken(string * toparse,string delim){
    size_t found;
    string toreturn;

    // cout<<"*toparse "<<*toparse<<endl;

    found=toparse->find(delim);
    if (found!=string::npos){
	// cout << "first 'needle' found at: " << int(found) << endl;
	toreturn=toparse->substr(0,found);
	toparse->erase(0,found+delim.length());
	// cout<<toreturn<<endl;
	// cout<<*toparse<<endl;

	return toreturn;
    }
    toreturn=(*toparse);
    toparse->erase(0,toparse->length());    
    return toreturn;
}

int progress_func(int current,
		  int total ){
    int stars=50;
    double frac = double(current) / double(total);
    int dot = round(frac * stars);


    int ii=0;
    fprintf(stderr, "%3.0f%% [",frac*100);

    for ( ; ii < dot;ii++) {
	fprintf(stderr,"*");
    }

    for ( ; ii < stars;ii++) {
	fprintf(stderr," ");
    }

    fprintf(stderr,"]\r");
    fflush(stdout);
}

inline string joinlist(list<string> tocat){
    if(tocat.size() == 0)
	return "";
    if(tocat.size() == 1)
	return tocat.front();
    string toreturn="";
    int printed=0;
    while(tocat.size() != 0){
	if(tocat.size() == 1){
	    toreturn+= tocat.front();
	    tocat.pop_front();
	}else{
	    toreturn+= tocat.front()+",,,";
	    tocat.pop_front();
	}
	// if(printed >=10)
	//     return toreturn;    
	printed++;
    }
    return toreturn;    
}


inline unsigned int computeCorrectCoord(singlematch & sm,unsigned int tagLengthFound){
    if(sm.strand == '+'){
	return sm.coord; //no modificati
    }else{
	if(sm.strand == '-'){
	    return (sm.coord +  (lengthTagDatabase-tagLengthFound));
	}else{
	    cerr<<endl<<"Wrong strand for record "<<sm.idseq <<" contact developers"<<endl;
	    exit(1);
	}
    }
}


int main(int argc, char *argv[]){
    Trie<allmatches> * trieOftagSeqs;
    map<unsigned int,string> id2refseq;
    map<unsigned int,string> id2unigene;
    map<unsigned int,string> id2genome;
    map<string,unsigned int> refseq2id;
    map<string,unsigned int> unigene2id;
    map<string,unsigned int> genome2id;

    unsigned int counterrefseq=0;
    unsigned int counterunigene=0;
    unsigned int countergenome=0;

    if(argc != 4 || 
       strcmp(argv[1],"-h")==0 || 
       strcmp(argv[1],"-h")==0){
	cout<<"Usage:"<<endl<<"searchtags [database file] [fastq input file] [number of mismatches (1..3)]"<<endl;
	return 1;
    }

    igzstream fDatabase;
    string databaseFileName=string(argv[1]);
    igzstream fInput;
    string inputFileName=string(argv[2]);
    int numberOfMismatches=atoi(argv[3]);
    if(numberOfMismatches < 0  ||
       numberOfMismatches > 3){
	cerr<<endl<<"Error, number of mismatches must be between 0 and 3"<<endl;
	exit(1);	
    }
       

    string line;
    string segment;
    string tag;


    fDatabase.open(databaseFileName.c_str());
    if ( !(fDatabase.good()) ){
	cerr<<endl<<"Error, database file "<<databaseFileName<<" cannot be opened"<<endl;
	exit(1);	
    }


    //count lines
    int totalLinesdb=0;
    while( getline(fDatabase, line) ){
	totalLinesdb++;
    }
    fDatabase.close();

    igzstream fDatabase2;

    fDatabase2.open(databaseFileName.c_str());
    if ( !(fDatabase2.good()) ){
	cerr<<endl<<"Error, database file "<<databaseFileName<<" cannot be opened for the second time"<<endl;
	exit(1);	
    }


    bool firstLine=true;	
    string token;
    int lineCounterdb=0;
    while( getline(fDatabase2, line) ){
	if(lineCounterdb%1000000==0){
	    progress_func(lineCounterdb,totalLinesdb);
	}
	istringstream ss(line) ;
	allmatches * myCurrentMatch=new allmatches;
	myCurrentMatch->refseq =new vector<singlematch>();
	myCurrentMatch->unigene=new vector<singlematch>();
	myCurrentMatch->genome =new vector<singlematch>();

	getline(ss,segment,'\t');
	tag=string(segment);
	//cout<<tag<<endl;
	getline(ss,segment,'\t');

	//refseq
	token=returnFirstToken(&segment,"###");
	while(token.length()!=0){

	    matchtodb toadd=parseMatch(token);
	    if( !(refseq2id.find(toadd.defline) != refseq2id.end())){ //not found, to add
		// cout<<"adding "<<counterrefseq<<endl;
		refseq2id.insert(pair<string, unsigned int>(toadd.defline,counterrefseq)) ;
		id2refseq.insert(pair<unsigned int,string> (counterrefseq,toadd.defline)) ;
		singlematch newrec;
		newrec.idseq=counterrefseq;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->refseq->push_back(newrec);
		counterrefseq++;
	    }else{
		unsigned int countertouse=refseq2id[toadd.defline];
		singlematch newrec;
		newrec.idseq=countertouse;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->refseq->push_back(newrec);
	    }

	    token=returnFirstToken(&segment,"###");	    
	}

	//myCurrentMatch->refseq=string(segment);
	getline(ss,segment,'\t');

	token=returnFirstToken(&segment,"###");
	while(token.length()!=0){

	    matchtodb toadd=parseMatch(token);
	    if( !(unigene2id.find(toadd.defline) != unigene2id.end())){ //not found, to add		
		unigene2id.insert(pair<string, unsigned int>(toadd.defline,counterunigene)) ;
		id2unigene.insert(pair<unsigned int,string> (counterunigene,toadd.defline)) ;
		singlematch newrec;
		newrec.idseq=counterunigene;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->unigene->push_back(newrec);
		counterunigene++;
	    }else{
		unsigned int countertouse=unigene2id[toadd.defline];
		singlematch newrec;
		newrec.idseq=countertouse;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->unigene->push_back(newrec);
	    }

	    token=returnFirstToken(&segment,"###");	    
	}


	//myCurrentMatch->unigene=string(segment);
	getline(ss,segment,'\t');
        //myCurrentMatch->genome=string(segment);


	//myCurrentMatch->refseq=string(segment);
	getline(ss,segment,'\t');

	token=returnFirstToken(&segment,"###");
	while(token.length()!=0){

	    matchtodb toadd=parseMatch(token);
	    if( !(genome2id.find(toadd.defline) != genome2id.end())){ //not found, to add		
		genome2id.insert(pair<string, unsigned int>(toadd.defline,countergenome)) ;
		id2genome.insert(pair<unsigned int,string> (countergenome,toadd.defline)) ;
		singlematch newrec;
		newrec.idseq=countergenome;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->genome->push_back(newrec);
		countergenome++;
	    }else{
		unsigned int countertouse=genome2id[toadd.defline];
		singlematch newrec;
		newrec.idseq=countertouse;
		newrec.coord=toadd.coord;
		newrec.strand=toadd.strand;
		myCurrentMatch->genome->push_back(newrec);
	    }

	    token=returnFirstToken(&segment,"###");	    
	}

	if(firstLine){
	    trieOftagSeqs=new Trie<allmatches>( tag.c_str() ,*myCurrentMatch );
	    lengthTagDatabase=tag.size();
	    firstLine=false;
	}else{
	    if(lengthTagDatabase != tag.size() ){
		cerr<<"Length of the tag "<<tag.size()<<" for sequence "<<tag<<" is different from the one previously found="<<lengthTagDatabase<<endl;
		return 1;
	    }
	    trieOftagSeqs->insert(tag.c_str() ,*myCurrentMatch );	    
	}
	lineCounterdb++;
    }

    fDatabase2.close();


    // char * sequenceFromInput=new char[1000];

    // strcpy(sequenceFromInput,  "CGTAGTCCTGAGTCGTC");

    // trieOftagSequences=new Trie<string>( sequenceFromInput , ">test1");

    // trieOftagSequences->insert("CGTAGTCCTGAGTCGTC" , ">test2");

    // trieOftagSequences->insert("CGTAGTCCTGAGTCGTA" , ">test3");

    // trieOftagSequences->insert("CGTAGTCCTG" , ">test4");
    cerr<<"searching "<<endl;
    fInput.open(inputFileName.c_str());
    if ( !(fInput.good()) ){
	cerr<<endl<<"Error, input file "<<inputFileName<<" cannot be opened"<<endl;
	exit(1);	
    }
    unsigned int lineCounter=0;
    string defline;
    
    while( getline(fInput, line) ){
	// cout<<"line"<<endl;
	if(lineCounter %4 == 0){
	    defline=line;
	    if(defline[0] != '@'){
		cerr<<endl<<"Expected a '@' sign for defline  "<<line<<" please make sure your input is fastq, without blank lines"<<endl;
		exit(1);	
	    }
	}

	if(lineCounter %4 == 1){
	    // string mysequencetosearch="GATCCAGACGGTCGTACATTTTCAAAAGATGTCTAAAACTGCCACAAACCTCGGGAGAA";
	    string mysequencetosearch=line;
	    // cout<<"mysequencetosearch "<<mysequencetosearch<<endl;
	    if(mysequencetosearch.size() > lengthTagDatabase){
		cerr<<"Sequence "<<defline<<" with sequence "<<mysequencetosearch<<" is greater than length of the database"<<endl;
		return 1;
	    }
	    //vector< matches<allmatches> >  * vectorOfDeflines=trieOftagSeqs->searchForWordMismatch((mysequencetosearch.substr(4,mysequencetosearch.length()-4)).c_str(),numberOfMismatches);
	    vector< matches<allmatches> >  * vectorOfDeflines=trieOftagSeqs->searchForWordMismatch(mysequencetosearch.c_str(),numberOfMismatches);
	    // "GATCAGATTAGCAT",0);
	    //GATCAGAACAAGCATG",0);			
	    list<string> refseqP;
	    list<string> refseqM;
	    list<string> unigeneP;
	    list<string> unigeneM;
	    list<string> genomeP;
	    list<string> genomeM;

	    // cout<<"matches "<<vectorOfDeflines->size()<<endl;
	    for(int i=0;i<vectorOfDeflines->size();i++){
		// cout<<endl<<"MATCH"<<endl;
		//list< allmatches  > * iter=(*vectorOfDeflines)[i].listOfDeflinesOfMatches;
		list< allmatches  >::const_iterator iter;		    
		for (iter  = (*vectorOfDeflines)[i].listOfDeflinesOfMatches->begin(); 
		     iter != (*vectorOfDeflines)[i].listOfDeflinesOfMatches->end(); 
		     iter++){
		    for(int j=0;j<iter->refseq->size();j++){
			if( (*vectorOfDeflines)[i].numberOfMismatches == 0){
			    refseqP.push_back( ( (id2refseq[(*(iter->refseq))[j].idseq ]+":")+
						 (convertInt(computeCorrectCoord((*(iter->refseq))[j],
										 mysequencetosearch.size()))+":")) +
						 (*(iter->refseq))[j].strand);
			}else{
			    refseqM.push_back(((id2refseq[ (*(iter->refseq))[j].idseq ]+":")+
					       //(convertInt((*(iter->refseq))[j].coord)+":"))+
					       (convertInt(computeCorrectCoord((*(iter->refseq))[j],
									       mysequencetosearch.size()))+":")) +
					      (*(iter->refseq))[j].strand);
			}
			// cout<<id2refseq[ (*(iter->refseq))[j].idseq ]<<"\t";
			// cout<<(*(iter->refseq))[j].coord<<"\t";
			// cout<<(*(iter->refseq))[j].strand<<endl;

		    }
		    for(int j=0;j<iter->unigene->size();j++){
			if( (*vectorOfDeflines)[i].numberOfMismatches == 0){
			    unigeneP.push_back(((id2unigene[ (*(iter->unigene))[j].idseq ]+":")+
						//(convertInt((*(iter->unigene))[j].coord)+":"))+
						(convertInt(computeCorrectCoord((*(iter->unigene))[j],
										mysequencetosearch.size()))+":")) +
					       (*(iter->unigene))[j].strand);
			}else{
			    unigeneM.push_back(((id2unigene[ (*(iter->unigene))[j].idseq ]+":")+
						//(convertInt((*(iter->unigene))[j].coord)+":"))+
						(convertInt(computeCorrectCoord((*(iter->unigene))[j],
										mysequencetosearch.size()))+":")) +

					       (*(iter->unigene))[j].strand);
			}
			// cout<<id2unigene[ (*(iter->unigene))[j].idseq ]<<"\t";
			// cout<<(*(iter->unigene))[j].coord<<"\t";
			// cout<<(*(iter->unigene))[j].strand<<endl;
		    }
		    for(int j=0;j<iter->genome->size();j++){
			if( (*vectorOfDeflines)[i].numberOfMismatches == 0){
			    genomeP.push_back(((id2genome[ (*(iter->genome))[j].idseq ]+":")+
					       //(convertInt((*(iter->genome))[j].coord)+":"))+
					       (convertInt(computeCorrectCoord((*(iter->genome))[j],
									       mysequencetosearch.size()))+":")) +
					      (*(iter->genome))[j].strand);
			}else{
			    genomeM.push_back(((id2genome[ (*(iter->genome))[j].idseq ]+":")+
					       //(convertInt((*(iter->genome))[j].coord)+":"))+
					       (convertInt(computeCorrectCoord((*(iter->genome))[j],
									       mysequencetosearch.size()))+":")) +
					      (*(iter->genome))[j].strand);
			}
		    }


		}
	    }

	    cout<<defline<<"\t"<<
		mysequencetosearch<<"\t"<<
		joinlist(refseqP)<<"\t"<<
		joinlist(refseqM)<<"\t"<<
		joinlist(unigeneP)<<"\t"<<
		joinlist(unigeneM)<<"\t"<<
		joinlist(genomeP)<<"\t"<<
		joinlist(genomeM)<<"\n";
	   
	}
	lineCounter++;
    }
    cerr<<"done "<<endl;
    return 0;
}




//     unsigned int idseq;
//     int coord;
//     char strand;
// } singlematch;

