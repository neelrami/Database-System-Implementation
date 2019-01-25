#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

// stub file .. replace it with your own DBFile.cc

DBFile::DBFile () 
{
    myFile=new File();
    currentRecord=new Record();
    myPage=new Page();
    pageWritten=true;
}

DBFile::~DBFile() 
{
    delete myFile;
    delete currentRecord;
    delete myPage;
}

int DBFile::Create (const char *f_path, fType f_type, void *startup) 
{
    char* newFPath=(char*)f_path;
    myFile->Open(0,newFPath);
    return 1;
}

void DBFile::Load (Schema &f_schema, const char *loadpath) 
{
    FILE* tempFile;
    tempFile= fopen(loadpath,"r");
    if(tempFile==NULL)
    {
        cerr<<"Given File Path "<<loadpath<<" is invalid."<<endl;
		exit (1);
    }
    else
    {
        Record tempRecord;
        while(tempRecord.SuckNextRecord(&f_schema,tempFile)!=0)
        {
            Add(tempRecord);
        }
        fclose(tempFile);
    }
}

int DBFile::Open (const char *f_path) 
{ 
    char* newFPath=(char*)f_path;
    myFile->Open(1,newFPath);
    return 1;
}

void DBFile::MoveFirst () 
{

}

int DBFile::Close () 
{

}

void DBFile::Add (Record &rec) 
{


}

int DBFile::GetNext (Record &fetchme) 
{

}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) 
{

}