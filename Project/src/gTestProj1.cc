#include "gtest/gtest.h"
#include "DBFile.h"
#include "Schema.h"
#include "Record.h"
#include "TwoWayList.h"

TEST (CreateTest, SubTest1) 
{ 
  
}

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
  dbfile->MoveFirst();
  Record tempRecord;
  FILE* tempFile;
  tempFile=fopen(loadPath,"r");
  while(tempRecord.SuckNextRecord(testSchema,tempFile)!=0)
  {
    dbfile->Add(tempRecord);
  }
  Page* tempPage=new Page();
  tempPage=dbfile->GetCurrentPage();
  TwoWayList<Record> *tempList=tempPage->GetmyRecs();
  tempList->MoveToStart();

}

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}