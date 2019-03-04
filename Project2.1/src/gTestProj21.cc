#include "gtest/gtest.h"
#include "DBFile.h"
#include "Schema.h"
#include "Record.h"
#include "TwoWayList.cc"
#include "GenericDBFile.h"
#include "SortedFile.h"

TEST (SFCreateTest, SubTest1)
{
    
    char catalog_path[]="catalog";
    char test[]="region";
    char loadPath[]="region.tbl";
    
    Schema* testSchema=new Schema(catalog_path,test);
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    char* tempFilePath="region.bin";
    DBFile* dbfile=new DBFile();
  
    int result=dbfile->Create (tempFilePath, sorted, &startup);
    //cout<<"LENGTH "<<dbfile->GetMyFile()->GetLength()<<endl;
    EXPECT_EQ(1,result);
    dbfile->Close();
    delete dbfile;
}


TEST (SFOpenTest, SubTest1)
{
    DBFile* dbfile=new DBFile();
  
    char catalog_path[]="catalog";
    char test[]="region";
    char loadPath[]="region.tbl";
    
    Schema* testSchema=new Schema(catalog_path,test);
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    char* tempFilePath="region.bin";
    int tempResult=dbfile->Create(tempFilePath, sorted, &startup);   
    int result=dbfile->Open(tempFilePath);
    //cout<<"LENGTH "<<dbfile->GetMyFile()->GetLength()<<endl;
    EXPECT_EQ(1,result);
    dbfile->Close();
    delete dbfile;
}

TEST (SFMoveFirstTest, SubTest1)
{
    char catalog_path[]="catalog";
    char region[]="region";
    char loadPath[]="region.tbl";
    
    Schema* testSchema=new Schema(catalog_path,region);
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    DBFile* dbfile=new DBFile();
    
    char* tempFilePath="region.bin";
    
    dbfile->Create(tempFilePath, sorted, &startup);
    dbfile->Open(tempFilePath);
    Record tempRecord;
    Record* tempRecord1;
    FILE* tempFile;
    int counter=0;
    char* tempBits1;
    tempFile=fopen(loadPath,"r");
    while(tempRecord.SuckNextRecord(testSchema,tempFile)!=0)
    {
        if(counter==0)
        {
            tempRecord1=(&tempRecord);
            tempBits1=tempRecord1->GetBits();
        }
        dbfile->Add(tempRecord);
        counter++;
    }
    const char aaa= *tempBits1;
    dbfile->Close();

    dbfile->Open(tempFilePath);
    dbfile->MoveFirst();
   
    Page* tempPage=new Page();
    dbfile->GetMyFile()->GetPage(tempPage,0);
  
    TwoWayList<Record>* tempList=tempPage->GetmyRecs();
    tempList->MoveToStart();
  
    Record* tempRecord2=tempList->Current(0);
    char* tempBits2=tempRecord2->GetBits();
  
    const char aaa2= *tempBits2;
  
    int answer=0;
    if(aaa==aaa2)
    {
        answer=1;
    }
    else
    {
        answer=0;
    }
    dbfile->Close();
    EXPECT_EQ(1,answer);
}

TEST (SFAddTest, SubTest1)
{
    char catalog_path[]="catalog";
    char region[]="region";
    char loadPath[]="region.tbl";
    Schema* testSchema=new Schema(catalog_path,region);
    
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    DBFile* dbfile=new DBFile();
    
    char* tempFilePath="region.bin";
    
    
    dbfile->Create(tempFilePath, sorted, &startup);
    dbfile->Open(tempFilePath);
    Record* tempRecord=new Record();
    FILE* tempFile;
    tempFile=fopen(loadPath,"r");
    while(tempRecord->SuckNextRecord(testSchema,tempFile)!=0)
    {
        dbfile->Add(*tempRecord);
    }
    dbfile->Close();
    Page* tempPage=new Page();
    dbfile->Open(tempFilePath);
    dbfile->GetMyFile()->GetPage(tempPage,0);
    EXPECT_EQ(5,tempPage->GetNumRecs());
    delete tempPage;
    delete dbfile;
}

TEST (SFLoadTest, SubTest1)
{
    char catalog_path[]="catalog";
    char region[]="region";
    char loadPath[]="region.tbl";
    
    Schema* testSchema=new Schema(catalog_path,region);
    
    OrderMaker* myOrderMaker1=new OrderMaker(testSchema);
    int runlen=5;
    struct 
    {
        OrderMaker* o; 
        int l;
    } startup = {myOrderMaker1, runlen};
    
    DBFile* dbfile=new DBFile();
    
    char* tempFilePath="region.bin";
    
    
    dbfile->Create(tempFilePath, sorted, &startup);
    dbfile->Open(tempFilePath);    
    dbfile->Load (*testSchema, loadPath);
    dbfile->Close();
    Page* tempPage=new Page();
    delete tempPage;
    delete dbfile;
}

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}