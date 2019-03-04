#ifndef GENERIC_DBFILE_H
#define GENERIC_DBFILE_H

#include "Schema.h"
#include "Record.h"
#include "File.h"

enum fType {heap, sorted, tree};

class GenericDBFile
{

    public:
    
	    GenericDBFile();
	
	    virtual int Create(char *fpath, fType file_type, void *startup) = 0;
	    virtual int Open(char *fpath) = 0;
	    virtual int Close() = 0;

	    virtual void Load(Schema &myschema, char *loadpath) = 0;

	    virtual void MoveFirst() = 0;
	    virtual void Add(Record &addme) = 0;
	    virtual int GetNext(Record &fetchme) = 0;
	    virtual int GetNext(Record &fetchme, CNF &cnf, Record &literal) = 0;
	
		virtual File* GetFile() = 0;
		virtual Page* GetPage() = 0;

	    virtual ~GenericDBFile();

};
#endif