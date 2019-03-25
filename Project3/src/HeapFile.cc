#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "HeapFile.h"
#include "Defs.h"

//Class Constructor
HeapFile::HeapFile () 
{
    myFile=new File();
    currentRecord=new Record();
    myPage=new Page();
    pageWritten=true;
    currentPageIndex=0;
}

//Class Destructor
HeapFile::~HeapFile() 
{
    delete myFile;
    delete currentRecord;
    delete myPage;
}

/*
Return Type: Integer 
1: Success
0: Failure

This function is used to create the file.

*/
int HeapFile::Create (char *f_path, fType f_type, void *startup) 
{
    myFile->Open(0,f_path);
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

/*
Return Type: void

This function bulk loads the HeapFile instance from a text file, appending
new data to it using the SuckNextRecord function from Record.h

*/


void HeapFile::Load (Schema &f_schema, char *loadpath) 
{
    FILE* tempFile;
    const char* newLoadPath=(const char*)loadpath;
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

/*
Return Type: Integer
1: Success
0: Failure

This function is used to open a file that has already been created and also that has been closed.
*/
int HeapFile::Open (char *f_path) 
{ 
    int temp=myFile->Open(1,f_path);
    if(temp==0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

/*
Return Type: void

*/
void HeapFile::MoveFirst () 
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

/*
Return Type: Integer
1: Success
0: Failure

This function simply closes the file.

*/
int HeapFile::Close () 
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

/*
Return Type: void

This function is used to add records to the end of the file.

*/
void HeapFile::Add (Record &rec) 
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

/*
Return Type: Integer
1: Success
0: Failure

This function simply gets the next record from the file and returns it to
the user.

*/
int HeapFile::GetNext (Record &fetchme) 
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

/*
Return Type: Integer
1: Success
0: Failure

This function accepts a selection predicate and returns the next record
in the file which is accepted by the selection predicate.

*/
int HeapFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) 
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

File* HeapFile::GetFile()
{
    return myFile;
}

Page* HeapFile::GetPage()
{
    return myPage;
}