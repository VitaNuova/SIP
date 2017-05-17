#include "BPatch.h"
#include "BPatch_function.h"
#include "BPatch_flowGraph.h"
#include "BPatch_point.h"

unsigned long calc_hash_sum(BPatch_basicBlock*);
void precompute_hashes(BPatch_image*);
std::set<BPatch_basicBlock*> get_basic_blocks(BPatch_function*);
BPatchSnippetHandle* insert_checker(BPatch_image*, BPatch_binaryEdit*, BPatch_basicBlock*, BPatch_point* point);
// BPatchSnippetHandle* insert_checker(BPatch_image*, BPatch_binaryEdit*, BPatch_basicBlock*, std::vector<BPatch_point*>*); 
