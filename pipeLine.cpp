//
// Created by 毕为豪 on 2019/10/15.
//

#include "pipeLine.h"
#include "MyDB.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "util.h"
#include <boost/algorithm/string.hpp>
#include <typeinfo>
#include "vbyte.hpp"
#include <algorithm>

using namespace std;



void MergeTmpIndex()
{
    //Use unix sort to merge temp doc and delete temp doc posting file once merging procession completed.
    system("LC_ALL=C sort -S 2G -m -k 2,2 -k 1n,1n /Users/nightmare/CLionProjects/inverted_index/postinglist/*.dat -o  /Users/nightmare/CLionProjects/inverted_index/merge.index");
    system("rm -rf /Users/nightmare/CLionProjects/inverted_index/postinglist/*.dat");
}

void deleteMergeFile()
{
    // delete temp merge file at the end.
    system("rm -rf /Users/nightmare/CLionProjects/inverted_index/merge.index");
}



bool CompareDocID(Posting *posting1, Posting *posting2)
{
    return posting1->docID < posting2->docID;
}

bool CompareWord(Posting *posting1, Posting *posting2)
{
    return posting1->word < posting2->word;
}

void WriteRawPostingToFile(PostingVector *vector, int doc_file_index)
{
    ofstream ofs;
    stringstream docFileName;
    docFileName << "/Users/nightmare/CLionProjects/inverted_index/postinglist/" << doc_file_index << ".dat";
    cout << "The temp Doc posting: " <<docFileName.str() << " created\n" << endl;
    ofs.open(docFileName.str(), ios::out | ios::trunc);
    if (ofs.is_open()) {
        /*
         We need to compare DocID and Word twice to make temp doc posting has sorted by two columns
         This order of compares cannot be changed
         */
        stable_sort(vector->begin(), vector->end(), CompareDocID);
        stable_sort(vector->begin(), vector->end(), CompareWord);
        for(auto posting : *vector) {
            //write posting to temp doc posting file in format: [docID word]
            ofs << posting->docID << " " << posting->word << " "<< posting->pos << " " << posting->rawpage << "\n";
        }
        ofs.close();
        vector->clear();
    } else{
        cout<<"Failed to open temp file!"<<endl;
    }
}
void freeRawPostingVector(PostingVector *vector)
{
    for(int i=0;i<vector->size();i++) {
        Posting *posting = (*vector)[i];
        delete posting;
        (*vector)[i] = NULL;
    }

    vector->clear();
}

