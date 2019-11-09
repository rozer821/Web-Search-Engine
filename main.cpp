#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <map>
#include <vector>
#include "struct.h"
#include "util.h"
#include "pipeLine.h"
#include <time.h>
#include <boost/filesystem.hpp>
#include "query.h"
using namespace std;



int main() {
    //if lexiconTable is not exist, create inverted index and lexicon table.
    FILE *fh = fopen("/Users/nightmare/CLionProjects/inverted_index/lexiconTable","r");
    if (fh == NULL){
        //analysis time consume
        time_t start, middle;
        start = time(NULL);
        middle = time(NULL);
        //This dirname point to the wet file.
        string dirname = "/Users/nightmare/Downloads/Course/Web Searching Engine/HW/hw2/inverted_Index/data";
        string wetEnding = ".wet";
        vector<string> wetDataSet;
        vector<string> tempSet;

        //Find all wet file at dirname
        if (findAllFile(dirname, wetEnding, wetDataSet)){
            cout<<"Start generating temp Docs"<<endl;
            createTempDoc(wetDataSet);
            cout << "Running time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
            middle = time(NULL);
            cout<<"Start merge temp Docs"<<endl;
            MergeTmpIndex();
            cout << "Running time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
            middle = time(NULL);
        }else{
            cout << "could not find any wet file" << endl;
            exit(1);
        }

        cout<<"Start create inverted index and lexicon table"<<endl;
        //generate final inverted index and lexicon table.
        generateIndex();
        //delete temp merge file
        deleteMergeFile();
        cout << "Running time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
        cout << "All done" << endl;
        cout << "Total Running time: " << difftime(time(NULL), start) << " Seconds." << endl;

    }


    //input query
    string keys;
    while (true) {
        cout << "enter your searching key words:";
        getline(cin, keys);
        //terminate program
        if (keys == "exit()"){
            break;
        }
        //determain if input is empty
        if (keys.empty()){
            cout << "invalid input, please re-entry your key words" <<endl;
            continue;
        }

        cout << "the key words of your searching is: " << keys << endl;
        resultSet results;
        results = query::searching(keys, 0, 10);
        if (results.size() == 0){
            continue;
        }
        // display results
        auto start = std::chrono::high_resolution_clock::now();
        for (auto &result : results) {
            cout <<"Score: "<< result.bm25Score <<" URL: "<< result.docUrl <<endl;
            string last = ""; // using to check if different word has same snippet
            for (const auto& i:result.word_pos_raw){
                int pos;
                int raw;
                pos = i.second.pos_raw[0]+2;
                raw = i.second.pos_raw[1];
                string snippet = getSnippet(raw,pos);
                if (snippet!=last){
                    last = snippet;
                    result.snippet.append(snippet);
                    result.snippet.append("\n");
                }
            }
            cout << result.snippet <<endl;
            for (const auto& i:result.word_freq) {
                cout << i.first <<" "<< i.second << "|" <<endl;
            }
            cout << "docID is: "<<result.docID << endl;
            cout << "\n";
        }
        auto finish = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = finish - start;
        std::cout << "get snippet and showing result time: " << elapsed.count() << " s\n";
        results.clear();
    }







    //Alternative way to sort and merge
//    string ending2 = ".dat";
//    string tempFilePath = "/Users/nightmare/CLionProjects/inverted_index/cmake-build-debug/postinglist";
//    if (findAllFile(dirname, wetEnding, wetDataSet)){
//        cout<<"Start generating temp Docs"<<endl;
//        createTempDoc(wetDataSet);
//        cout << "Runing time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
//        middle = time(NULL);
//        char prefix[] = "result";
//        char tem_prefix[] = "temp_pfix";
//        string tem = "temp";
//        string list = "list";
//        createList();
//        cout<<"Start merge temp Docs"<<endl;
//        MergeTmpIndex();
//        cout << "Runing time: " << difftime(time(NULL), middle)<< " Seconds." << endl;
//        middle = time(NULL);
//        if (findAllFile(tempFilePath,ending2,tempSet)){
//            int n = tempSet.size();
//            ifstream fin;
//            ofstream fout("data");
//            for (int i = 0; i < n; i++) {
//                fin.open(tempFilePath + "/" + tempSet[i]);
//                string a;
//                while(getline(fin,a,'\n')){
//                    fout << a << "\n";
//                }
//                fin.close();
//                fin.clear();
//            }
//        }
//    }
    return 0;
}