//
// Created by 毕为豪 on 2019/10/15.
//
#include "struct.h"
#include <vector>
#include <string>
#include <fstream>


#ifndef INVERTED_INDEX_PIPELINE_H
#define INVERTED_INDEX_PIPELINE_H
using namespace std;


typedef struct {FILE *f; char* buf; int curRec; int numRec;} buffer;
typedef struct {int *arr; char *cache; int size; } heapStruct;
#define KEY(z) (*(int *)(&(heap.cache[heap.arr[(z)]*recSize])))

class pipeLine {
private:
    string midWord;
    char outputPath[200];
    int lastDocID;
    int sizeofOutFile;
    int outputFileID;
    vector<DocPosting *> tempInvertedList;
    FILE *outputFile;
    ofstream ouF;
    int DocNumLastWord;
    void writeOut2File();
public:
    pipeLine();
    ~pipeLine();
    int Insert(const Posting *rawPosting);
    int WriteFinalPart();
    int GetFileID();
    int GetDocNumLastWord();

};

void createTempDoc(vector<string> dir);
void heapify(int i);
int nextRecord(int i);
void writeRecord(buffer *b, int i);
void createList();
void MergeTmpIndex();
void sort(int recordSize,int memSize, string data, string temp, string list);
void merge(int recordSize, int memSize, int maxDegree, const char *inFile, const char *outFIle, char *oPrefix );
void generateIndex();
void deleteMergeFile();
#endif //INVERTED_INDEX_PIPELINE_H
