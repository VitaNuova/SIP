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

   string newpath(path);
   newpath += "_instrumented";
   bool result = (*appbin).writeFile(newpath.c_str());
   if(!result) {
      fprintf(stderr, "Writing file to disk failed.\n");
      return -1;
   }
   return 0;
}

void precompute_hashes(BPatch_image* appImage) {   
   vector<BPatch_function*>* functions = (*appImage).getProcedures();
   for(BPatch_function* f: *functions) {
      cout << "Function name: " << (*f).getName() << endl;
      BPatch_flowGraph* fg = (*f).getCFG();
      set<BPatch_basicBlock*> blocks;
      (*fg).getAllBasicBlocks(blocks);
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
