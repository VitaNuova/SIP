#include "BPatch.h"
#include "BPatch_function.h"
#include "BPatch_flowGraph.h"
#include "BPatch_point.h"

unsigned int hash_sum(BPatch_basicBlock*);
void precompute_hashes(BPatch_image*);
std::set<BPatch_basicBlock*> get_basic_blocks(BPatch_function*);
