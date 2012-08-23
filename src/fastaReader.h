#ifndef __FASTAREADER_H__
#define __FASTAREADER_H__


#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib> 


using namespace std;

typedef struct{
    string * defline;
    string * sequence;
} record;


class fastaReader {
 private:
     char const* delims;
    ifstream fastafile;
    bool firstRecord;
    string buffer;
    bool hasNext;
    void checkString(string fromFile);
 public:    
    fastaReader(string fileName);
    record * getNextRecord();
    bool hasNextRecord();
};

#endif
