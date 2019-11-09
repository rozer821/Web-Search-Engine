//
// Created by 毕为豪 on 2019/10/31.
//

#ifndef INVERTED_INDEX_LEXICONTABLE_H
#define INVERTED_INDEX_LEXICONTABLE_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include "query.h"
using namespace std;
typedef struct {
    int occurence;
    int fileID;
    int pointer;
    int size;
}lexicon;
typedef std::vector<lexicon> lexiconSet;

class lexiconTable {
public:
    unordered_map<string,lexicon> dict;
    lexiconTable();
    ~lexiconTable();
    lexicon getLexicon(const string& word);
    int getLengthOfLexiconTable();
};


#endif //INVERTED_INDEX_LEXICONTABLE_H
