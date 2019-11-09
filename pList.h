//
// Created by 毕为豪 on 2019/11/4.
//

#ifndef INVERTED_INDEX_PLIST_H
#define INVERTED_INDEX_PLIST_H

#include "query.h"

using namespace std;
class pList {
    public:
        pList(indexs indexs1);
        indexs indexset;
        int cursor;
        int size;
};


#endif //INVERTED_INDEX_PLIST_H
