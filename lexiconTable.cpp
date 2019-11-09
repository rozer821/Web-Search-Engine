//
// Created by 毕为豪 on 2019/10/31.
//

#include "lexiconTable.h"
#include <time.h>
#include <chrono>
lexiconTable::lexiconTable(){
    /*
     * The initialization function load lexicon table from file
     * The lexicon table will be loaded into main memory
     * */
    static lexiconSet lexiconset;
    if (lexiconset.empty()){
        auto start = std::chrono::high_resolution_clock::now();
        cout << "load lexicontable" <<endl;
        string lexiconTable = "/Users/nightmare/CLionProjects/inverted_index/lexiconTable";
        ifstream fin;
        fin.open(lexiconTable,ifstream::in);
        string line;
        while (getline(fin,line)){
            lexicon item;
            vector<string> lexiconInfo = split(line, ' ');
            item.occurence = stoi(lexiconInfo[1]);
            item.fileID = stoi(lexiconInfo[2]);
            item.pointer = stoi(lexiconInfo[3]);
            item.size = stoi(lexiconInfo[4]);
            lexiconset.push_back(item);
            dict[lexiconInfo[0]] = item;
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "Running time of generate lexicon set: " << elapsed.count() << " s\n";
        cout << "lexicontable loading finished" <<endl;
//        cout << "Running time of generate lexicon set: " << difftime(time(NULL), middle)<< " Seconds." << endl;
    }

}



lexiconTable::~lexiconTable() {

}

lexicon lexiconTable::getLexicon(const string& word) {
    return dict[word];
}

int lexiconTable::getLengthOfLexiconTable() {
    return dict.size();
}

