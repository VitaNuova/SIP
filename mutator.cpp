#include <stdio.h>
#include <iostream>
#include <string>
#include "mutator.h"

using namespace Dyninst;
using namespace std;

BPatch bpatch;

int main(int argc, const char* argv[]) {
   if(argc < 3) {
      fprintf(stderr, "Usage: mutator <connectivity_level> <path_to_binary>.\n");
      return 0;
   }
   
   int connectivity_level = *argv[1] - '0';
   if(connectivity_level < 1 || connectivity_level > 10) {
      fprintf(stderr, "Connectivity level should be between 1 and 10.\n");
   }
   
   const char* path = argv[2];
   BPatch_binaryEdit* appbin = bpatch.openBinary(path);
   if(appbin == NULL) {
      fprintf(stderr, "Something went wrong with opening the binary.\n");
      return -1;
   }
 
   BPatch_image* appImage = (*appbin).getImage();
   precompute_hashes(appImage);
   
   vector<BPatch_function*>* functions = (*appImage).getProcedures();
   int counter =0;     
   for(BPatch_function* f: *functions) {
      if((*f).getName() == "main") {
         cout << "Function name: " << (*f).getName() << endl;
         set<BPatch_basicBlock*> blocks = get_basic_blocks(f);
         for(BPatch_basicBlock* bb: blocks) {
	    if(counter == 0) {
               vector<BPatch_point*>* points;
               vector<BPatch_function*> functions;
               (*appImage).findFunction("main", functions);
               points = (*functions[0]).findPoint(BPatch_locEntry);    
               //unsigned long start = 0x400666;
	       //unsigned long end = 0x4006c7 call to time;
	       BPatch_variableExpr* start = (*appbin).malloc(*((*appImage).findType("unsigned long")));
	       BPatch_variableExpr* end = (*appbin).malloc(*((*appImage).findType("unsigned long")));
	       BPatch_variableExpr* hash_sum = (*appbin).malloc(*((*appImage).findType("unsigned long")));

	       //BPatch_constExpr* startConst = new BPatch_constExpr((*bb).getStartAddress());
               //BPatch_constExpr* endConst = new BPatch_constExpr((*bb).getEndAddress());
	       BPatch_constExpr* startConst = new BPatch_constExpr(0x400666);
               BPatch_constExpr* endConst = new BPatch_constExpr(0x4006c7);
	       BPatch_constExpr* hashConst = new BPatch_constExpr(0);
	      
	       BPatch_arithExpr* assignStart = new BPatch_arithExpr(BPatch_assign, *start, *startConst);
	       BPatch_arithExpr* assignEnd = new BPatch_arithExpr(BPatch_assign, *end, *endConst);
	       BPatch_arithExpr* assignInitialHash = new BPatch_arithExpr(BPatch_assign, *hash_sum, *hashConst);
	      
               BPatch_boolExpr* condition = new BPatch_boolExpr(BPatch_ne, *start, *end);
	       //BPatch_constExpr* valueAtAddr = new BPatch_constExpr(*(unsigned char*)((*bb).getStartAddress()));
	       BPatch_constExpr* valueAtAddr = new BPatch_constExpr(*((unsigned char*)(0x400666)));  BPatch_arithExpr* sumHash = new BPatch_arithExpr(BPatch_plus, *hash_sum, *valueAtAddr);
	       BPatch_arithExpr* assignHash = new BPatch_arithExpr(BPatch_assign, *hash_sum, *sumHash);
	       BPatch_constExpr* oneConst = new BPatch_constExpr(1);
               BPatch_arithExpr* incrementStart = new BPatch_arithExpr(BPatch_plus, *start, *oneConst);
	       BPatch_arithExpr* assignNewStart = new BPatch_arithExpr(BPatch_assign, *start, *incrementStart);
               vector<BPatch_snippet*> whileItems;
	       whileItems.push_back(sumHash);
               whileItems.push_back(assignHash);
               whileItems.push_back(incrementStart);
               whileItems.push_back(assignNewStart);
               BPatch_sequence* whileSequence = new BPatch_sequence(whileItems);
	       BPatch_whileExpr* finalWhile = new BPatch_whileExpr(*condition, *whileSequence);
               
               BPatch_constExpr* expectedHash = new BPatch_constExpr(0x21d4);
	       BPatch_boolExpr* ifCondition = new BPatch_boolExpr(BPatch_ne, *hash_sum, *expectedHash);
               /*vector<BPatch_snippet*> argToExit;
               argToExit.push_back(oneConst);               
               vector<BPatch_function*> exitFuncs;
               (*appImage).findFunction("exit", exitFuncs);
               if(exitFuncs.size() == 0) {
                  fprintf(stderr, "Could not find exit");
		    
               }
               BPatch_funcCallExpr* exitCall = new BPatch_funcCallExpr(*(exitFuncs[0]), argToExit);*/
               BPatch_constExpr* argToPrintf = new BPatch_constExpr("Fail\n");
               vector<BPatch_snippet*> argsToPrintf;
               argsToPrintf.push_back(argToPrintf);
               vector<BPatch_function*> printfFuncs;
               (*appImage).findFunction("printf", printfFuncs, true, true, true);
               if(printfFuncs.size() == 0) {
                  fprintf(stderr, "Could not find printf");
		  exit(0);    
               }
               BPatch_funcCallExpr* printfCall = new BPatch_funcCallExpr(*(printfFuncs[0]), argsToPrintf);
               vector<BPatch_snippet*> ifBody;
               //ifBody.push_back(exitCall);
               ifBody.push_back(printfCall);
               BPatch_sequence* ifBodySeq = new BPatch_sequence(ifBody);
               BPatch_ifExpr* ifExpr = new BPatch_ifExpr(*ifCondition, *ifBodySeq); 
               
               vector<BPatch_snippet*> wholeSnippet;
               wholeSnippet.push_back(assignStart);
               wholeSnippet.push_back(assignEnd);
               wholeSnippet.push_back(assignInitialHash);
               wholeSnippet.push_back(finalWhile);
               wholeSnippet.push_back(ifExpr);
               
	       BPatch_sequence* wholeSeq = new BPatch_sequence(wholeSnippet);
               
               (*appbin).insertSnippet(*wholeSeq, *points);
	       /*(*appbin).free(*start);
	       (*appbin).free(*end);
	       (*appbin).free(*hash_sum);
               delete startConst;
	       delete endConst;
               delete hashConst;
               delete assignStart;
               delete assignEnd;
               delete assignInitialHash;
               delete condition;
               delete valueAtAddr;
               delete sumHash;
               delete assignHash;
               delete oneConst;
               delete incrementStart;
               delete assignNewStart;
               delete expectedHash;
               delete ifCondition;
               delete exitCall;
               delete argToPrintf;
               delete printfCall;
               delete ifBodySeq;
               delete finalWhile;
               delete ifExpr;
               delete wholeSeq;
               delete whileSequence;*/
	       //BPatch_point* point = (*bb).findEntryPoint();
	       /*
	       unsigned long start = (*bb).getStartAddress();
               unsigned long end = (*bb).getEndAddress();
               unsigned int hash_sum = 0;
	       printf("start: %lx\n", start);
	       printf("end: %lx\n", end);*/
	       /*while(start != end) {
	          hash_sum += *start;
	          start++;
	       }
	       if(hash_sum != 0x6ae) {
	          exit(1);
	       }
               goto end;*/
	       counter++;
	    }
         }
      }
   }

   string newpath(path);
   newpath += "_instrumented";
   bool result = (*appbin).writeFile(newpath.c_str());
   if(!result) {
      fprintf(stderr, "Writing file to disk failed.\n");
      return -1;
   }
   return 0;
}

