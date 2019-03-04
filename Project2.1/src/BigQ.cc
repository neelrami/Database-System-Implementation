#include "BigQ.h"
#include "ComparisonEngine.h"
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

void* TPMMS_Algo(void* args);

/*
BigQ Constructor
*/

BigQ::BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) 
{
	myDS=new TPMMS_DS();
	myDS->inputPipe = &in;

	myDS->outputPipe = &out;
	myDS->runLength=runlen;
	myDS->sortorder= &sortorder;
	myDS->myFile=new File();
	myDS->numRuns=0;

	pthread_t TPMMSThread;
	
	pthread_create(&TPMMSThread,NULL,&TPMMS_Algo,(void*)myDS);

	
}

/*
BigQ Destructor
*/

BigQ::~BigQ () 
{

}

/*
The below function implements the TPMMS (Two Pass Multiway Merge Sort) Algorithm.
*/

void* TPMMS_Algo(void* args)
{
	/*
	TPMMS Phase 1
	*/

	struct TPMMS_DS* tempDS=new TPMMS_DS();
	tempDS=(TPMMS_DS* )args;

	//numPages is used to keep track of number of pages
	int numPages=0;

	//Temporary variables for storing Records and Pages
	Record* tempRecord1=new Record();
	Page* tempPage1=new Page();
	Page* tempPage2=new Page();
	
	//This vector is used to store and sort vectors
	vector<Record*> v1;

	//This vector stores the length of each run
	vector<int> v2;
	
	char myFileName[]="tpmmsFile";
	tempDS->myFile->Open(0,myFileName);

	/* 1. Keep removing records from Input Pipe. 
	   2. Insert the records in a temporary page.
	   3. If the page is full, 2 cases arise. 
	   		CASE 1: When the number of pages equals the runlength, sort the vector of records and add the pages to the file.
			CASE 2: When the number of pages is not equal to the runlength,empty the page and insert the record in the page.
	   4. If the page is not full, insert the record into the vector and also insert it into the page.
	*/
	
	//Keep on extracting records from the Input Pipe
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
				int tempRunLength=0;
				//Sorting a vector of records using custom struct
				sort(v1.begin(),v1.end(),RecordSorter(tempDS->sortorder));
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
						tempRunLength++;
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
					tempRunLength++;
					tempPage2->EmptyItOut();
					
				}	

				v1.clear();
                numPages=0;
                tempDS->numRuns++;
                v2.push_back(tempRunLength);
			}
			v1.push_back(tempRecord2);
            tempPage1->EmptyItOut();
            tempPage1->Append(tempRecord1);
		}
	}

	tempPage1->EmptyItOut();
	tempPage2->EmptyItOut();
	
	/*
	The below IF case handles the case when the page into which records were written is not added into the file.
	So, first we will sort the vector of records and then add the page to the file. 
	*/
	
	if(v1.size()!=0)
	{
		sort(v1.begin(),v1.end(),RecordSorter(tempDS->sortorder));
		vector<Record*>::iterator it2;
		int tempRunLength=0;
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
				tempRunLength++;
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
			tempRunLength++;
			tempPage2->EmptyItOut();
		}
		tempDS->numRuns++;
		v2.push_back(tempRunLength);
	
	}
	
		
	tempDS->myFile->Close();


	/*
	Deleting the Pages and Records after doing Valgrind
	*/
	delete tempRecord1;
	delete tempPage1;
	delete tempPage2;
	
	//cout<<"TPMMS PHASE 1 COMPLETE"<<endl;
	
	/*
	TPMMS Phase 2
	*/

	tempDS->myFile->Open(1,myFileName);
	
	//Priority queue having a custom comparator
	priority_queue<Record1*,vector<Record1*>, RecordComparison> myQueue(tempDS->sortorder);

	//This array initially holds first page of each run
	Page* startPages[tempDS->numRuns]; 

	//If Page objects are not created using new operator, segmentation fault occurs
	for(int i=0;i<tempDS->numRuns;i++)
	{
		startPages[i]=new Page();
	}

	//This array indicates the page which is currently used to extract record for each run
	int currentPage[tempDS->numRuns];

	vector<int> startPageIndex;
	

	startPageIndex.push_back(0);
	for(int i=1;i<tempDS->numRuns;i++)
	{
		startPageIndex.push_back(startPageIndex[i-1]+v2[i-1]);
	}

	/*
	for(int i=0;i<tempDS->numRuns;i++)
	{
		cout<<startPageIndex1[i]<<" ";
	}
	*/

	//Get first Page of each run into 
	for(int i=0;i<tempDS->numRuns;i++)
	{
        tempDS->myFile->GetPage(startPages[i],startPageIndex[i]);
		currentPage[i]=0;
	}

	//Push first record of each run into priority queue
	for(int i=0;i<tempDS->numRuns;i++)
	{
	    Record1* tempR1=new Record1();
		tempR1->record=new Record();
		tempR1->runNumber=i;
		startPages[i]->GetFirst(tempR1->record);
		myQueue.push(tempR1);
	}
	
	//Keep on poping records until priority queue is empty

	/*
	1. Get the top record of the priority queue.
	2. Extract the run number of the extracted record.
	3. Now push the record from the page which belongs to the run number which we found above.
	   If that page is exhausted then get next page for that run but if the run contains no pages, then do nothing.
	*/
	while(myQueue.empty()==false)
	{
		Record1* tempR2=myQueue.top();
		int runNum=tempR2->runNumber;
		tempDS->outputPipe->Insert(tempR2->record);
		myQueue.pop();
		Record1* tempR3=new Record1();
		tempR3->record=new Record();
		if(startPages[runNum]->GetFirst(tempR3->record))
		{
			tempR3->runNumber=runNum;
			myQueue.push(tempR3);
		}	
		else
		{
			currentPage[runNum]++;
			if(currentPage[runNum]>=v2[runNum])
			{

			}	
			else
			{
				tempDS->myFile->GetPage(startPages[runNum],currentPage[runNum]+startPageIndex[runNum]);
				startPages[runNum]->GetFirst(tempR3->record);
				tempR3->runNumber=runNum;
				myQueue.push(tempR3);
			}
		}
	}
    	
	tempDS->myFile->Close();
    tempDS->outputPipe->ShutDown();
}