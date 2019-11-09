//
// Created by 毕为豪 on 2019/10/8.
//

#ifndef INVERTED_INDEX_UTIL_H
#define INVERTED_INDEX_UTIL_H

#include <string>

using namespace std;
bool hasEnding (std::string const &fullString, std::string const &ending);
bool findAllFile (const string& inputPath, const string& ending, vector<string> &fileSet);
void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c);

double getBM25(int freq, int numContain, int totalDoc, int docLength, int docLengthAvg);
template <typename Out>
void split(const std::string &s, char delim, Out result);
std::vector<std::string> split(const std::string &s, char delim);
std::fstream& GotoLine(std::fstream& file, unsigned int num);
string getSnippet(int rawPageNum,int Pos);
char* convert(char *src);
class util {

};


#endif //INVERTED_INDEX_UTIL_H
