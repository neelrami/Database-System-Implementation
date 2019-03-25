#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include "GenericDBFile.h"
#include "HeapFile.h"
#include "SortedFile.h"

using namespace std;

DBFile::DBFile() 
{
	myGDBFile=NULL;
}

int DBFile::Create(char *f_path, fType f_type, void *startup) 
{
    if(f_type==heap)
    {
		myGDBFile=new HeapFile();
	}

	else if(f_type==sorted)
	{
		myGDBFile=new SortedFile();	
	}

	else if(f_type==tree)
	{

	}

	if(myGDBFile!=NULL)
	{
		return myGDBFile->Create(f_path,f_type,startup);		
	}	
}

void DBFile::Load(Schema &f_schema, char *loadpath) 
{
	myGDBFile->Load(f_schema,loadpath);
}

int DBFile::Open(char *f_path) 
{
	char metaDataFilePath[1000];
	sprintf(metaDataFilePath,"%s.metadata",f_path);
	FILE* metaData= fopen(metaDataFilePath,"r");;
	fType type;
	fscanf(metaData,"%d",&type);
	fclose(metaData);

	if(type==0)
	{
		if(myGDBFile==NULL)
		{
			myGDBFile=new HeapFile();
		}
	}

	else if(type==1)
	{
		if(myGDBFile==NULL)
		{
			myGDBFile=new SortedFile();
		}
		 
	}

	else if(type==2)
	{

	}

	return myGDBFile->Open(f_path);
}

void DBFile::MoveFirst()
{
	myGDBFile->MoveFirst();
}

int DBFile::Close() 
{
	return myGDBFile->Close();
}

void DBFile::Add(Record &rec) 
{
	myGDBFile->Add(rec);
}

int DBFile::GetNext(Record &fetchme) 
{
	return myGDBFile->GetNext(fetchme);
}

int DBFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) 
{
	return myGDBFile->GetNext(fetchme,cnf,literal);
}

File* DBFile::GetMyFile()
{
	return myGDBFile->GetFile();
}

Page* DBFile::GetMyPage()
{
	return myGDBFile->GetPage();
}