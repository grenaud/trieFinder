/*
 * PUBLIC DOMAIN NOTICE
 *
 * This software/database is United States Government Work under the
 * terms of the United States Copyright Act. It was written as part of
 * the authors official duties for the United States Government and thus
 * cannot be copyrighted. This software/database is freely available to
 * the public for use without a copyright notice. Restrictions cannot
 * be placed on its present or future use.
 *
 * Although all reasonable efforts have been taken to ensure the accuracy
 * and reliability of the software and data, the National Human Genome
 * Research Institute (NHGRI) and the U.S. Government does not and cannot
 * warrant the performance or results that may be obtained by using this
 * software or data.  NHGRI and the U.S. Government disclaims all
 * warranties as to performance, merchantability or fitness for any
 * particular purpose.
 *
 * In any work or product derived from this material, proper
 * attribution of the author as the source of the software or
 * data should be made.
 *
 */

#include "fastaReader.h"



fastaReader::fastaReader(string fileName){
    firstRecord=true;
    hasNext=true;
    fastafile.open(fileName.c_str());
    delims = " \t\r\n";
    if (!fastafile || !fastafile.is_open() ) {
        cerr << "Cant open file "<<fileName<<endl;
        exit(1); 
    }
}






void
fastaReader::checkString(string fromFile){
    string::size_type first=fromFile.find_first_not_of(delims);
    string::size_type last=fromFile.find_last_not_of(delims);

    if(first != last){

	if(first != 0){
	    cerr<<"Error, line  "<<fromFile<<"should not begin with whitespaces"<<endl;
	    exit(1);
	}

	if(last != (fromFile.length()-1) ){
	    cerr<<"Error, line  "<<fromFile<<"should not end with whitespaces"<<endl;
	    exit(1);
	} 
    }

    if(fromFile.find_first_of(delims) == 0){
	cerr<<"Error, line  "<<fromFile<<" should not begin with whitespaces"<<endl;
	exit(1);
    }

    if(fromFile.find_last_of(delims) != string::npos &&
       fromFile.find_last_of(delims) == (fromFile.length()-1) ){
	cerr<<"Error, line  "<<fromFile<<" should not end with whitespaces"<<endl;
	exit(1);
    }


}




bool
fastaReader::hasNextRecord(){
    return hasNext;
}


record *
fastaReader::getNextRecord(){
    record * toReturn=new record;
    bool firstLineOfSequence=true;    

    //Capture the defline from the last getNextRecord()
    if(!firstRecord){
	toReturn->defline=new string(buffer);	
	if( toReturn->defline->length() == 1 ){	    
	    cerr<<"Empty defline, exiting"<<endl;
	    exit(1); 
	}
    }


    while( getline(fastafile, buffer) ){
	
	checkString(buffer);
	//defline
	if(buffer[0] == '>'){
	    if(firstRecord){ //if this is the first record, we record this as our defline
		toReturn->defline=new string(buffer);

		if(toReturn->defline->length() == 1){
		    cerr<<"Empty defline, exiting"<<endl;
		    exit(1); 
		}
		firstRecord=false;
	    }else{

		if( (!toReturn->defline)  ||
		    (!toReturn->sequence)   ||
		    toReturn->sequence->length() == 0){
		    cerr<<"Error, fastaReader: Invalid record ";
		    if( toReturn->defline ){
			cerr<<" with record"<<(*toReturn->defline)<<" ";
		    }
		    cerr<<" exiting"<<endl;
		    exit(1); 		    
		}
		return toReturn;
	    }
	    
	}else{
	    //if there is an empty line after the first defline, return error
	    if(firstRecord){
		if(buffer.length() != 0){
		    cerr<<"Error, fastaReader: first record must start with > sign exiting"<<endl;
		    exit(1); 
		}
	    }else{
		if(firstLineOfSequence){ //initialize the sequence if its the first line of the sequence
		    toReturn->sequence=new string(buffer);
		    if(toReturn->sequence->length() == 0){
			cerr<<"Error, fastaReader: empty sequence ";
			if( toReturn->defline ){
			    cerr<<" with record"<<(*toReturn->defline)<<" ";
			}
			cerr<<" exiting"<<endl;
			exit(1); 
		    }

		    firstLineOfSequence=false;
		}else{ // if not the first line, just append to the current string
		    (*toReturn->sequence)+=buffer;
		}
	    }
	}
    }
    
    fastafile.close();
    hasNext=false;

    if( (!toReturn->defline)    ||
	(!toReturn->sequence)   ||
	toReturn->sequence->length() == 0){
	cerr<<"Error, fastaReader: Invalid record";
	if( toReturn->defline ){
	    cerr<<" with record"<<(*toReturn->defline)<<" ";
	}
	cerr<<"...exiting"<<endl;
	exit(1); 		    
    }

    return toReturn;
}
