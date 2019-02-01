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
    int temp=myFile->GetmyFilDes();
    if(temp==0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void DBFile::Load (Schema &f_schema, const char *loadpath) 
{
    FILE* tempFile;
    tempFile=fopen(loadpath,"r");
    if(tempFile==NULL)
    {
        cerr<<"Given File Path "<<loadpath<<" is invalid."<<endl;
		exit(1);
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
    int temp=myFile->Open(1,newFPath);
    if(temp==0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void DBFile::MoveFirst () 
{
    if(myFile->GetLength()==0)
    {
        cerr<<"File is Empty"<<endl;
		exit(1);
    }
    else
    {
        myFile->GetPage(myPage,0);
    }
    
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
    int temp1=myFile->Close();
    int closeValue=myFile->GetmyFilDes();
    if(closeValue==-1)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void DBFile::Add (Record &rec) 
{
    int temp=myPage->Append(&rec);
    if(temp==0)
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
        myFile->AddPage(myPage,currentPageIndex);
        myPage->EmptyItOut();
        myPage->Append(&rec);
    }
    pageWritten=false;
}

int DBFile::GetNext (Record &fetchme) 
{
    while(myPage->GetFirst(&fetchme)!=1)
    {
        off_t fileLength;
        if(myFile->GetLength()==0)
        {
            fileLength=0;
        }
        else
        {
           fileLength=myFile->GetLength()-1;
        }
        if(++currentPageIndex<fileLength)
        {
           myFile->GetPage(myPage,currentPageIndex);
        }
        else
        {
           return 0;
        }
    }
    return 1;
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) 
{
    ComparisonEngine cEngine;
    while(GetNext(fetchme))
    {
        if(cEngine.Compare(&fetchme,&literal,&cnf))
        {
            return 1;
        }
    }
    return 0;
}

File* DBFile::GetFile()
{
    return myFile;
}

Page* DBFile::GetCurrentPage()
{
    return myPage;
}