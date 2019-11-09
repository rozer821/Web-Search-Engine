//
// Created by 毕为豪 on 2019/10/9.
//

#include "MyDB.h"
#include <string>
#include <iostream>
using namespace std;

sqlite3 *db;
char *zErrMsg;
int rc;




void MyDB::addToDB(long doc_id, string doc_url, long doc_size) {
    string sql;
    sql = "INSERT INTO doc_info(DocID,d_url,d_size) VALUES("+to_string(doc_id)+",'" +doc_url+ "',"+to_string(doc_size)+");";


    rc = sqlite3_exec(db,sql.c_str(),callback,0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
//        exit(0);
    } else {
        fprintf(stdout, "Records created successfully\n");
    }

}

MyDB::MyDB() {
    rc = sqlite3_open("/Users/nightmare/CLionProjects/inverted_index/DocDB.db", &db);
    if( rc ) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));

    } else {
        fprintf(stderr, "Opened database successfully\n");
    }
}

int MyDB::callback(void *NotUsed, int argc, char **argv, char **azColName) {
    int i;
    for(i = 0; i<argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

MyDB::~MyDB() {
    sqlite3_close(db);
}


