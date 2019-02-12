#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"

using namespace std;

struct TPMMS_DS
{
	Pipe* inputPipe;
	Pipe* outputPipe;
	OrderMaker sortorder;
	int runLength;
	int numRuns;
	File* myFile;
};

class BigQ 
{
	private:
		struct TPMMS_DS* myDS;

	public:

		BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
		~BigQ ();
};

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

#endif
