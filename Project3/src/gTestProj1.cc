#include "gtest/gtest.h"
#include "DBFile.h"
#include "Schema.h"
#include "Record.h"
#include "TwoWayList.cc"

TEST (OpenTest, SubTest1) 
{ 
  DBFile* dbfile=new DBFile();
  const char* tempFilePath="test.txt";
  EXPECT_EQ(1,dbfile->Open(tempFilePath));
  dbfile->Close();
  delete dbfile;
}

TEST (OpenTest, SubTest2) 
{ 
  DBFile* dbfile=new DBFile();
  const char* tempFilePath="test123.txt";
  EXPECT_EQ(0,dbfile->Open(tempFilePath));
  dbfile->Close();
  delete dbfile;
}

TEST (CloseTest, SubTest1) 
{ 
  DBFile* dbfile=new DBFile();
  const char* tempFilePath="test.txt";
  dbfile->Open(tempFilePath);
  EXPECT_EQ(1,dbfile->Close());
  delete dbfile;
}

TEST (CloseTest, SubTest2) 
{ 
  DBFile* dbfile=new DBFile();
  const char* tempFilePath="test123.txt";
  dbfile->Open(tempFilePath);
  EXPECT_EQ(0,dbfile->Close());
  delete dbfile;
}

TEST (LoadTest, SubTest1)
{
  const char* catalog_path="testCatalog";
  const char* addTest="loadTest";
  const char* loadPath="loadTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  DBFile* dbfile=new DBFile();
  dbfile->Create ("loadTest.bin", heap, NULL);
  dbfile->Load (*testSchema, loadPath);
  dbfile->Close();
  Page* tempPage=new Page();
  dbfile->GetFile()->GetPage(tempPage,0);
  EXPECT_EQ(4,tempPage->GetNumRecs());
  delete tempPage;
  delete dbfile;
}

TEST (AddTest, SubTest1)
{
  const char* catalog_path="testCatalog";
  const char* addTest="addTest";
  const char* loadPath="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  DBFile* dbfile=new DBFile();
  dbfile->Create ("addTest.bin", heap, NULL);
  dbfile->Open("addTest.bin");
  Record tempRecord;
  FILE* tempFile;
  tempFile=fopen(loadPath,"r");
  while(tempRecord.SuckNextRecord(testSchema,tempFile)!=0)
  {
    dbfile->Add(tempRecord);
  }
  Page* tempPage=new Page();
  tempPage=dbfile->GetCurrentPage();
  EXPECT_EQ(10,tempPage->GetNumRecs());
  dbfile->Close();
}

TEST (MoveFirstTest, SubTest1)
{
  const char* catalog_path="testCatalog";
  const char* addTest="addTest";
  const char* loadPath="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  DBFile* dbfile=new DBFile();
  dbfile->Create ("addTest.bin", heap, NULL);
  dbfile->Open("addTest.bin");
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
  
  dbfile->Open("addTest.bin");
  dbfile->MoveFirst();

  Page* tempPage=new Page();
  dbfile->GetFile()->GetPage(tempPage,0);
  
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

TEST (GetNext1Test, SubTest1)
{
  const char* catalog_path="testCatalog";
  const char* getnextTest="getnextTest";
  const char* loadPath="getnextTest.tbl";
  Schema* testSchema=new Schema(catalog_path,getnextTest);
  DBFile* dbfile=new DBFile();
  Record tempRecord;
  Record* tempRecord1;
  dbfile->Create ("getnextTest.bin", heap, NULL);
  dbfile->Open("getnextTest.bin");
  FILE* tempFile;
  int counter=0;
  tempFile=fopen(loadPath,"r");
  while(tempRecord.SuckNextRecord(testSchema,tempFile)!=0)
  {
    if(counter==0)
    {
      tempRecord1=new Record(tempRecord);
    }
    dbfile->Add(tempRecord);
    counter++;
  }
  EXPECT_EQ(1,dbfile->GetNext(*tempRecord1));
  dbfile->Close();
}

TEST (GetNext2Test, SubTest1)
{
  const char* catalog_path="testCatalog";
  const char* getnextTest="getnextTest";
  const char* loadPath="getnextTest.tbl";
  Schema* testSchema=new Schema(catalog_path,getnextTest);
  DBFile* dbfile=new DBFile();
  Record tempRecord;
  Record* tempRecord1;
  dbfile->Create ("getnextTest.bin", heap, NULL);
  dbfile->Open("getnextTest.bin");
  FILE* tempFile;
  int counter=0;
  tempFile=fopen(loadPath,"r");
  while(tempRecord.SuckNextRecord(testSchema,tempFile)!=0)
  {
    if(counter==0)
    {
      tempRecord1=new Record(tempRecord);
    }
    dbfile->Add(tempRecord);
    counter++;
  }
  CNF myCNF;
  Record myRecord;
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,myRecord);
  EXPECT_EQ(1,dbfile->GetNext(tempRecord,myCNF,myRecord));
  dbfile->Close();
  
}

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}