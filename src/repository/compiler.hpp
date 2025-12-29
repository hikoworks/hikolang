
#pragma once

#include "module_list.hppp"
#include "utility/thread_pool.hpp"
#include <vector>

namespace hk {

struct make_ast_context {

};

/**
 * 
 * The @a sequence is used for determining if files need to be recompiled.
 * 
 * @param num_threads How many parralel compilations should be executed.
 * @param sequence Sequence number for compilation.
 */
void compile(module_list &modules, std::vector<source*> &executables, make_ast_context &ctx, size_t num_threads, size_t sequence);

void synthesize();

//void 

}