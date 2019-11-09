//
// Created by 毕为豪 on 2019/10/31.
//
#include <vector>
#include <string>

#ifndef INVERTED_INDEX_URLTABLE_H
#define INVERTED_INDEX_URLTABLE_H

using namespace std;

typedef struct {
    string url;
    int fileID;
    int pageSize;
}urlTableItem;
typedef vector<urlTableItem> urlSet;

class urlTable {
private:


public:
    urlTableItem getDoc(int docID);
    urlSet array;
    double avgdl;
    int size;
    urlTable();
    ~urlTable();
};


#endif //INVERTED_INDEX_URLTABLE_H
