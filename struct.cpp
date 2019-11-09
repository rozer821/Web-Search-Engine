//
// Created by 毕为豪 on 2019/10/6.
//

#include "struct.h"

void writeLexiconTable(LexiconTable *lexiconTable)
{
    /*
     * Write lexicon table out to file
     * @para lexiconTable: a set of lexicon items
     * */
        FILE *fp = fopen("/Users/nightmare/CLionProjects/inverted_index/lexiconTable", "w+");
        for(int i=0;i<lexiconTable->size();i++) {
            LexiconItem *lexiconItem =(*lexiconTable)[i];
            fprintf(fp,"%s %d %d %d %d\n", lexiconItem->word.data(), lexiconItem->num, lexiconItem->fileID, lexiconItem->invertedPointer, lexiconItem->size);
        }
        fclose(fp);
}
