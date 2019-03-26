#include "RelOp.h"

int RUNLEN=10;
struct Param{
	DBFile *dbfile;
	Pipe *outPipe;
	Pipe *inPipe,*inPipe2;
	CNF *cnf;
	Record *literal;
	Function *func;
	Schema *mySchema;
	int *keepMe;
	FILE *file;
	int numAttsInput;
	int numAttsOutput;
	OrderMaker *groupAtts;
};

  
void SelectFile::Run (DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal) {
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->dbfile=&inFile;
	args->outPipe=&outPipe;
	args->cnf=&selOp;
	args->literal=&literal;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void *SelectFile::thread_work(void *args){
	#ifdef F_DEBUG
		std::cout<<"File Thread started"<<std::this_thread::get_id()<<endl;
	#endif
  	struct Param *arg = (struct Param *)(args);                     
  	arg->dbfile->MoveFirst();
  	Record next;
  	int i=0;
  	while(arg->dbfile->GetNext(next,*(arg->cnf),*(arg->literal))){
		arg->outPipe->Insert(&next);
	}
	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"File Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
  	return NULL;
}

void SelectPipe::Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal){
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe=&inPipe;
	args->outPipe=&outPipe;
	args->cnf=&selOp;
	args->literal=&literal;
	//std::cout<<"In the select pipe\n";
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void* SelectPipe::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Pipe Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	struct Param *arg = (struct Param *)(args);     
	ComparisonEngine comp;
	Record rec;
	while(arg->inPipe->Remove(&rec))
		if(comp.Compare(&rec,arg->literal,arg->cnf))
			arg->outPipe->Insert(&rec);
	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"Pipe Thread Closed"<<std::this_thread::get_id()<<endl;
	#endif
	return NULL;
}

void Sum::Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe){
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe=&inPipe;
	args->outPipe=&outPipe;
	args->func=&computeMe;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void *Sum::thread_work(void *args){
	#ifdef F_DEBUG
		std::cout<<"sum Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	struct Param *arg = (struct Param *)(args);   
	Record rec;
	double result=0.0;
	while(arg->inPipe->Remove(&rec)){
		int int_res=0;
		double dbl_res=0;
		arg->func->Apply(rec,int_res,dbl_res);
		result += (int_res + dbl_res);
	}
	Attribute DA = {"sum", Double};
	Schema sum_sch ("sum_sch", 1, &DA);
	stringstream ss;
	ss<<result<<"|";
	Record *rcd=new Record();
	rcd->ComposeRecord(&sum_sch, ss.str().c_str());
	arg->outPipe->Insert(rcd);
	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"sum Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
	return NULL;
}

void DuplicateRemoval::Run (Pipe &inPipe, Pipe &outPipe, Schema &mySchema) {
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe=&inPipe;
	args->outPipe=&outPipe;
	args->mySchema=&mySchema;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
 }

//Duplicate needs to be done
void* DuplicateRemoval::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Duplicate Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	struct Param *arg = (struct Param *)(args);  
 	OrderMaker sortOrder(arg->mySchema);
  	Pipe sorted(100);
  	BigQ biq(*arg->inPipe, sorted, sortOrder, RUNLEN);
  	Record cur, next;
  	ComparisonEngine comp;
  	if(sorted.Remove(&cur)) {
    	while(sorted.Remove(&next))
      		if(comp.Compare(&cur, &next, &sortOrder)) {	
        		arg->outPipe->Insert(&cur);
        		cur.Consume(&next);
      		}
    	arg->outPipe->Insert(&cur);
  	}
  	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"Duplicate Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
}


