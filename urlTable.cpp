//
// Created by 毕为豪 on 2019/10/31.
//

#include "urlTable.h"
#include <string>
#include "query.h"
#include "util.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <time.h>


urlTable::urlTable() {
    /*
     * The initialization function load url table from file
     * The url table will be loaded into main memory
     * */
    auto start = std::chrono::high_resolution_clock::now();
    cout << "urlTable loading" <<endl;
    string docPath = "/Users/nightmare/CLionProjects/inverted_index/DocTable";
    ifstream docin;
    docin.open(docPath);
    int number_of_lines = 0;
    string line;
    long counter = 0;
    while (getline(docin, line)) {
        urlTableItem item;
        vector<string> urlInfo = split(line,' ');
        item.fileID = stoi(urlInfo[0]);
        item.url = urlInfo[1];

        item.pageSize = stoi(urlInfo[2]);
        array.push_back(item);
        counter += stoi(urlInfo[2]);
        number_of_lines++;

    }

    cout << "urlTable loading finished" <<endl;
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "Running time of url table: " << elapsed.count() << " s\n";
    size = number_of_lines;
    avgdl = counter/size;
}

urlTableItem urlTable::getDoc(int docID) {

    return array[docID];
}

urlTable::~urlTable() = default;
