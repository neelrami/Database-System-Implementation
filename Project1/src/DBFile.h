#ifndef DBFILE_H
#define DBFILE_H

#include <iostream>
#include <stdlib.h>
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"

typedef enum {heap, sorted, tree} fType;

// stub DBFile header..replace it with your own DBFile.h 

class DBFile 
{
	public:
		File* myFile;
		Record* currentRecord;
		Page* myPage;
		bool pageWritten;
		off_t currentPageIndex;

	public:
		DBFile(); 
		~DBFile();
		
		int Create(const char *fpath, fType file_type, void *startup);
		
		int Open(const char *fpath);
		
		int Close();

		void Load(Schema &myschema, const char *loadpath);

		void MoveFirst();
		
		void Add(Record &addme);
		
		int GetNext(Record &fetchme);
		
		int GetNext(Record &fetchme, CNF &cnf, Record &literal);

		File* GetFile();

		Page* GetCurrentPage();
};
#endif
