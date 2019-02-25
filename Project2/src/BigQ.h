#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include "Schema.h"

using namespace std;

/*
Data Structure for TPMMS Algo
*/
struct TPMMS_DS
{
	Pipe* inputPipe;
	Pipe* outputPipe;
	OrderMaker* sortorder;
	int runLength;
	int numRuns;
	File* myFile;

	TPMMS_DS()
	{
		
	}
};

/*
A wrapper around Record class.
*/
struct Record1
{
	Record* record;
	int runNumber;
};

class BigQ 
{
	public:
		struct TPMMS_DS* myDS;

		BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
		~BigQ ();
};

/*
This struct is used by vector(which belongs to Phase 1 of TPMMS Algo) in BigQ.cc 
to sort a vector of Record Class using ComparisonEngine class
*/
struct RecordSorter
{
	ComparisonEngine cEngine;
	OrderMaker* sortorder;

	RecordSorter(OrderMaker* sortorder)
	{
		this->sortorder=sortorder;
	}

	bool operator()(Record* myRecord1, Record* myRecord2) 
	{
		int answer=cEngine.Compare(myRecord1,myRecord2,this->sortorder);
		
		if(answer<0)
		{
			return true;
		}
		else
		{
			return false;
		}
		
	}
};

/*
This struct is used by priority queue(which belongs to Phase 2 of TPMMS Algo) in BigQ.cc 
to sort a list of Record1 struct using ComparisonEngine class
*/
struct RecordComparison
{
	ComparisonEngine cEngine1;
	OrderMaker* sortorder1;

	RecordComparison(OrderMaker* sortorder1)
	{
		this->sortorder1=sortorder1;
	}
	
	bool operator()(Record1* temp1, Record1* temp2)
	{
		int answer=cEngine1.Compare(temp1->record,temp2->record,this->sortorder1);
		if(answer>=0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
};

#endif
