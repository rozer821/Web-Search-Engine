//
// Created by 毕为豪 on 2019/11/1.
//
#include <string>
#include <unordered_map>
#include <vector>
#ifndef INVERTED_INDEX_INDEX_H
#define INVERTED_INDEX_INDEX_H

using namespace std;

typedef struct {
    int docID = -1;
    int freq = -1;
    int pos = -1;
    int rawpage = -1;
}posting;
typedef vector<posting> indexs;

class indexTable {
private:
    unordered_map<int,FILE*> files;
public:
    indexTable();
    indexs GetIndex(int fileID, int pointer, int length, int occurence);
};


#endif //INVERTED_INDEX_INDEX_H
