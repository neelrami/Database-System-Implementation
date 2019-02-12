#include "BigQ.h"
#include <vector>
#include <algorithm>
#include <queue>

void* TPMMS_Algo(void* args);

BigQ::BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) 
{
	myDS->inputPipe = &in;
	myDS->outputPipe = &out;
	myDS->runLength=runlen;
	myDS->sortorder=sortorder;
	myDS->myFile=new File();
	myDS->numRuns=0;
	// read data from in pipe sort them into runlen pages

    // construct priority queue over sorted runs and dump sorted data 
 	// into the out pipe

    // finally shut down the out pipe

	pthread_t TPMMSThread;

	pthread_create(&TPMMSThread,NULL,&TPMMS_Algo,(void*)myDS);

	out.ShutDown ();
}

BigQ::~BigQ () 
{

}

void* TPMMS_Algo(void* args)
{
	struct TPMMS_DS* tempDS;
	tempDS=(TPMMS_DS* )args;

	int numPages=0;
	Record* tempRecord1=new Record();
	Page* tempPage1=new Page();
	Page* tempPage2=new Page();
	vector<Record*> v1;
	while(tempDS->inputPipe->Remove(tempRecord1)!=0)
	{
		Record* tempRecord2=new Record();
		tempRecord2->Copy(tempRecord1);
		if(tempPage1->Append(tempRecord1))
		{
			v1.push_back(tempRecord2);
		}
		else
		{
			numPages=numPages+1;
			if(numPages==tempDS->runLength)
			{
				tempDS->numRuns++;
				sort(v1.begin(),v1.end(),RecordSorter(&(tempDS->sortorder)));
				vector<Record*>::iterator it1;
				for(it1=v1.begin();it1!=v1.end();it1++)
				{
					Record* myRecord=(*it1);
					if(tempPage2->Append(myRecord))
					{

					}
					else
					{
						int fLength=tempDS->myFile->GetLength();
						if(fLength==0)
						{
							tempDS->myFile->AddPage(tempPage2,0);
						}
						else
						{
							tempDS->myFile->AddPage(tempPage2,fLength-1);
						}
						tempPage2->EmptyItOut();
						tempPage2->Append(myRecord);
					}
				}

				if(tempPage2->GetNumRecs()!=0)
				{
					int fLength=tempDS->myFile->GetLength();
					if(fLength==0)
					{
						tempDS->myFile->AddPage(tempPage2,0);
					}
					else
					{
						tempDS->myFile->AddPage(tempPage2,fLength-1);
					}
					tempPage2->EmptyItOut();	
				}

				v1.clear();
			}
			tempPage1->EmptyItOut();
			tempPage1->Append(tempRecord1);
			v1.push_back(tempRecord2);
		}

		if(tempPage1->GetNumRecs()!=0)
		{
			vector<Record*>::iterator it2;
			for(it2=v1.begin();it2!=v1.end();it2++)
			{
				Record* myRecord=(*it2);
				if(tempPage2->Append(myRecord))
				{

				}
				else
				{
					int fLength=tempDS->myFile->GetLength();
					if(fLength==0)
					{
						tempDS->myFile->AddPage(tempPage2,0);
					}
					else
					{
						tempDS->myFile->AddPage(tempPage2,fLength-1);
					}
					tempPage2->EmptyItOut();
					tempPage2->Append(myRecord);
				}
			}
		}


	}
}

