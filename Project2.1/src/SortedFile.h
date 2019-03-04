#ifndef SORTEDFILE_H
#define SORTEDFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "GenericDBFile.h"
#include "Pipe.h"
#include "BigQ.h"
#include <iostream>
#include <cstdlib> 

enum Mode {Read,Write};

typedef struct
{
    OrderMaker* myOrder; 
    int runLength;

    void setRunLength(int tempRunLen)
    {
        runLength=tempRunLen;
    }
} SortInfo;

class SortedFile: public GenericDBFile
{
    private:
        SortInfo* mySFSortInfo;
        Pipe* mySFInputPipe;
        Pipe* mySFOutputPipe;
        File* mySFFile;
        Page* mySFPage;
        Mode myMode;
        BigQ* mySFBigQ;
        int currentPageIndex;
        OrderMaker* mySFOrderMaker;
        bool queryFlag;
        bool sortOrderFlag;

    public:
		SortedFile (); 
		
		~SortedFile();

		int Create (char *fpath, fType file_type, void *startup);
		
		int Open (char *fpath);
		
		int Close ();

		void Load (Schema &myschema, char *loadpath);

		void MoveFirst ();
		
		void Add (Record &addme);
		
		int GetNext (Record &fetchme);
		
		int GetNext (Record &fetchme, CNF &cnf, Record &literal);

        void Merge();

        File* GetFile();

        Page* GetPage();

        int GetNextUtil1(Record &fetchme, CNF &cnf, Record &literal);
        
        int GetNextUtil2(Record &fetchme, CNF &cnf, Record &literal);
        
        int BSearch(Record &fetchme,CNF &cnf,Record &literal);

};

#endif