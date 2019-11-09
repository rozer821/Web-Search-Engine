//
// Created by 毕为豪 on 2019/11/4.
//

#include "pList.h"

pList::pList(indexs indexs1) {
    indexset = indexs1;

    if (!indexset.empty()){
        size = indexset.size();
    }else{
        size = 0;
    }
}