void Project::Run (Pipe &inPipe, Pipe &outPipe, int *keepMe, int numAttsInput, int numAttsOutput){
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe=&inPipe;
	args->outPipe=&outPipe;
	args->keepMe=keepMe;
	args->numAttsInput=numAttsInput;
	args->numAttsOutput=numAttsOutput;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
 }

 void* Project::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Project Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	//std::cout<<"Inside thread\n";
	struct Param *arg = (struct Param *)(args);  
	Record tmpRcd;
	int i=0;
	while(arg->inPipe->Remove(&tmpRcd)) {
		tmpRcd.Project(arg->keepMe, arg->numAttsOutput, arg->numAttsInput);
		arg->outPipe->Insert(&tmpRcd);
	}
	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"Project Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
	return NULL;
 }

 void GroupBy::Run (Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe) {
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe = &inPipe;
	args->outPipe = &outPipe;
	args->groupAtts = &groupAtts;
	args->func = &computeMe;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void* GroupBy::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Group By Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	struct Param *arg = (struct Param *)(args);  
	Pipe sortPipe(100);
	BigQ *bigQ = new BigQ(*(arg->inPipe), sortPipe, *(arg->groupAtts), RUNLEN);
	int ir;  double dr;
	Type type;
	Attribute attr;
	attr.name = (char *)"sum";
	attr.myType = type;
	Schema *schema = new Schema ((char *)"sum", 1, &attr);
	int numAttsToKeep = arg->groupAtts->numAtts + 1;
	int *attsToKeep = new int[numAttsToKeep];
	attsToKeep[0] = 0; 																																																																																																																																															
	for(int i = 1; i < numAttsToKeep; i++)
		attsToKeep[i] =arg->groupAtts->whichAtts[i-1];

	ComparisonEngine comp;
	Record *tmpRcd = new Record();
	if(sortPipe.Remove(tmpRcd)) {
		bool more = true;
		while(more) {
			more = false;
			type = arg->func->Apply(*tmpRcd, ir, dr);
			double sum=0;
			sum += (ir+dr);
			Record *r = new Record();
			Record *lastRcd = new Record;
			lastRcd->Copy(tmpRcd);
			while(sortPipe.Remove(r)) {
				if(!comp.Compare(lastRcd, r, arg->groupAtts)){
					type = arg->func->Apply(*r, ir, dr);
					sum += (ir+dr);
				} else {
					tmpRcd->Copy(r);
					more = true;
					break;
				}
			}
			ostringstream ss;
			ss <<sum <<"|";
			Record *sumRcd = new Record();
			sumRcd->ComposeRecord(schema, ss.str().c_str());
			Record *tuple = new Record;
			tuple->MergeRecords(sumRcd, lastRcd, 1, arg->groupAtts->numAtts, attsToKeep,  numAttsToKeep, 1);
			arg->outPipe->Insert(tuple);
		}
	}
	arg->outPipe->ShutDown();
	#ifdef F_DEBUG
		std::cout<<"Group By Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
	return NULL;
}
void WriteOut::Run (Pipe &inPipe, FILE *outFile, Schema &mySchema){
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe = &inPipe;
	args->file = outFile;
	args->mySchema = &mySchema;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void* WriteOut::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Work Thread started"<<std::this_thread::get_id()<<endl;
	#endif
		struct Param *arg = (struct Param *)(args); 
		Attribute *atts = arg->mySchema->GetAtts();
		int n = arg->mySchema->GetNumAtts();
		Record rec;
		int cnt=1;
		while(arg->inPipe->Remove(&rec)){
			if(!(arg->file))
			{	
				std::cout<<"Write Out "<<cnt++<<"\n";
				rec.Print(arg->mySchema);
			}
			else{
				//std::fstream fstr(arg->file);
				fprintf(arg->file, "%d: ", cnt++);
				char *bits = rec.bits;
				for (int i = 0; i < n; i++) {
					fprintf(arg->file, "%s",atts[i].name);
					int pointer = ((int *) bits)[i + 1];
					fprintf(arg->file, "[");
					if (atts[i].myType == Int) {
						int *myInt = (int *) &(bits[pointer]);
						fprintf(arg->file, "%d",*myInt);
					} else if (atts[i].myType == Double) {
						double *myDouble = (double *) &(bits[pointer]);
						fprintf(arg->file, "%f", *myDouble);
					} else if (atts[i].myType == String) {
						char *myString = (char *) &(bits[pointer]);
						fprintf(arg->file, "%s", myString);
					}
					fprintf(arg->file, "]");
					if (i != n - 1) {
						fprintf(arg->file, ", ");
					}
				}
				fprintf(arg->file, "\n");
				//fflush(arg->file);
			}
		}
		if(arg->file)
			fclose(arg->file);
	#ifdef F_DEBUG
		std::cout<<"Work Thread closed"<<std::this_thread::get_id()<<endl;
	#endif
}

