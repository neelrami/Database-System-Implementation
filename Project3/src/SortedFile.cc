#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "SortedFile.h"
#include "Defs.h"
#include "DBFile.h"

//Class Constructor
SortedFile::SortedFile () 
{
    mySFFile=new File();
    mySFPage=new Page();
    mySFInputPipe=new Pipe(100);
    mySFOutputPipe=new Pipe(100);
    mySFSortInfo=NULL;
    mySFBigQ=NULL;
    currentPageIndex=0;
    mySFOrderMaker=NULL;
    queryFlag=false;
    
    //sortOrderFlag variable is used to check if sortOrder exists or not.
    //This variable is used when GetNext is called repeatedly
    sortOrderFlag=true;
}

//Class Destructor
SortedFile::~SortedFile() 
{
    delete mySFFile;
    delete mySFPage;
    delete mySFInputPipe;
    delete mySFOutputPipe;
}

/*


Return Type: Integer 
1: Success
0: Failure

Function Description:
1. This function is used to create the Sorted File. 
2. Here we also create metadata file for each .tbl file
3. Metadata File Description
    A. The first line of metadata file consists of type of DBFile.
        0 - HeapFile
        1 - SortedFile
        2 - B+ Tree (Not implemented till now)
    B. The second line consists of number of runs which is provided by the user.
    C. The third line consists of number of attributes in the ordermaker provided by the user.
    D. Then attribute number for each attribute of ordermaker is written on single line. 
    E. Then attribute type for each attribute of ordermaker is written on single line.
        0 - Int
        1 - Double
        2 - String
*/

int SortedFile::Create(char *f_path, fType f_type, void *startup) 
{
    myMode=Read;
    mySFSortInfo=(SortInfo*)startup;
    int result=mySFFile->Open(0,f_path);
    char metaDataFilePath[1000];
	sprintf(metaDataFilePath,"%s.metadata",f_path);
	FILE* metaData= fopen(metaDataFilePath,"w");
    
    if(metaData==NULL)
    {
        return 0;
    }
    //Insert integer for File Type in metadatafile
    fprintf(metaData, "%d\n", f_type);

    //Insert RunLength in metadata file
    fprintf(metaData, "%d\n", mySFSortInfo->runLength);
    OrderMaker* tempOrderMaker=mySFSortInfo->myOrder;

    /*
    
    tempString is used to store number of attributes, then it stores number of each attribute and also 
    stores type of each attribute as integer.

    */
    string tempString="";
    int myLen=tempOrderMaker->getNumAtts();
    tempString+=to_string(myLen);
    tempString+='\n';
    int* tempArr0=tempOrderMaker->getWhichAtts();
    Type* tempArr=tempOrderMaker->getWhichTypes();
    for(int i=0;i<myLen;i++)
    {
        //cout<<myLen<<endl;
        int myTempInt=(*(tempArr0+i));
        tempString+=to_string(myTempInt);
        tempString+='\n';
    }

    for(int i=0;i<myLen;i++)
    {
        if((*(tempArr+i))==Int)
        {
            tempString+='0';
        }
        else if((*(tempArr+i))==Double)
        {
            tempString+='1';
        }
        else
        {
            tempString+='2';
        }
        tempString+='\n';
    }
    fprintf(metaData, "%s", tempString.c_str());
	fclose(metaData);
    
    return result;
}

/*
Return Type: void

Function Description
1. This function bulk loads the records from the file into the Input Record
2. After all the records have been inserted into Input Pipe, they are removed from the Output Pipe and
   then they are inserted into the page and these pages are added into the Sorted File. 

*/


void SortedFile::Load (Schema &f_schema, char *loadpath) 
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
        if(mySFBigQ==NULL)
        {
            mySFInputPipe=new Pipe(100);
            mySFOutputPipe=new Pipe(100);
            mySFBigQ=new BigQ(*mySFInputPipe,*mySFOutputPipe,*(mySFSortInfo->myOrder),mySFSortInfo->runLength);
        }
        Record tempRecord1;
        while(tempRecord1.SuckNextRecord(&f_schema,tempFile)!=0)
        {
            mySFInputPipe->Insert(&tempRecord1);
        }
        mySFInputPipe->ShutDown();
        fclose(tempFile);
        Page* tempPage=new Page();
        Record* tempRecord2=new Record();
        int tempPageIndex=0;
        while(mySFOutputPipe->Remove(tempRecord2))
        {
            int tempAnswer=tempPage->Append(tempRecord2);
            if(tempAnswer==0)
            {
                mySFFile->AddPage(tempPage,tempPageIndex);
			    tempPage->EmptyItOut();
			    tempPage->Append(tempRecord2);
                tempPageIndex++;
            }
        }
        mySFFile->AddPage(tempPage,tempPageIndex);
    } 
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description