void createTempDoc(vector<string> dir)
{
    ifstream fin;
//    MyDB db;
    DocInfoTable docInfoTable;
    PostingVector *postingVector = new PostingVector;
    //this is self increase doc_id
    static int doc_id = 0;
    //this represent the output file name index
    int doc_file_index = 0;
    for (int i = 0; i < 5; i++) {
        vector<int> start_index; //all offsets for documents start position
        vector<string> doc;//contain all lines
        int index = 0;
        string line;
        string first;
        string dirname = "/Users/nightmare/Downloads/Course/Web Searching Engine/HW/hw2/inverted_Index/data/";
        string fileName = dirname + dir[i];
        vector <string> dir_num = split(dir[i],' ');
        int rawpagenum = stoi(dir_num[0]);
        fin.open(fileName.c_str(), ios::in);
        getline(fin, first);
        while (!fin.eof()) {
            getline(fin, line);
            doc.push_back(line);
            if (line == first) {
                start_index.push_back(index);
            }
            index++;
        }
        cout << "finish load data from " << dir[i] << " files" << endl;

        //split document from doc and create doc_content to contain all lines for one document.
        for (unsigned long q = 0; q < start_index.size()-1; q++) {
            string doc_url;
            int doc_size;
            vector<string> doc_content;
            DocInfoTableItem *item = new DocInfoTableItem;
            doc_url = doc[start_index[q] + 2].substr(17);
            doc_size = stoi(doc[start_index[q] + 8].substr(16));
            copy(doc.begin() + start_index[q] + 10, doc.begin() + start_index[q + 1] - 2, back_inserter(doc_content));
            item->docID = doc_id;
            item->docURL = doc_url;
            item->docSize = doc_size;
            docInfoTable.push_back(item);
            //doc_content contain all lines for one document
            for (int j =0; j < doc_content.size();j++) {
                //set seperators for parsing word from each doc
                const char *d = " ,*-()[]@.|+=!#$%^&{}，。~`:;<>_？「」【】《》：；、·\"\'\\/\t\v\n\r";
                char *cstr = &doc_content[j][0];
                char *p;
                p = strtok(cstr, d);
                while (p) {
                    // limit the word size to 58
                    if (strlen(p) > 58){
                        break;
                    }
                    convert(p);
                    Posting *posting = new Posting;
                    posting -> docID = doc_id;
                    posting -> word = p;
                    posting -> pos = start_index[q] + 10 + j;
//                    cout << posting -> pos <<endl;
                    posting -> rawpage =rawpagenum;
                    postingVector->push_back(posting);
                    p = strtok(NULL, d);
                }
            }
            //set the limitaion of size of postingVector, which also constrain the size of temp doc posting file.
            if (postingVector->size() > 10000000){
                WriteRawPostingToFile(postingVector,doc_file_index);
                freeRawPostingVector(postingVector);
                doc_file_index++;
            }
            doc_id++;
            /* This is a alternative way to store docTable by using Sqlite database
                       cout << doc_id << endl;
                       cout << doc_url << endl;
                       cout << doc_size << endl;
                       MyDB::addToDB(doc_id,doc_url,doc_size);
            */
        }
        if (postingVector->size() > 0){
            WriteRawPostingToFile(postingVector,doc_file_index);
            freeRawPostingVector(postingVector);
        }
        doc.clear();
        start_index.clear();
        cout << "finish word parse" << endl;
        fin.close();
    }

    cout << "start to create doc info table" << endl;
    FILE *fp = fopen("/Users/nightmare/CLionProjects/inverted_index/DocTable", "w+");
    for(int i=0;i<docInfoTable.size();i++) {
        DocInfoTableItem *urlitem = docInfoTable[i];
        fprintf(fp,"%d %s %d\n", urlitem->docID, urlitem->docURL.c_str(), urlitem->docSize);
    }
    fclose(fp);
    cout << "finish doc info table" << endl;
}

//The construction function and destruct funtion
pipeLine::pipeLine()
{

    outputFileID=0;
    sprintf(outputPath, "/Users/nightmare/CLionProjects/inverted_index/inverted_%d.index", outputFileID);
    sizeofOutFile = 0;


//    outputFile = fopen(outputPath, "a");
    ouF.open(outputPath,ios::out | ios::binary);


}
pipeLine::~pipeLine()

{
//    fclose(outputFile);
    ouF.close();
}

int pipeLine::GetFileID()
{
    return outputFileID;
}

int pipeLine::GetDocNumLastWord()
{
    return DocNumLastWord;
}

int pipeLine::WriteFinalPart()
{
    writeOut2File();
    tempInvertedList.clear();
    return sizeofOutFile;
}

void pipeLine::writeOut2File()
{
    //Setting the size of each inverted index file
    if(sizeofOutFile > 200000000) {
        // close output file
        ouF.close();
//        fclose(outputFile);

        outputFileID++;
        //open new file for inverted index
        sprintf(outputPath, "/Users/nightmare/CLionProjects/inverted_index/inverted_%d.index", outputFileID);

        ouF.open(outputPath,ios::out);
//        outputFile = fopen(outputPath, "a");

        sizeofOutFile = 0;
    }
    DocNumLastWord = tempInvertedList.size();

    //uncommant this line will generate a inverted index with word as header.
    //sizeofOutFile += fprintf(outputFile, "%s ", midWord.c_str());
    std::vector<uint8_t> buf;
    std::vector<uint32_t> value;
    for(int j=0; j < tempInvertedList.size(); j++) {
        DocPosting *docSet =tempInvertedList[j];
        value = {docSet->docID,docSet->freq,docSet->pos,docSet->rawpage};
        vb::vbyte::encode(buf, value);
        char a [buf.size()];
        char * cp = a;
        for (int k = 0; k < buf.size(); ++k) {
            a[k] = (char) buf[k];
        }
        ouF.write(cp, buf.size());
        sizeofOutFile += sizeof(uint8_t) * buf.size();
        buf.clear();
        value.clear();

        delete docSet;
        tempInvertedList[j] = NULL;
    }
//    sizeofOutFile += fprintf(outputFile, "\n");
    tempInvertedList.clear();
    vector<DocPosting *> tmp;
    tempInvertedList.swap(tmp);
}

