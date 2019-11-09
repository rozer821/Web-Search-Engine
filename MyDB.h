//
// Created by 毕为豪 on 2019/10/9.
//

#ifndef INVERTED_INDEX_MYDB_H
#define INVERTED_INDEX_MYDB_H
#include <sqlite3.h>
#include <string>
using namespace std;

class MyDB {
    static int callback(void *NotUsed, int argc, char **argv, char **azColName);

public:
    MyDB();
    ~MyDB();
    static void addToDB(long doc_id, string doc_url, long doc_size);
};


#endif //INVERTED_INDEX_MYDB_H