1. This function is used to open a file that has already been created and also that has been closed.
2. First we obtain information about from the metadata file which was created by the Create method 
   and assign those values to sortInfo instance of Sorted File class.
*/
int SortedFile::Open(char *f_path) 
{ 
    int result=mySFFile->Open(1,f_path);
    char metaDataFilePath[1000];
	sprintf(metaDataFilePath,"%s.metadata",f_path);
	FILE* metaData=fopen(metaDataFilePath,"r");
    
    if(metaData==NULL)
    {
        return 0;
    }

    int tempFileType;
    int tempRunLen;
	int tempNumAtts;
    
    //read fileType from metadata file
    fscanf(metaData,"%d",&tempFileType);

    //read run length from metadata file
    fscanf(metaData,"%d",&tempRunLen);

    mySFSortInfo=new SortInfo();
    mySFSortInfo->setRunLength(tempRunLen);

    //read number of attributes from metadata file
    fscanf(metaData,"%d",&tempNumAtts);
    
    mySFSortInfo->myOrder=new OrderMaker();
    mySFSortInfo->myOrder->setNumAtts(tempNumAtts);

    int tempArr1[tempNumAtts];

    //read attribute number of each attribute from metadata file
    for(int i=0;i<tempNumAtts;i++)
    {
        int tempWhichAtt=0;
        fscanf(metaData,"%d",&tempWhichAtt);
        tempArr1[i]=tempWhichAtt;
    }

    mySFSortInfo->myOrder->setWhichAtts(tempArr1,tempNumAtts);

    Type tempArr2[tempNumAtts];

    //read attribute type of each attribute from metadata file
    for(int i=0;i<tempNumAtts;i++)
    {
        int tempWhichType=0;
        fscanf(metaData,"%d",&tempWhichType);
        if(tempWhichType==0)
        {
            tempArr2[i]=Int;
        }
        else if(tempWhichType==1)
        {
            tempArr2[i]=Double;
        }
        else if(tempWhichType==2)
        {
            tempArr2[i]=String;
        }
        
    }

    mySFSortInfo->myOrder->setWhichTypes(tempArr2,tempNumAtts);

	return result;
    
}