int pipeLine::Insert(const Posting *rawPosting)
{

    // first insert, new word;
    if(tempInvertedList.size() == 0) {
        midWord = rawPosting->word;
        DocPosting * docTuple = new DocPosting;
        lastDocID = rawPosting->docID;

        docTuple->actualDocID = rawPosting->docID;
        docTuple->docID = rawPosting->docID;
        docTuple->freq =1;
        docTuple->pos = rawPosting->pos;
        docTuple->rawpage = rawPosting->rawpage;

        tempInvertedList.push_back(docTuple);
    } else if (midWord != rawPosting->word){
        //write and free the memory
        writeOut2File();
        midWord = rawPosting->word;
        DocPosting * docTuple = new DocPosting;
        lastDocID = rawPosting->docID;
        docTuple->actualDocID = rawPosting->docID;
        docTuple->docID = rawPosting->docID;
        docTuple->freq=1;
        docTuple->pos = rawPosting->pos;
        docTuple->rawpage = rawPosting->rawpage;

        tempInvertedList.push_back(docTuple);
        return sizeofOutFile;
    } else {
        // if it is same doc, get the last doc and plus one to its frequency
        if(rawPosting->docID == lastDocID) {
            DocPosting * docTuple = tempInvertedList.back();
            docTuple->freq++;
        } else {
            // if it is different doc, set offset of docID
            DocPosting * docTuple = new DocPosting;
            lastDocID = rawPosting->docID;
            docTuple->actualDocID = rawPosting->docID;
            // the docID will compress by using offset of docID
            docTuple->docID = (rawPosting->docID - tempInvertedList.back()->actualDocID);
            docTuple->freq =1;
            docTuple->pos = rawPosting->pos;
            docTuple->rawpage = rawPosting->rawpage;

            tempInvertedList.push_back(docTuple);
        }
    }
    // 0 means no data written
    return 0;
}

void generateIndex(){
    ifstream fin("/Users/nightmare/CLionProjects/inverted_index/merge.index");
    string s;
    Posting posting;
    LexiconTable lexiconTable;
    pipeLine pipeLine;
    string lastWord = "";
    int lastCounter = 0;
    int flag = 0;

    while( getline(fin,s) ){
        vector <string> set;
        string seperator = " ";
        SplitString(s,set,seperator);

        posting.docID = stoi(set[0]);
        posting.word= set[1];
        posting.pos = stoi(set[2]);
        posting.rawpage = stoi(set[3]);

        // If the last word is empyt means this is the first seen
        if(lastWord == "") {
            lastWord = posting.word;
            LexiconItem *lexiconItem = new LexiconItem;
            lexiconItem->word = posting.word;
            lexiconItem->invertedPointer = 0;
            lexiconTable.push_back(lexiconItem);
        }
        // flag has two function, first to determine wheather the word is new word, the seoncd func is also a offset of pointer to word.
        flag = pipeLine.Insert(&posting);
        // if flag > 0 means it is the new word, and we also need to create new lexicon item for it.
        if(flag > 0) {
            LexiconItem *lexiconItem = lexiconTable.back();
            lexiconItem->fileID = pipeLine.GetFileID();
            lexiconItem->num = pipeLine.GetDocNumLastWord();
            if(flag < lastCounter) {
                lexiconItem->invertedPointer = 0;
            }
            lexiconItem->size = flag - lexiconItem->invertedPointer;
            lexiconItem = new LexiconItem;
            lexiconItem->word = posting.word;
            lastWord = posting.word;
            lexiconItem->invertedPointer = flag;
            lexiconTable.push_back(lexiconItem);
            lastCounter = flag;
        }

    }
        //write out final part.
    flag = pipeLine.WriteFinalPart();
    lexiconTable.back()->fileID = pipeLine.GetFileID();
    lexiconTable.back()->num = pipeLine.GetDocNumLastWord();
    if(flag > 0 && flag < lastCounter) {
        lexiconTable.back()->invertedPointer = 0;
    }
    lexiconTable.back()->size = flag - lexiconTable.back()->invertedPointer;
    writeLexiconTable(&lexiconTable);
}

