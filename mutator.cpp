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
   BPatch_binaryEdit* appbin = bpatch.openBinary(path, true);
   if(appbin == NULL) {
      fprintf(stderr, "Something went wrong with opening the binary.\n");
      return -1;
   }
 
   BPatch_image* app_image = (*appbin).getImage();
   //precompute_hashes(appImage);
   
   vector<BPatch_function*>* functions = (*app_image).getProcedures();
   int counter = 0;     
   for(BPatch_function* f: *functions) {
      if((*f).getName() == "licenceCheck") {
         cout << "Function name: " << (*f).getName() << endl;
         set<BPatch_basicBlock*> blocks = get_basic_blocks(f);
         for(BPatch_basicBlock* bb: blocks) {
	    if(counter == 0) {
               vector<BPatch_point*>* points;
               vector<BPatch_function*> functions;
               bool findRes = (*app_image).findFunction("main", functions);
               points = (*functions[0]).findPoint(BPatch_locEntry);    
               
	       BPatchSnippetHandle* res = insert_checker(app_image, appbin, bb, points); 
	       if(res == NULL) {
                  fprintf(stderr, "Something wrong with inserting snippet\n");
		  exit(1);
               }
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


void precompute_hashes(BPatch_image* app_image) {   
   vector<BPatch_function*>* functions = (*app_image).getProcedures();     
   for(BPatch_function* f: *functions) {
      cout << "Function name: " << (*f).getName() << endl;
      set<BPatch_basicBlock*> blocks = get_basic_blocks(f);
      unsigned int hashsum_bb = 0;
      for(BPatch_basicBlock* bb: blocks) {
	 printf("Start address %x\n", (*bb).getStartAddress());
	 printf("End address %x\n", (*bb).getEndAddress());
         hashsum_bb = calc_hash_sum(bb);
         printf("Sum of bytes in the binary of basic block: %x\n", hashsum_bb);
      }
   }
}

unsigned long calc_hash_sum(BPatch_basicBlock* bb) {
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


BPatchSnippetHandle* insert_checker(BPatch_image* app_image, BPatch_binaryEdit* appbin, BPatch_basicBlock* bb, vector<BPatch_point*>* points) {
   BPatch_variableExpr* start = (*appbin).malloc(*((*app_image).findType("unsigned long")));             
   BPatch_variableExpr* end = (*appbin).malloc(*((*app_image).findType("unsigned long")));
   BPatch_variableExpr* hash_sum = (*appbin).malloc(*((*app_image).findType("unsigned long")));
                                                                                                       
   BPatch_constExpr start_const((*bb).getStartAddress());
   BPatch_constExpr end_const((*bb).getEndAddress());
   BPatch_constExpr hash_const((unsigned long)0);
   BPatch_constExpr* one_const = new BPatch_constExpr((unsigned long)1);
  
   BPatch_arithExpr* assign_start = new BPatch_arithExpr(BPatch_assign, *start, start_const);
   BPatch_arithExpr* assign_end = new BPatch_arithExpr(BPatch_assign, *end, end_const);
   BPatch_arithExpr* assign_initial_hash = new BPatch_arithExpr(BPatch_assign, *hash_sum, hash_const);

   BPatch_boolExpr while_condition(BPatch_ne, *start, *end);
                                                                                                                            
   BPatch_variableExpr* char_at_start = (*appbin).malloc(*((*app_image).findType("unsigned char")));
   BPatch_arithExpr* char_at_start_pointer = new BPatch_arithExpr(BPatch_addr, *char_at_start);
   
   vector<BPatch_function*> memcpy_funcs;
   (*app_image).findFunction("memcpy", memcpy_funcs);
   if(memcpy_funcs.size() == 0) {
      fprintf(stderr, "Could not find memcpy");
      exit(0);    
   }
                                                                                                                            
   vector<BPatch_snippet*> args_to_memcpy;
   args_to_memcpy.push_back(char_at_start_pointer);
   args_to_memcpy.push_back(start);
   args_to_memcpy.push_back(one_const);
   BPatch_funcCallExpr* memcpy_call = new BPatch_funcCallExpr(*(memcpy_funcs[0]), args_to_memcpy);
   BPatch_arithExpr* char_value_at_addr = new BPatch_arithExpr(BPatch_deref, *char_at_start_pointer); 
                                                                                                                            
   vector<BPatch_snippet*> args_to_printf_in_while; 
   BPatch_constExpr* arg_to_printf_in_while = new BPatch_constExpr("Current start %x. Current val in char at start %x.\n");
   args_to_printf_in_while.push_back(arg_to_printf_in_while);
   args_to_printf_in_while.push_back(start);
   args_to_printf_in_while.push_back(char_value_at_addr);
                                                                                                                     
   vector<BPatch_function*> printf_funcs;
   (*app_image).findFunction("printf", printf_funcs);
   if(printf_funcs.size() == 0) {
      fprintf(stderr, "Could not find printf");
      exit(0);    
   }
   BPatch_funcCallExpr* printf_call_in_while = new BPatch_funcCallExpr(*(printf_funcs[0]), args_to_printf_in_while);
   BPatch_arithExpr* total_hash = new BPatch_arithExpr(BPatch_plus, *hash_sum, *char_value_at_addr);
   BPatch_arithExpr* assign_hash = new BPatch_arithExpr(BPatch_assign, *hash_sum, *total_hash);	   
   BPatch_arithExpr* increment_start = new BPatch_arithExpr(BPatch_plus, *start, *one_const);
   BPatch_arithExpr* assign_new_start = new BPatch_arithExpr(BPatch_assign, *start, *increment_start);

   vector<BPatch_snippet*> while_items;
   while_items.push_back(memcpy_call);
   while_items.push_back(printf_call_in_while);
   while_items.push_back(total_hash);
   while_items.push_back(assign_hash);
   while_items.push_back(increment_start);
   while_items.push_back(assign_new_start);
   BPatch_sequence while_sequence(while_items);
   BPatch_whileExpr* final_while = new BPatch_whileExpr(while_condition, while_sequence);
                                                                                           
   unsigned long bb_hash = calc_hash_sum(bb);
   BPatch_constExpr expected_hash(bb_hash);
   BPatch_boolExpr if_condition(BPatch_ne, *hash_sum, expected_hash);
   
   vector<BPatch_snippet*> arg_to_exit;
   arg_to_exit.push_back(one_const);
   vector<BPatch_function*> exit_funcs;
   (*app_image).findFunction("exit", exit_funcs);
   if(exit_funcs.size() == 0) {
      fprintf(stderr, "Could not find exit");
        
   }
   BPatch_funcCallExpr* exit_call = new BPatch_funcCallExpr(*(exit_funcs[0]), arg_to_exit);
   BPatch_constExpr* arg_to_printf = new BPatch_constExpr("Fail, computed hash %x.\n");
   vector<BPatch_snippet*> args_to_printf;
   args_to_printf.push_back(arg_to_printf);
   args_to_printf.push_back(hash_sum);
   BPatch_funcCallExpr* printf_call = new BPatch_funcCallExpr(*(printf_funcs[0]), args_to_printf);
   
   BPatch_constExpr* arg_to_succ_printf = new BPatch_constExpr("Success, computed hash %x.\n");
   vector<BPatch_snippet*> args_to_succ_printf;
   args_to_succ_printf.push_back(arg_to_succ_printf);
   args_to_succ_printf.push_back(hash_sum);
   BPatch_funcCallExpr printf_succ_call(*(printf_funcs[0]), args_to_succ_printf);
   
   vector<BPatch_snippet*> if_body;
   if_body.push_back(exit_call);
   if_body.push_back(printf_call);
   BPatch_sequence if_body_seq(if_body);
   BPatch_ifExpr* if_expr = new BPatch_ifExpr(if_condition, if_body_seq, printf_succ_call); 
   
   vector<BPatch_snippet*> whole_snippet;
   whole_snippet.push_back(assign_start);
   whole_snippet.push_back(assign_end);
   whole_snippet.push_back(assign_initial_hash);
   whole_snippet.push_back(final_while);
   whole_snippet.push_back(if_expr);
   
   BPatch_sequence whole_seq(whole_snippet);
   BPatchSnippetHandle* handle = (*appbin).insertSnippet(whole_seq, *points);  
                                                                                                   
   /*(*appbin).free(*start);
   (*appbin).free(*end);
   (*appbin).free(*hash_sum);
   (*appbin).free(*char_at_start);*/
                                                                                                   
   delete assign_start;
   delete assign_end;
   delete assign_initial_hash;
   delete one_const;
   delete memcpy_call;
   delete char_at_start_pointer;
   delete char_value_at_addr;
   delete arg_to_printf_in_while;
   delete printf_call_in_while;
   delete total_hash;
   delete assign_hash;
   delete increment_start;
   delete assign_new_start;
   delete final_while;
   delete exit_call;
   delete arg_to_printf;
   delete printf_call;
   delete arg_to_succ_printf;
   delete if_expr;

   return handle;
}