set<BPatch_basicBlock*> get_basic_blocks(BPatch_function* f) { 
   BPatch_flowGraph* fg = (*f).getCFG();
   set<BPatch_basicBlock*> blocks;   
   (*fg).getAllBasicBlocks(blocks);
return blocks;
}

void precompute_hashes(BPatch_image* appImage) {   
   vector<BPatch_function*>* functions = (*appImage).getProcedures();     
   for(BPatch_function* f: *functions) {
      cout << "Function name: " << (*f).getName() << endl;
      set<BPatch_basicBlock*> blocks = get_basic_blocks(f);
      unsigned int hashsum_bb = 0;
      for(BPatch_basicBlock* bb: blocks) {
         hashsum_bb = hash_sum(bb);
         printf("Sum of bytes in the binary of basic block: %x\n", hashsum_bb);
      }
   }
}

unsigned int hash_sum(BPatch_basicBlock* bb) {
   vector<Dyninst::InstructionAPI::Instruction::Ptr> instructions;
   (*bb).getInstructions(instructions);
   unsigned int hashsum_bb = 0;
   for(InstructionAPI::Instruction::Ptr instr: instructions) {
       size_t size = (*instr).size();
       unsigned char res;
       unsigned int hashsum_instr = 0;
       for(unsigned index = 0; index < size; index++) {
          res = (*instr).rawByte(index);
	  hashsum_instr += res;
       } 
       hashsum_bb += hashsum_instr;
   }
   return hashsum_bb;
}
