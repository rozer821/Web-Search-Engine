//
// Created by 毕为豪 on 2019/10/31.
//

#include <string>
#include "query.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

using namespace std;

vector<string> query::parseKeywords(string keyWords){
    /*
     * eliminate space in query
     * @param: The input keywords
	 * @return: The set of words after parse
     *
     * */
    vector <string> results;
    vector <string> tempSet = split(keyWords,' ');
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < tempSet.size(); ++i) {
        if (strlen(tempSet[i].c_str())){
            int length = tempSet[i].length();
            char word[length+1];
            strcpy(word, tempSet[i].c_str());
            convert(word); // convert input to lowercase
            results.push_back(word);
        }
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "key parse time: " << elapsed.count() << " s\n";
    return results;
}

resultSet query::searching(const string& queryString, int start, int limit){
    /*
     * The entrance of seaching
     * @param: queryString: the input keywords
     *         start: the results start from rank k
     *         limit: how many results output
	 * @return: The set of result
     *
     * */

    resultSet temp = querystart(queryString);
    if (temp.size() == 0){
        return temp;
    }
    int resultSize = temp.size();
    if (start > resultSize - 1){
        cout << "start index large than the result set" << endl;
        exit(1);
    }
    sort(temp.begin(), temp.end(),
         [](const resultItem & a, const resultItem & b) -> bool
         {
             return a.bm25Score > b.bm25Score;
         });
    int startIndex = start;
    int endIndex = startIndex + limit;
    if (endIndex > resultSize){
        endIndex = resultSize;
    }
    resultSet queryResult;
    for (int i = 0; i < endIndex; ++i) {
        queryResult.push_back(temp[i]);
    }
    return queryResult;
}





resultSet query::querystart(const string& keyWord) {
    /*
    * The processing of query
    * @param: keywords: a string of keywords without parse.
    * @return: The set of result
    *
    * */

    int flag = 1;
    int is_zero = 0;
    //initial result set
    resultSet resultset;
    //initial url table
    static urlTable urltable;
    //analysis input key word
    vector <string> keyWords = parseKeywords(keyWord);
    int num = keyWords.size();
    cout << num << " words received" <<endl;
    //generate correspond plist
    vector <pplist> vpp;
    vpp.reserve(num);
    for (int j = 0; j < num; ++j) {
        cout << "open list for word: \""<< keyWords[j] <<"\""<< endl;
            vpp.push_back(query::openppList(keyWords[j]));
            if (vpp[j].size == 0){
                is_zero = 1;
            }
    }
    // if is_zero is 1, that means one of keywords cannot be found.
    if (is_zero==1){
        for (int j = 0; j < num; ++j) {
            if (vpp[j].size != 0) {
                flag = 0;
                break;
            }else{
                return resultset;
            }
        }
    }

    cout << "start sorting plist" <<endl;

    //sort plist by its size, ascending
    sort(vpp.begin(), vpp.end(),
         [](const pplist & a, const pplist & b) -> bool
         {
             return a.size < b.size;
         });


    //when docID smaller than configuration. generate all plist
    auto start_neq = std::chrono::high_resolution_clock::now();
    if (flag == 1){
        //if flag == 1 means there is a page that contain all words
        int did = 0;
        while (did <= 44969)
        {
            did = query::nextGEQ(vpp[0],did);
            if (did == 44969)
            {
                break;
            }
            int d = -1;
            for (int i = 1; i < num; ++i) {
                d = query::nextGEQ(vpp[i],did);
                if(d != did){
                    break;
                }
            }
            if (d != -1 and d > did){
                did = d;
            }else{
                resultItem item;
                item.docID = did;
                item.docUrl = urltable.getDoc(did).url;
                double score = 0;
                for (int i = 0; i < num; ++i) {
                    int freq = query::getFreq(vpp[i],did);
                    item.word_freq[vpp[i].word] = freq;
                    int rawpage = query::getRawPage(vpp[i],did);
                    int pos = query::getPos(vpp[i],did);
                    pos_raw pos_rawSet;
                    pos_rawSet.pos_raw[0] = pos;
                    pos_rawSet.pos_raw[1] = rawpage;
                    item.word_pos_raw[vpp[i].word] = pos_rawSet;
                    score += getBM25(freq,vpp[i].size,urltable.size,urltable.getDoc(did).pageSize,urltable.avgdl);
                }
                item.bm25Score = score;
                resultset.push_back(item);
                did ++;
            }
        }
    }


    if (resultset.size()<10){
        for(int k =0; k<vpp.size();k++){
            int did = 0;
            while (did <= 44969)
            {
                did = query::nextGEQ(vpp[k],did);
                if (did == 44969)
                {
                    break;
                }
                resultItem item;
                item.docID = did;
                item.docUrl = urltable.getDoc(did).url;
                double score = 0;

                int freq = query::getFreq(vpp[k],did);
                item.word_freq[vpp[k].word] = freq;
                int rawpage = query::getRawPage(vpp[k],did);
                int pos = query::getPos(vpp[k],did);
                pos_raw pos_rawSet;
                pos_rawSet.pos_raw[0] = pos;
                pos_rawSet.pos_raw[1] = rawpage;
                item.word_pos_raw[vpp[k].word] = pos_rawSet;
                score += getBM25(freq,vpp[k].size,urltable.size,urltable.getDoc(did).pageSize,urltable.avgdl);
                item.bm25Score = score;
                resultset.push_back(item);
                did ++;
            }
        }
    }
    auto finish_neq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish_neq - start_neq;
    std::cout << "Running time of seeking doc: " << elapsed.count() << " s\n";
    cout <<"number of total document "<< urltable.size << " number of average document length " << urltable.avgdl<< endl;
    return resultset;

}


