#ifndef HEAPFILE_H
#define HEAPFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "Defs.h"
#include "GenericDBFile.h"
#include <iostream>
#include <cstdlib> 

class HeapFile: public GenericDBFile
{

	private:
		File* myFile;
		Record* currentRecord;
		Page* myPage;
		bool pageWritten;
		off_t currentPageIndex;

	public:
		HeapFile (); 
		
		~HeapFile();

		int Create (char *fpath, fType file_type, void *startup);
		
		int Open (char *fpath);
		
		int Close ();

		void Load (Schema &myschema, char *loadpath);

		void MoveFirst ();
		
		void Add (Record &addme);
		
		int GetNext (Record &fetchme);
		
		int GetNext (Record &fetchme, CNF &cnf, Record &literal);

		File* GetFile();

		Page* GetPage();
};

#endif