/*
 * Code below is the alternative way to sort and merge
 * This use heap sort and
 *
 * */


//
//int intCompare(const void *r1, const void *r2)
//{  return *(char*)r1 - *(char*)r2; }
//
//void sort(int recordSize,int memSize, string data, string temp, string list) {
//    FILE *fin, *fout, *foutlist;
//    unsigned char *buf;        /* buffer holding records */
//    int recSize, bufSize;      /* size of a record, and # of records in buffer */
//    int numRecs, numFiles = 0; /* number of records, and # of output files */
//    char filename[1024];
//    int i;
//
//    recSize = recordSize;
//    buf = (unsigned char *) malloc(memSize);
//    bufSize = memSize / recSize;
//
//    fin = fopen(data.c_str(), "r");
//    foutlist = fopen(list.c_str(), "w");
//
//    while (!feof(fin))
//    {
//        /* read data until buffer full or input file finished */
//        for (numRecs = 0; numRecs < bufSize; numRecs++)
//        {
//            fread(&(buf[numRecs*recSize]), recSize, 1, fin);
//            if (feof(fin))  break;
//        }
//
//        /* create output filename, store sorted data, then store the filename */
//        if (numRecs > 0)
//        {
//            sprintf(filename, "%s%d", temp.c_str(), numFiles);;
//            fout = fopen(filename, "w");
//            qsort((void *)(buf), numRecs, recSize, intCompare);
//            for (i = 0; i < numRecs; i++)
//                fwrite(&buf[i*recSize], recSize, 1, fout);
//            fclose(fout);
//            fprintf(foutlist, "%s\n", filename);
//            numFiles++;
//        }
//    }
//
//    fclose(fin);
//    fclose(foutlist);
//    free(buf);
//}
//
//
//buffer *ioBufs;          /* array of structures for in/output buffers */
//heapStruct heap;         /* heap structure */
///* size of record (in bytes) */
//int bufSize;
//int recSize;
///* standard heapify on node i. Note that minimum is node 1. */
//void heapify(int i)
//
//{
//    int s, t;
//
//    s = i;
//    while(1)
//    {
//        /* find minimum key value of current node and its two children */
//        if (((i<<1) <= heap.size) && (KEY(i<<1) < KEY(i)))  s = i<<1;
//        if (((i<<1)+1 <= heap.size) && (KEY((i<<1)+1) < KEY(s)))  s = (i<<1)+1;
//
//        /* if current is minimum, then done. Else swap with child and go down */
//        if (s == i)  break;
//        t = heap.arr[i];
//        heap.arr[i] = heap.arr[s];
//        heap.arr[s] = t;
//        i = s;
//    }
//}
//int nextRecord(int i)
//{
//    buffer *b = &(ioBufs[i]);
//
//    /* if buffer consumed, try to refill buffer with data */
//    if (b->curRec == b->numRec)
//        for (b->curRec = 0, b->numRec = 0; b->numRec < bufSize; b->numRec++)
//        {
//            fread(&(b->buf[b->numRec*recSize]), recSize, 1, b->f);
//            if (feof(b->f))  break;
//        }
//
//    /* if buffer still empty, return -1; else copy next record into heap cache */
//    if (b->numRec == 0)  return(-1);
//    memcpy(heap.cache+i*recSize, &(b->buf[b->curRec*recSize]), recSize);
//    b->curRec++;
//    return(i);
//}
//void writeRecord(buffer *b, int i)
//
//{
//    int j;
//
//    /* flush buffer if needed */
//    if ((i == -1) || (b->curRec == bufSize))
//    {
//        string str;
//        str = b->buf;
//
//        for (j = 0; j < b->curRec; j++)
//
//            cout << &(b->buf[(j+1)*recSize]) << endl;
//
//            fwrite(&(b->buf[j*recSize]), recSize, 1, b->f);
//        b->curRec = 0;
//    }
//
//    if (i != -1)
//        memcpy(&(b->buf[(b->curRec++)*recSize]), heap.cache+i*recSize, recSize);
//}
//void createList(){
//    ofstream outfile;
//    string dirName = "/Users/nightmare/CLionProjects/inverted_index/cmake-build-debug/postinglist";
//    string ending = ".dat";
//    vector <string> datSet;
//    outfile.open("/Users/nightmare/CLionProjects/inverted_index/list");
//
//    if (findAllFile(dirName,ending,datSet)){
//        for(auto &i : datSet){
//            outfile << "/Users/nightmare/CLionProjects/inverted_index/cmake-build-debug/postinglist/"<<i << endl;
//        }
//    }
//    outfile.close();
//}
//void merge(int recordSize, int memSize, int maxDegree, const char *inFile, const char *outFIle, char *oPrefix ) {
//
//
//    /* # of records that fit in each buffer */
//
//    recSize = recordSize;
//    FILE *finlist, *foutlist;  /* files with lists of in/output file names */
//
//    int degree;   /* max allowed merge degree, and actual degree */
//    int numFiles = 0;        /* # of output files that are generated */
//    char *bufSpace;
//    char filename[1024];
//    int i;
//
//    bufSpace = (char *) malloc(memSize);
//    ioBufs = (buffer *) malloc((maxDegree + 1) * sizeof(buffer));
//    heap.arr = (int *) malloc((maxDegree + 1) * sizeof(int));
//    heap.cache = (char *) malloc(maxDegree * recSize);
//    string line;
//    finlist = fopen(inFile, "r");
//    foutlist = fopen(outFIle, "w");
//
//    while (!feof(finlist))
//    {
//
//        for (degree = 0; degree < maxDegree; degree++)
//        {
//            fscanf(finlist, "%s", filename);
//            cout << filename << endl;
//            if (feof(finlist)) break;
//            ioBufs[degree].f = fopen(filename, "r");
//        }
//        if (degree == 0) break;
//
//        /* open output file (output is handled by the buffer ioBufs[degree]) */
//        sprintf(filename, "%s%d", oPrefix, numFiles);
//        ioBufs[degree].f = fopen(filename, "w");
//
//        /* assign buffer space (all buffers same space) and init to empty */
//        bufSize = memSize / ((degree + 1) * recSize);
//        for (i = 0; i <= degree; i++)
//        {
//            ioBufs[i].buf = &(bufSpace[i * bufSize * recSize]);
//            ioBufs[i].curRec = 0;
//            ioBufs[i].numRec = 0;
//        }
//
//        /* initialize heap with first elements. Heap root is in heap[1] (not 0) */
//        heap.size = degree;
//        for (i = 0; i < degree; i++)  heap.arr[i+1] = ::nextRecord(i);
//        for (i = degree; i > 0; i--)  ::heapify(i);
//
//        /* now do the merge - ridiculously simple: do 2 steps until heap empty */
//        while (heap.size > 0)
//        {
//            /* copy the record corresponding to the minimum to the output */
//            ::writeRecord(&(ioBufs[degree]), heap.arr[1]);
//
//            /* replace minimum in heap by the next record from that file */
//            if (::nextRecord(heap.arr[1]) == -1)
//                heap.arr[1] = heap.arr[heap.size--];     /* if EOF, shrink heap by 1 */
//            if (heap.size > 1)  ::heapify(1);
//        }
//
//        /* flush output, add output file to list, close in/output files, and next */
//        ::writeRecord(&(ioBufs[degree]), -1);
//        fprintf(foutlist, "%s\n", filename);
//        for (i = 0; i <= degree; i++)  fclose(ioBufs[i].f);
//        numFiles++;
//    }
//    fclose(finlist);
//    fclose(foutlist);
//    free(ioBufs);
//    free(heap.arr);
//    free(heap.cache);
//
//}
