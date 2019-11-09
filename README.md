# Web-Search-Engine
## IMPLEMENT WEB SEARCH ENGINE BY USING INVERTED INDEX
### Assignment for Web Search Engine at NYU
### Weihao Bi wb832@nyu.edu
## INTORDUCTION

The second part is query word from inverted index made by first step. In this part, program take user’s input as keyword and query it, return the ranked k list result URL and snippet(fig.2).
 The whole project is written in C++ and create several index files in particular path. At the same time, document information table and lexicon table are also created by it.
![fig1](/pic/Picture1.png)
fig.1
![fig2](/pic/Picture2.png)
fig.2
## HOW IT RUN INTERNALLY

There are two main modules in this program called: Pipeline and Query, which correspond to the two parts in this program.

### PIPELINE
Pipeline control the generation of index file. And how it works can be summarized as several steps:
1. With default setting, this project will load data from wet file. Since there are more than one wet file, we will do a loop reading for wet file. 
2. The format of wet file contain header and body, the header contains some basic URL information, with that information, we could create Documentation information table.
3. When collect raw document, we record the line number of documents start and end, then put all lines between that two lines to a vector called doc_content. In the same time, we also record the line number related to particular word for further snippet construction.
4. Since doc_content contain lines rather the word, we also need to parse the line into words. In this case, we use strtok() to achieve this, we set the separator and the return data is a set of words divide by separator. In the meantime, we convert capital letter to lowercase.
5. With all words, we could create postings from each document. And append to postingVector.
6. To make sure the file won't be too large and excess the size of memory, when the document posting size larger then 10000000, the temporary document posting file will close and open a new file. The name of temp file is <doc_id>.dat
7. When writing out the file, we will sort the all documents postings in that file, in WriteRawPostingToFile() function, we will sort all postings by its DocID and word, in the fixed order. This step will make sort merge phase quicker. 
8. Once all temp files created, we also collect all document information, we then generate docInfoTable.
9. With all temp files, the next procedure is sort and merge, using Unix sort to do so. The sort command is: LC_ALL=C sort -S 2G -m -k 2f,2f -k 1n,1n <your_path_to+temp_files>*.dat -o  <The_target_path>merge.index after merge, delete all temp file to keep storage in decent situation.
10. The output file will be a huge file and contain all sort posting, we could do the last step -- generate inverted index and generate lexicon table. Reading the merge file line by line, when meet new word, we will create new lexicon item and put this word into a temp posting vector. Because one certain word can be in one document several times and different document as well, we need to calculate the word frequency in particular document and documents for that word. Since the memory is limited, we also cannot load all lines in main memory, we will record the size of inverted index file and if the file is larger than 200000000, the new file will be created. In the end, Inverted files and lexicon table will be created.
11. This program uses variable byte encoding algorithm to encode and decode for inverted index.

### QUERY
Query is a module to deal with query process, it can receive input from command line and return ranked results. The way how it works is:
1. To start query, the program will check if all files have already created by first part, if not, the pipeline will be called again. After that, there will prompt an input requirement that ask user to type the keyword they want to search.
2. With word received, the next step is check if input words are legal (empty or white space, etc.). Also, the input words will be converted to lowercase in the same time.
3. Then, the program will call search() function and it start to load lexicon table and document information table into memory. This step may cost several minutes depends on the size of raw pages. But only for the first time, after the first time, each query will look up those two files in memory directly rather load it again.
4. This project used DAAT query processing, the reason why using DAAT is that DAAT can deal with OR operator and won’t create a large hash table that excess the main memory. For DAAT, there are four basic functions called:
    - openList(t) and closeList(lp) open and close the inverted list for term t for reading
    - nextGEQ(lp, k) find the next posting in list lp with docID >= k and return its docID. Return value > MAXDID if none exists.
    - getFreq(lp) get the frequency of the current posting in list lp
The getPos() and getRawPage() were created for generating snippet.
5. After getting all results, we rank them using BM25, it is a basic ranking algorithm. The detail of BM25 show in fig.3
![fig3](/pic/Picture3.png)
fig3
6. At the end, it will output the URL, score of BM25, Document ID, word frequency and snippet at the same time. Looking up Document information table to find URL and frequency from inverted index. 
7. To get snippet, project reserve the position and raw page number for each posting, in other word, when we search posting in inverted index, not only document ID and frequency be received, but the position of that word in raw page and which page contain this word can be obtained as well. Like fetching data from inverted index file, we fetch raw page data by seek the pointer (position) and file name (raw page). It avoids loading all raw page to main memory but may cost a little bit time consuming.
 8. For conjunctive and disjunctive, this project support both searching method. The sequence of handle them is: 
    - check if any document contains all keywords in input, if has, output all of them (maximum number is 10)
    - if one of keyword can not find in inverted index file, it will alternatively be searching each single word. 

9. inverted index cache has been used in this project. Because we didn’t store whole inverted index file into memory, so we need to open and close the file every time when query come in. The good way to reduce time consume when same keyword appear again and again is store the posting to cache, which means next time we get same keyword, the only thing needs to do is check the cache and output it without access raw file.

## MAIN MODULE

