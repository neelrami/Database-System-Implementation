#include "gtest/gtest.h"
#include "BigQ.h"
#include "Pipe.h"
#include "Schema.h"

TEST (TPMMS_Test1, SubTest) 
{ 
  char catalog_path[]="testCatalog";
  char addTest[]="addTest";
  char loadPath[]="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  int runlen=5;
  OrderMaker myOrderMaker1;
  OrderMaker myOrderMaker2;
  CNF myCNF;
  Record* myRecord=new Record();
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,*myRecord);
	myCNF.GetSortOrders(myOrderMaker1,myOrderMaker2);
  int buffsz=100; 
	Pipe input(buffsz);
	Pipe output(buffsz);
  BigQ* myBigQ=new BigQ(input,output,myOrderMaker1,runlen);
  EXPECT_EQ(5,myBigQ->myDS->runLength);
  delete myBigQ;
}

TEST (TPMMS_Test2, SubTest) 
{ 
  char catalog_path[]="testCatalog";
  char addTest[]="addTest";
  char loadPath[]="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  int runlen=5;
  OrderMaker myOrderMaker1;
  OrderMaker myOrderMaker2;
  CNF myCNF;
  Record* myRecord=new Record();
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,*myRecord);
	myCNF.GetSortOrders(myOrderMaker1,myOrderMaker2);
  int buffsz=100; 
	Pipe input(buffsz);
	Pipe output(buffsz);
  
  BigQ* myBigQ=new BigQ(input,output,myOrderMaker1,runlen);
  EXPECT_TRUE(myBigQ->myDS->inputPipe!=NULL);
  delete myBigQ;
}

TEST (TPMMS_Test3, SubTest) 
{ 
  char catalog_path[]="testCatalog";
  char addTest[]="addTest";
  char loadPath[]="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  int runlen=5;
  OrderMaker myOrderMaker1;
  OrderMaker myOrderMaker2;
  CNF myCNF;
  Record* myRecord=new Record();
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,*myRecord);
	myCNF.GetSortOrders(myOrderMaker1,myOrderMaker2);
  int buffsz=100; 
	Pipe input(buffsz);
	Pipe output(buffsz);
  
  BigQ* myBigQ=new BigQ(input,output,myOrderMaker1,runlen);
  EXPECT_TRUE(myBigQ->myDS->outputPipe!=NULL);
  delete myBigQ;
}

TEST (TPMMS_Test4, SubTest) 
{ 
  char catalog_path[]="testCatalog";
  char addTest[]="addTest";
  char loadPath[]="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  int runlen=5;
  OrderMaker myOrderMaker1;
  OrderMaker myOrderMaker2;
  CNF myCNF;
  Record* myRecord=new Record();
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,*myRecord);
	myCNF.GetSortOrders(myOrderMaker1,myOrderMaker2);
  int buffsz=100; 
	Pipe input(buffsz);
	Pipe output(buffsz);
  
  BigQ* myBigQ=new BigQ(input,output,myOrderMaker1,runlen);
  EXPECT_TRUE(myBigQ->myDS->sortorder!=NULL);
  delete myBigQ;
}

TEST (TPMMS_Test5, SubTest) 
{ 
  char catalog_path[]="testCatalog";
  char addTest[]="addTest";
  char loadPath[]="addTest.tbl";
  Schema* testSchema=new Schema(catalog_path,addTest);
  int runlen=5;
  OrderMaker myOrderMaker1;
  OrderMaker myOrderMaker2;
  CNF myCNF;
  Record* myRecord=new Record();
  struct AndList *final;
  myCNF.GrowFromParseTree(final,testSchema,*myRecord);
	myCNF.GetSortOrders(myOrderMaker1,myOrderMaker2);
  int buffsz=100; 
	Pipe input(buffsz);
	Pipe output(buffsz);
  
  BigQ* myBigQ=new BigQ(input,output,myOrderMaker1,runlen);
  EXPECT_TRUE(myBigQ->myDS->myFile!=NULL);
  delete myBigQ;
}

int main(int argc, char **argv) 
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}