/*
Return Type: void

Function Description
1. This function is used to pint to first record og the file.
2. If the mode is read, the first merge operation is performed.
*/
void SortedFile::MoveFirst() 
{
    if(myMode==Write)
    {
        Merge();
    }

    
    if(mySFFile->GetLength()==0)
    {
        cerr<<"File is Empty"<<endl;
		exit(1);
    }
    else
    {
        mySFFile->GetPage(mySFPage,0);
    }
    queryFlag=false;
    sortOrderFlag=true;
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description
1. This function simply closes the file.
2. Before closing the file, we call the merge procedure to merge records of InputPipe and records which are already present in the Sorted File.
*/
int SortedFile::Close() 
{
    if(myMode==Write)
    {
        Merge();
    }
    return mySFFile->Close();
}

/*
Return Type: void

Function Description
1. This function is used to add records to the input pipe.
2. If the sorted file is in read mode, we will re-instantiate its BigQ instance and change its mode to write and insert the record in the pipe.

*/
void SortedFile::Add(Record &rec) 
{
    if(myMode==Read)
    {
        delete mySFInputPipe;
        delete mySFOutputPipe;
        delete mySFBigQ;
        mySFInputPipe=new Pipe(100);
        mySFOutputPipe=new Pipe(100);
        mySFBigQ=new BigQ(*mySFInputPipe,*mySFOutputPipe,*(mySFSortInfo->myOrder),mySFSortInfo->runLength);
        myMode=Write;
    }
    mySFInputPipe->Insert(&rec);
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description

1. This function simply gets the next record from the file and returns a 1 if it is found 
   or 0 if it is not found.

*/
int SortedFile::GetNext(Record &fetchme) 
{
    if(myMode==Write)
    {
        Merge();
    }

    while(mySFPage->GetFirst(&fetchme)!=1)
    {
        off_t fileLength;
        if(mySFFile->GetLength()==0)
        {       
            fileLength=0;
        }
        else
        {
            fileLength=mySFFile->GetLength()-1;
        }
        if(++currentPageIndex<fileLength)
        {
            mySFFile->GetPage(mySFPage,currentPageIndex);
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

Function Description:

1. This function accepts a selection predicate and returns 1 if the next record
   in the file is accepted by the selection predicate or 0.
2. sortOrderFlag is used in case when the function is called repeadtedly. So when the this called for the 2nd time 
   immediately, we don't do binary search again and 

*/
int SortedFile::GetNext(Record &fetchme, CNF &cnf, Record &literal) 
{
    if(myMode==Write)
    {
        cout<<"HELLO"<<endl;
        Merge();
        queryFlag=false;
		sortOrderFlag=true;
    }
    cout<<"ddd"<<endl;
    if(sortOrderFlag==true)
	{
        if(queryFlag==false)
		{
            queryFlag=true;
			int tempVar;
            if(mySFOrderMaker==NULL)
            {
                mySFOrderMaker=new OrderMaker();
            }
			
			if(cnf.GetSortOrders1(*mySFOrderMaker,*(mySFSortInfo->myOrder))>0)
			{  
                tempVar=MyBinarySearch(fetchme,cnf,literal);
                ComparisonEngine engine;
				if(tempVar)
				{
					if (engine.Compare(&fetchme, &literal, &cnf))  
                    {
                        return 1;
                    }        
					else
					{
						GetNextUtil2(fetchme, cnf, literal);
					}
				}
				else
                {
                    return 0;
                }
						
			}
			else
			{
				sortOrderFlag=false;
				return GetNextUtil1(fetchme,cnf,literal);
			}
		}
		else
        {
            GetNextUtil2(fetchme,cnf,literal);
        }
				
	}
	else
    {
        return GetNextUtil1(fetchme,cnf,literal);
    }
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description
1. This function merges records from input pipe with the records from the sorted file.
2. We also first change its mode to read and shutdown the pipe.
3. Then, we merge records from the pipe and sorted file.
*/

void SortedFile::Merge()
{
    cout<<"bbb"<<endl;
    myMode=Read;
    mySFInputPipe->ShutDown();
    Record* temp1=new Record();
    Record* temp2=new Record();
    bool flag=true;

    Page* tempPage=new Page();
    int temp3=GetNext(*temp1);
    int temp4=mySFOutputPipe->Remove(temp2);
    ComparisonEngine ce1;
    int tempPageIndex=0;

    /*
    1. Extract one record from the Output Pipe and other record from Sorted File. 
    2. If the record from Sorted File is smaller, then insert it into Page and extract record from Sorted File for comparison.
    3. Else if the record from Output Pipe is smaller, the insert it into Page and remove next record from Pipe for comparison.
    4. When all records from Sorted File have been extracted and inserted, then simply remove records from Pipe and keep on inserting
       them until the Pipe becomes exhausted.
    5. Else when all the records from Output Pipe have been removed and inserted, then simply remove records from Sorted File and keep on 
       inserting them until the Sorted File becomes exhausted of records.
    */

    while(flag==true)
    {
        if(temp3==1 && temp4==1) 
	    {
            ce1.Compare(temp1,temp2,mySFSortInfo->myOrder);
            if(ce1.Compare(temp1,temp2,mySFSortInfo->myOrder)>=1) 
			{  
                int tempAnswer=tempPage->Append(temp1);
                if(tempAnswer==0)
                {
                    mySFFile->AddPage(tempPage,tempPageIndex);
			        tempPage->EmptyItOut();
			        tempPage->Append(temp1);
                    tempPageIndex++;
                }
				temp3=GetNext(*temp1);
			}
			else
			{
                int tempAnswer=tempPage->Append(temp2);
                if(tempAnswer==0)
                {
                    mySFFile->AddPage(tempPage,tempPageIndex);
			        tempPage->EmptyItOut();
			        tempPage->Append(temp2);
                    tempPageIndex++;
                }
				temp4=mySFOutputPipe->Remove(temp2);
			}
		}
		else
        {
            flag=false;
        }
    }
    while(temp3==1)
    {
        int tempAnswer=tempPage->Append(temp1);
        if(tempAnswer==0)
        {
            mySFFile->AddPage(tempPage,tempPageIndex);
			tempPage->EmptyItOut();
			tempPage->Append(temp1);
            tempPageIndex++;
        }
        temp3=GetNext(*temp1);
    }
    while(temp4==1)
    {
        int tempAnswer=tempPage->Append(temp2);
        if(tempAnswer==0)
        {
            mySFFile->AddPage(tempPage,tempPageIndex);
			tempPage->EmptyItOut();
			tempPage->Append(temp2);
            tempPageIndex++;
        }
        temp4=mySFOutputPipe->Remove(temp2);
	}
    
    mySFFile->AddPage(tempPage,tempPageIndex);
    MoveFirst();
}


/*
Return Type: Integer
1: Success
0: Failure

Function Description

1. This function performs binary search on the records in the file.

*/
int SortedFile::MyBinarySearch(Record& fetchme,CNF &cnf,Record &literal)
{
	off_t low,mid=currentPageIndex;
    off_t high=mySFFile->GetLength()-2;
    Page* tempPage=new Page();
	Record* tempRecord=new Record();
	bool flag=false; 
	ComparisonEngine ce1;
    while(low<high)
	{
        mid=low+(high-low)/2;
        mySFFile->GetPage(tempPage,mid);
        if(tempPage->GetFirst(&fetchme)==1)
		{      
			if(ce1.Compare(&literal,mySFOrderMaker,&fetchme,mySFSortInfo->myOrder)<=0)
            {
                high=mid;
            }
			else
			{
				low=mid;
				if(low==high-1)
				{
					mySFFile->GetPage(tempPage,high);
					int tempFetch=tempPage->GetFirst(&fetchme);
					if(ce1.Compare(&literal,mySFOrderMaker,&fetchme,mySFSortInfo->myOrder)>0)
                    {
                        mid=high;
                    }
					break;
				}
			}
		}
		else
        {
            break;
        }
	}
		
    if(mid==currentPageIndex)
	{  
		while(mySFPage->GetFirst(&fetchme)==1)
		{
			if(ce1.Compare(&literal, mySFOrderMaker, &fetchme,mySFSortInfo->myOrder) == 0 )
			{
				flag=true;
				break;
			}
		}
	}
	else
	{   
		mySFFile->GetPage(mySFPage,mid);
		while(mySFPage->GetFirst(&fetchme)==1)
		{
			if(ce1.Compare(&literal, mySFOrderMaker, &fetchme,mySFSortInfo->myOrder) == 0 )
			{
				flag=true;
				currentPageIndex=mid;
				break;
			}
		}
	}
    if(flag==false && mid < mySFFile->GetLength()-2)
    {
		mySFFile->GetPage(mySFPage,mid+1);
		if(mySFPage->GetFirst(&fetchme)==1 && ce1.Compare(&literal, mySFOrderMaker, &fetchme,mySFSortInfo->myOrder) == 0)
		{
			flag=true;
			currentPageIndex=mid+1;
		}
	}
	if(flag==true)
    {
        return 1;
    }
	else
    {
        return 0;
    }
}

/*
Return Type: Pointer to File Object 
(File*)

Function Description
1. A Getter method which returns a pointer of a File Object.
*/

File* SortedFile::GetFile()
{
    return mySFFile;
}


/*
Return Type: Pointer to Page Object 
(Page*)

Function Description
1. A Getter method which returns a pointer of a Page Object.
*/

Page* SortedFile::GetPage()
{
    return mySFPage;
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description
1. This function sequentially scans the records and compares them using Comaprison Engine.

*/
int SortedFile::GetNextUtil1(Record &fetchme, CNF &cnf, Record &literal) 
{
    ComparisonEngine ce1;
    while(true)
    {
		if(mySFPage->GetFirst(&fetchme)==1)          
		{
			if(ce1.Compare(&fetchme, &literal, &cnf))
            {
                return 1;
            }    
					
		}
		else        
		{
            off_t fileLength;
            if(mySFFile->GetLength()==0)
            {       
                fileLength=0;
            }
            else
            {
                fileLength=mySFFile->GetLength()-1;
            }
			if(++currentPageIndex<fileLength)
            {
                mySFFile->GetPage(mySFPage,currentPageIndex);
            }
            else
            {
                return 0;
            }         
					
		}
    }
}

/*
Return Type: Integer
1: Success
0: Failure

Function Description
1. This function sequentially scans the records and compares them using Comaprison Engine.

*/

int SortedFile::GetNextUtil2(Record &fetchme, CNF &cnf, Record &literal)
{
	ComparisonEngine ce1;
	while(true)
	{
		if(mySFPage->GetFirst(&fetchme)==1)
		{         
			if(ce1.Compare(&literal,mySFOrderMaker,&fetchme,mySFSortInfo->myOrder)==0)
			{
				if(ce1.Compare(&fetchme,&literal,&cnf))
                {
                    return 1;
                }    
			}
			else
            {
                return 0;
            }
		}
		else
		{   
            off_t fileLength;
            if(mySFFile->GetLength()==0)
            {       
                fileLength=0;
            }
            else
            {
                fileLength=mySFFile->GetLength()-1;
            }
			if(++currentPageIndex<fileLength)
            {
                mySFFile->GetPage(mySFPage,currentPageIndex);
            }
            else
            {
                return 0;
            }                 
		}
	}
}

