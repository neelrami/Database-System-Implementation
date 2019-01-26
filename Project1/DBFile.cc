#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"

DBFile::DBFile () 
{
    myFile=new File();
    currentRecord=new Record();
    myPage=new Page();
    pageWritten=true;
    currentPageIndex=0;
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
    if(pageWritten==false)
    {
        off_t tempIndex=myFile->GetLength();
        if(tempIndex==0)
        {
            currentPageIndex=0;
        }
        else
        {
            currentPageIndex=tempIndex-1;
        }
        myFile->AddPage(myPage,tempIndex);
        myPage->EmptyItOut();
    }
    pageWritten=true;
    return myFile->Close();
}

void DBFile::Add (Record &rec) 
{
    if(myPage->Append(&rec)==0)
    {
        off_t tempIndex=myFile->GetLength();
        if(tempIndex==0)
        {
            currentPageIndex=0;
        }
        else
        {
            currentPageIndex=tempIndex-1;
        }
        myFile->AddPage(myPage,tempIndex);
        myPage->EmptyItOut();
        myPage->Append(&rec);
    }
    pageWritten=false;
}

int DBFile::GetNext (Record &fetchme) 
{

}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) 
{
    ComparisonEngine cEngine;
    while(GetNext(fetchme)!=0)
    {
        if (cEngine.Compare(&fetchme,&literal,&cnf))
        {
            return 1;
        }
    }
    return 0;
}