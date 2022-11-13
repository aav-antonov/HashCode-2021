/*
 * structures.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */

#ifndef PARAM_SCREEN_H_
#define PARAM_SCREEN_H_

//------------ PARAM_SCREEN -----------------------------//
// parameters controls memory and number of threads per run on GPU
// number_of_mutations: total number of 1-step mutations
// size_of_mutations:  mutations to be screened (mutations are sorted by criteria and top K (K =  size_of_mutations) are screened )

// mutations are split in batches; number_of_batches so that:
// number_of_gpu * size_of_threads * number_of_batches ~ size_of_mutations

// number_of_gpu:  number of available gpu
// size_of_threads_GPU: number of threads per run per GPU (GPU memory control) , must be selected in respect to:  number_of_gpu * size_of_thread * number_of_batches ~ size_of_mutations



typedef struct
{    
    int number_of_mutations; // total number of mutation
    int size_of_mutations; // the number of mutations to screen for one optimization_round
    int number_of_gpu; 
    int size_of_threads_GPU; 
    int fixedstreets_current_MAXSIZE;//check this for larger prolems
    int optimization_rounds;
    
     
    
    // parameters controlling   CrossStreet mutation cuda screening  
    int mutationA_N = 1; // only for CrossStreet mutation cuda screening:  alwais 1 , outdated
    
    // parameters controlling   FIFG cuda screening   
    int clean_round_FIFG;// only for FIFG cuda screening: frequency of clean round (removing redundant fixed streets)
 
    // initial rounds to get fast to good solution
    int initial_rounds; // number of cuda screening runs 
    int initial_rounds_score_diff_MIN =20; // break condition , breaking cuda screening runs, minimal surplus of objective function not to be considered as  stagnation 
    int initial_rounds_count_stagnation_MAX = 10; // break condition , breaking cuda screening runs, max stagnation rounds

    // final rounds run by BLOCKS of 1000 optimization_rounds;
    // stops if progress per BLOCK(1000 rounds ) less then 500 
    int stop_criteria_per_1000_optimization_rounds = 500;

    std::string dir_results = "resultsQ"; // output dir with results files
          
} PARAM_SCREEN;

//------------ END PARAM_SCREEN  --------//






#endif /* PARAM_SCREEN_H_ */





