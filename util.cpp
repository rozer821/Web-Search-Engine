//
// Created by 毕为豪 on 2019/10/8.
//

#include "util.h"
#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <vector>
#include <math.h>
#include <sstream>

bool hasEnding (std::string const &fullString, std::string const &ending) {
    /*
     * check if string have certain suffix
     * @para fullString
     *       ending: suffix
     * @return bool value
     * */
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

bool findAllFile (const string& inputPath, const string& ending, vector<string>&fileSet){
    /*
     * find all files in certain directory
     * */
    const string& dirName = inputPath;
    DIR *dp;

    struct  dirent *dirp;
    if ((dp = opendir(dirName.c_str())) == NULL) {cout << "can't open" << dirName << endl;return false;}
    while ((dirp = readdir(dp)) != NULL){
        string name = dirp -> d_name;
        if (hasEnding(name, ending)) {
            fileSet.push_back(name);
        }
    }
    closedir(dp);
    return true;
}

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{

    std::string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(std::string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

double getBM25(int freq, int numContain, int totalDoc, int docLength, int docLengthAvg)
{
    /*
     * calculate BM25 score
     * */
    double k = 1.2;
    double b = 0.75;
    double idf = log(((totalDoc-numContain)+0.5)/(numContain+0.5));
    if (idf<0){
        idf = 0;
    }
    double score = idf * (freq*(k+1)/(freq+k*(1-b+b*docLength/docLengthAvg)));
    return score;
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

std::fstream& GotoLine(std::fstream& file, unsigned int num){
    /*
     * Go to specific line in file
     * */

    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

string getSnippet(int rawPageNum,int Pos){
    /*
     * get snippet from raw page
     * */
    string dirname = "/Users/nightmare/Downloads/Course/Web Searching Engine/HW/hw2/inverted_Index/data/";
    string wholepath;
    string suffix = " .wet";
    string snippet;
    wholepath = dirname + to_string(rawPageNum) + suffix;
//    cout << wholepath <<endl;
    fstream pFile;
    pFile.open(wholepath,ios::in);
    GotoLine(pFile, Pos);
    snippet.append(".....");
    string s;
    while(getline(pFile,s)){
        snippet.append(s);
        if (snippet.length() > 50){
            snippet.append(".....");
            break;
        }
    };
//    cout << snippet << endl;
    pFile.close();
    return snippet;
}
char* convert(char *src)
{
    /*
     * convert uppercase to lowercase
     *
     * */
    char *p = src;
    assert(p != NULL);
    while(*p)
    {
        if ('A' <= *p && *p < 'Z')
            *p |= 0x20;

        p++;
    }
    return src;
}
