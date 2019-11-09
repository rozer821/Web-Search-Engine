//
// Created by 毕为豪 on 2019/10/6.
//

#ifndef STRUCT_H
#define STRUCT_H

#include <pthread.h>
#include <set>
#include <map>
#include <vector>
#include <string>
using namespace std;


typedef struct {
    int docID;
    string word;
    int pos ;
    int rawpage;
} Posting;

typedef std::vector<Posting *> PostingVector;


typedef struct {
    uint32_t docID;
    uint32_t actualDocID;
    uint32_t freq;
    uint32_t pos = 0;
    uint32_t rawpage;
} DocPosting;
//a struct store the information of document


//This struct update only after merge phase end

typedef struct {
    string word;
    int num;   // number of docs containing the word
    int fileID;    // the ID of the file containing the invertd index
    int invertedPointer;   //pointer to the inverted index
    int size;  //size of this list in bytes
} LexiconItem;

typedef vector<LexiconItem *> LexiconTable;
void writeLexiconTable(LexiconTable *lexiconTable);


//The docInfo structure
typedef struct {
    int docID;
    string docURL;
    int docSize;
}DocInfoTableItem;
typedef vector<DocInfoTableItem *>DocInfoTable;




//typedef map<int,doc> doc_id_dic;  //implement as database
//typedef map<int,posting> postingList; //int is doc id
//typedef map<string,posting> invertedIndex;




#endif //STRUCT_H