query::query() {
//    unordered_map<> snippetCache;
}

unordered_map<string,indexs> query::indexCache;
//if key has been serached, get it from cache
indexs query::getFromCache(const string& keyWord) {
    /*
     * get posting list from cache
     * */
    auto start_cache = std::chrono::high_resolution_clock::now();
    if (indexCache.count(keyWord)){
        auto finish_cache = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish_cache - start_cache;
        std::cout << "get from cache time: " << elapsed.count()*2 << " s\n";
        return indexCache[keyWord];

    }else{
        //if not get it from file
        cout << "couldn't find "<<keyWord <<" at cache" << endl;
        return getFromFile(keyWord);
    }
}
//load key from lexicon table
indexs query::getFromFile(const string &keyWord) {
    /*
     * get posting list from file only if cannot find posting at cache.
     * */
    static lexiconTable lexicontable1;
    if (lexicontable1.dict.count(keyWord)){
        cout << "find "<< keyWord <<" at file" <<endl;
        lexicon lexicon1 = lexicontable1.dict[keyWord];
        indexTable indextable1;
        indexs indexItems = indextable1.GetIndex(lexicon1.fileID,lexicon1.pointer,lexicon1.size,lexicon1.occurence);
        indexCache[keyWord] = indexItems;
        return indexItems;
    }else{
        cout << "could not found word "<< keyWord <<" at anywhere"<< endl;
        indexs emptyIndexItem;
        return emptyIndexItem;
    }
}


pplist query::openppList(const string& keyWord) {
    /*
     * open posting list and set it
     * */
    indexs indexs1 = getFromCache(keyWord);
    pplist plist;
    plist.indexset = indexs1;
    plist.cursor = 0;
    plist.size = indexs1.size();
    plist.word = keyWord;
    return plist;
}

int query::nextGEQ(pplist &pList, int docID) {
    int start = pList.cursor;
    for (int i = start; i < pList.size; ++i) {
        pList.cursor ++;

        int a = pList.indexset[i].docID;
        if (a >= docID) {
            return a;
        }
    }
    return 44969;
}

int query::getFreq(pplist &pList, int docID) {

    if (pList.indexset[pList.cursor - 1].docID == docID){
        return pList.indexset[pList.cursor -1 ].freq;
    }

    return -1;
}

int query::getPos(pplist &pList, int docID) {

    if (pList.indexset[pList.cursor - 1].docID == docID){
        return pList.indexset[pList.cursor -1 ].pos;
    }

    return -1;
}

int query::getRawPage(pplist &pList, int docID) {

    if (pList.indexset[pList.cursor - 1].docID == docID){
        return pList.indexset[pList.cursor -1 ].rawpage;
    }

    return -1;
}



query::~query() = default;