void Join::Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal){
	Param *args=static_cast<struct Param *>(malloc(sizeof(struct Param)));
	args->inPipe = &inPipeL;
	args->inPipe2 = &inPipeR;
	args->cnf=&selOp;
	args->literal=&literal;
	args->outPipe=&outPipe;
	if(pthread_create(&worker, NULL, thread_work, (void *)args)){
		std::cout<<"Some issue with thread creation here\n";
	}
}

void* Join::thread_work(void* args){
	#ifdef F_DEBUG
		std::cout<<"Join Thread started"<<std::this_thread::get_id()<<endl;
	#endif
	try{
		struct Param *arg = (struct Param *)(args); 
		OrderMaker orderL;
		OrderMaker orderR;
		arg->cnf->GetSortOrders(orderL, orderR);
		if(orderL.numAtts && orderR.numAtts && orderL.numAtts == orderR.numAtts) {
			Pipe *pipeL=new Pipe(100), *pipeR=new Pipe(100);
			BigQ *bigQL = new BigQ(*(arg->inPipe), *pipeL, orderL, RUNLEN);
			BigQ *bigQR = new BigQ(*(arg->inPipe2), *pipeR, orderR, RUNLEN);
			vector<Record *> vecL;
			Record *rcdLeft = new Record();

			vector<Record *> vecR;
			Record *rcdRight = new Record();
			
			ComparisonEngine comp;
			if(pipeL->Remove(rcdLeft) && pipeR->Remove(rcdRight)) {
				int lAttr = ((int *) rcdLeft->bits)[1] / sizeof(int) -1;
				int rAttr = ((int *) rcdRight->bits)[1] / sizeof(int) -1;
				int totAttr = lAttr + rAttr;
				int attrToKeep[totAttr];
				for(int i = 0; i< lAttr; i++)
					attrToKeep[i] = i;
				for(int i = 0; i< rAttr; i++)
					attrToKeep[i+lAttr] = i;
				int joinNum;
				bool leftOK=true, rightOK=true;
				int num  =0;
				while(leftOK && rightOK) {
					leftOK=false; rightOK=false;
					int cmpRst = comp.Compare(rcdLeft, &orderL, rcdRight, &orderR);
					switch(cmpRst) {
						case 0:{
							num ++;
							Record *rcd1 = new Record(); 
							rcd1->Consume(rcdLeft);
							Record *rcd2 = new Record(); 
							rcd2->Consume(rcdRight);
							vecL.push_back(rcd1);
							vecR.push_back(rcd2);
							while(pipeL->Remove(rcdLeft)) {
								if(!comp.Compare(rcdLeft, rcd1, &orderL)) {
									Record *cLMe = new Record();
									cLMe->Consume(rcdLeft);
									vecL.push_back(cLMe);
								} else {
									leftOK = true;
									break;
								}
							}
							while(pipeR->Remove(rcdRight)) {
								if(!comp.Compare(rcdRight, rcd2, &orderR)) {
									Record *cRMe = new Record();
									cRMe->Consume(rcdRight);
									vecR.push_back(cRMe);
								} 
								else {
									rightOK = true;
									break;
								}
							}
							Record *lr = new Record(), *rr=new Record(), *jr = new Record();
							for(auto itL :vecL) {
								lr->Consume(itL);
								for(auto itR: vecR) {
									if(comp.Compare(lr, itR, arg->literal, arg->cnf)) {
										joinNum++;
										rr->Copy(itR);
										jr->MergeRecords(lr, rr, lAttr, rAttr, attrToKeep, lAttr+rAttr, lAttr);
										arg->outPipe->Insert(jr);
									}
								}
							}
							for(auto it:vecL)
								if(!it)
									delete it; 
							vecL.clear();
							for(auto it : vecR)
								if(!it)
									delete it;
							vecR.clear();
							break;
					}
					case 1:
						leftOK = true;
						if(pipeR->Remove(rcdRight))
							rightOK = true;
						break;
					case -1:
						rightOK = true;
						if(pipeL->Remove(rcdLeft))
							leftOK = true;
						break;
					}
				}
			}
		} 
		else 
		{
			int n_pages = 10;
			Record *rcdLeft = new Record;
			Record *rcdRight = new Record;
			Page pageR;
			DBFile dbFileL;
				fType ft = heap;
				dbFileL.Create((char*)"tmpL", ft, NULL);
				dbFileL.MoveFirst();

			int leftAttr, rightAttr, totalAttr, *attrToKeep;

			if(arg->inPipe->Remove(rcdLeft) && arg->inPipe2->Remove(rcdRight)) {
				leftAttr = ((int *) rcdLeft->bits)[1] / sizeof(int) -1;
				rightAttr = ((int *) rcdRight->bits)[1] / sizeof(int) -1;
				totalAttr = leftAttr + rightAttr;
				attrToKeep = new int[totalAttr];
				for(int i = 0; i< leftAttr; i++)
					attrToKeep[i] = i;
				for(int i = 0; i< rightAttr; i++)
					attrToKeep[i+leftAttr] = i;
				do {
					dbFileL.Add(*rcdLeft);
				}while(arg->inPipe->Remove(rcdLeft));
				vector<Record *> vecR;
				ComparisonEngine comp;

				bool rMore = true;
				int joinNum =0;
				while(rMore) {
					Record *first = new Record();
					first->Copy(rcdRight);
					pageR.Append(rcdRight);
					vecR.push_back(first);
					int rPages = 0;
					rMore = false;
					while(arg->inPipe2->Remove(rcdRight)) {
						Record *copyMe = new Record();
						copyMe->Copy(rcdRight);
						if(!pageR.Append(rcdRight)) {
							rPages += 1;
							if(rPages >= n_pages -1) {
								rMore = true;
								break;
							}
							else {
								pageR.EmptyItOut();
								pageR.Append(rcdRight);
								vecR.push_back(copyMe);
							}
						} else {
							vecR.push_back(copyMe);
						}
					}
					dbFileL.MoveFirst();
					int fileRN = 0;
					while(dbFileL.GetNext(*rcdLeft)) {
						for(auto it:vecR) {
							if(comp.Compare(rcdLeft, it, arg->literal, arg->cnf)) {
								joinNum++;
								Record *joinRec = new Record();
								Record *rightRec = new Record();
								rightRec->Copy(it);
								joinRec->MergeRecords(rcdLeft, rightRec, leftAttr, rightAttr, attrToKeep, leftAttr+rightAttr, leftAttr);
								arg->outPipe->Insert(joinRec);
							}
						}
					}
					for(auto it : vecR)
						if(!it)
							delete it;
					vecR.clear();
				}
				dbFileL.Close();
			}
		}
		arg->outPipe->ShutDown();
	}
	catch(std::exception e){
		std::cout<<"Exception in Join Thread\n";
	}
	#ifdef F_DEBUG
		std::cout<<"Join Thread Closed"<<std::this_thread::get_id()<<endl;
	#endif
 }

 void WriteOut::WaitUntilDone(){
	 pthread_join(worker,NULL);
 }