1. main.cpp the entrance of project and have timer for each step.
2. pipeLine, contain all process for generating index.
3. sturct.h define all structs used in this project.
4. Class of util, define several helper functions.
5. Class of MyDB, use sqlite as Database, but for default, all tables are stored in plain text file, if needed, can directly store into database.
6. query.cpp, contain all processing for querying.
7. Class of Index, made for getting posting from index files
8. Class of lexiconTable, made for loading lexicon Table from lexicon Table file
9. Class of UrlTable, made for loading Document information from raw file.

## MAIN STRUCTURE

This project will create three main files in the end of first part. It also will generate some temporary file but will delete all of them automatically. The structure of that three files shown on fig.4. Especially, Inverted Index file is written in binary format while other two file is in ascii format. The reason for that is ascii format is more read-friendly and easy to debug.
 ![fig4](/pic/Picture4.png)
 fig4

The structure of result for particular word looks like figure 5:
 ![fig5](/pic/Picture5.png)
 fig5
 
 ## BENCHMARK



### NUMBER OF EACH COMPONENT
For each raw file, the size is around 400MB. Each raw file could produce 6 temporary doc files. It has a threshold that each doc file will contain 10,000,000 postings. That means for each raw file, there will be 60,000,000 posting created.
The size merged file will be almost same with sum of all temporary doc file.
Roughly, each raw file includes 45,000 documents. 
The number of words has recorded is 3241265 (include all language)

### GENERATE INVERTED INDEX
The size of raw file: 421.3MB
Time for creating temporary index file: 420s
Time for merge temporary index file: 58s
Time for generate inverted index and lexicon table: 306s
Total time for first part (generates inverted index): 784s

The size of raw file: 4315.6MB
Time for creating temporary index file: 6083s
Time for merge temporary index file: 187s
Time for generate inverted index and lexicon table: 4602s
Total time for first part (generates inverted index): 10872s

### QUERY PROCESSING
Keyword: new
Running time of generate URL table: 0.608582 s
key parse time: 1.0572e-05 s
Running time of generate lexicon set: 28.9633 s
Get index from file: 0.0952213 s
get snippet and showing result time: 6.22267 s


keyword: new (second time)
Running time of generate URL table: 0s
key parse time: 2.587e-06 s
Running time of generate lexicon set: 0 s
Get index from cache: 4.966e-06 s
Get snippet and showing result time: 5.1107 s

Keyword: big apple
Running time of generate URL table: 0.608453 s
key parse time: 1.108e-05 s
Running time of generate lexicon set: 24.9592 s
Get index from file: Running time of seeking doc: 0.00165667 s
get snippet and showing result time: 11.996 s

keyword: big apple (second time)
Running time of generate URL table: 0s
key parse time: 4.6e-06 s
Running time of generate lexicon set: 0 s
Get index from cache: 5.138e-06 s + 1.254e-06 s
Get snippet and showing result time: 11.7859 s

### PERFORMANCE FOR BYTE COMPRESS
Size of raw file: 421.3MB
The inverted index file without compress: 311.3MB
The inverted after v-byte compress: 150.2MB


## SOME DECISION MADE

1. Using ASCII format file for lexicon table and docInfo table. Only for inverted index file using Binary format. There are two reason for it, the first is that ASCII can be more readable so that it is easy for debug, the second is that inverted index need to be compressed, using binary format is more efficient for uint8_t.
2. Default merge sort is using Unix sort, and also implement basic merge sort using heap which refer to Prof. Suel's code. To using that method, need to uncomment some lines in code.
3. At first, I try to use map to store posting and use nest map to generate inverted index. But this way will slow down the speed due to map using red-black tree and add new item will also need time to sort. So, I choose to use vector to store it.
4. To output inverted index, there are two way to represent docID, one is storing the original docID and also can use offset to indicate the docID. In this project, I use the second one as it can reduce some space for index file.
5. To retrieve the snippet, there is a more efficient way to do it same as retrieve posting in inverted index, but it need to record the absolute offset for word in raw page. With wet file, it is not that easy to traverse file and record point. So, I record the line number for word, and seek the line and read certain line. This way cost more time but more brute force.
6. To implement conjunctive and disjunctive searching, the best way to achieve it is generating subset keyword. For example, the keywords are: ‘good news today’. If it can not find any document contain all of three words, the next phase for searching should be ‘good news’ + ‘news today’ + ‘good today’ and if still no result, then should search word individually. But I only check if any document contains all words, if not, search single word one by one. Should be revised in the future.

## HOW TO RUN

This project CMake to manage the build process and can run with Linux, Unix-like and Windows operation system. 
1. Open this project in Clion IDE and change file path to right path.
2. Set proper path to boost path and sqlite3.
3. Run the project and check output.
4. Note that the program will detect if already a lexicon table had been created before, if not, the program will start to index and once it finish, it will prompt a request for input keyword.

## LIMITATION

1. So far, this project cannot choose reading and writing data in what kind of format (binary or ASCII).
2. The running time is still a little bit slow, it due to some necessary sort function and split function engage.
3. Fetching snippet cost more time.
4. I choose to index all language instead of only English. Although some language has more complex gramma. However, using space as separator is kind of common sense for all of the world. This is a part of reason that index time arise. 





