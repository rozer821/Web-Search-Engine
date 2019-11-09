//
// Created by 毕为豪 on 2019/10/31.
//

#ifndef INVERTED_INDEX_QUERY_H
#define INVERTED_INDEX_QUERY_H

#include <vector>
#include <string>
#include "urlTable.h"
#include "lexiconTable.h"
#include "index.h"
#include <unordered_map>
#include <time.h>

using namespace std;


typedef struct {
    indexs indexset;
    int cursor;
    int size;
    string word;
}pplist;

typedef struct{
    int pos_raw[2];
}pos_raw;

typedef struct {
    int docID;
    string docUrl;
    double bm25Score;
//    int pos_raw[2];
    unordered_map<string,int>word_freq;
    unordered_map<string,pos_raw>word_pos_raw;
    string snippet;
}resultItem;
typedef vector<resultItem> resultSet;

std::vector<std::string> split(const std::string &s, char delim);
class query {
public:
    static resultSet searching(const string& queryString, int start, int limit);
    query();
    ~query();

private:
    static unordered_map<string,indexs> indexCache;
    static indexs getFromCache(const string& keyWord);
    static indexs getFromFile(const string& keyWord);
    static int nextGEQ(pplist &pList, int docID);
    static int getFreq(pplist &pList, int docID);
    static int getPos(pplist &pList, int docID);
    static int getRawPage(pplist &pList, int docID);
    static resultSet querystart(const string& keyWord);
    static pplist openppList(const string& keyWord);
    static vector<string> parseKeywords(string keyWords);
};


#endif //INVERTED_INDEX_QUERY_H
