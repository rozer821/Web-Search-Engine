//
// Created by 毕为豪 on 2019/11/1.
//

#include "index.h"
#include "query.h"
#include <istream>
#include <ostream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include "vbyte.hpp"
indexs indexTable::GetIndex(int fileID, int pointer, int length, int occurence) {
    cout << "start to load inverted Index" << endl;
    string invertedIndexPre = "/Users/nightmare/CLionProjects/inverted_index/inverted_";
    string invertedIndexSuf = ".index";
    string wholePath = invertedIndexPre+to_string(fileID)+invertedIndexSuf;
    FILE * pFile;

    if (!files.count(fileID)){
        pFile = fopen(wholePath.c_str(),"rb");
        files[fileID] = pFile;
    }
    //load data from inverted index with pointer(offset) and length of that word
    pFile = files[fileID];
    fseek(pFile,pointer,SEEK_SET);
    char buffer[length];
    fread(buffer,sizeof(uint8_t),length,pFile);
    vector<uint8_t> buf;
    for(auto j:buffer){
        buf.push_back(j);
    }
    indexs invertedList1;
    vector<uint32_t> decoded_values;
    vb::vbyte::decode(decoded_values,buf);
    int unzipInt = 0;
//    cout << "size of result " << decoded_values.size() << endl;
//    cout << "number of occurence" << occurence <<endl;
    for (int i = 0; i < occurence*4; i++) {  //occurence * 4 shoule be equal to size of decoded_vaules
        unzipInt += decoded_values[i];
        posting posting1;
        posting1.docID = unzipInt;
        posting1.freq = decoded_values[i+1];
        posting1.pos = decoded_values[i+2];
        posting1.rawpage = decoded_values[i+3];
        invertedList1.push_back(posting1);
        i += 3;
    }
    return invertedList1;
}

indexTable::indexTable() {

}
