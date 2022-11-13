





void  optimize_MUTATION
(
PARAM_SCREEN & PARAM, 
cityC * CC,
cityC_run * CRUN,
std::string dir_results
)
{


//------------------------------------------------------------------------//
//----allocate city and city_run arrays on cuda --------------------------//
//----copy data to cuda --------------------------------------------------//
//----put pointers to all city and city_run arrays in vector: args_run ---//
//----copy args_run to cuda: args_run_cuda (to use in cuda kernels)    ---//
//---- var: <size_of_threads_GPU> defines the number of permutations tested at each GPU in one run--//
//---- var: <size_of_mutations> defines the overall number of mutation to be tested --// 
//------------------------------------------------------------------------//

std::vector<CUDAVARS*> args_run(PARAM.number_of_gpu), args_run_cuda(PARAM.number_of_gpu);
malloc_cuda_city_data_fixed( args_run,  CC, PARAM);
malloc_cuda_city_data_TIME( args_run,  CC, PARAM );
malloc_cuda_city_data_nodesSchedule(  args_run,   CRUN,  PARAM );
malloc_cuda_city_data_MUTATION( args_run,  CRUN, PARAM );//
//------------------------------------------------------------------------//
//------------------------------------------------------------------------//



//------------------------------------------------------------------------//
//set mutation type of screening for cuda and copy args_run to cuda
for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    args_run[dev]->type = 1;
    args_run[dev]->mutationA_N = PARAM.mutationA_N;
}

copy_args_run_to_cuda(PARAM,args_run, args_run_cuda);
//------------------------------------------------------------------------//


std::map<int, std::vector<int>> schedule = CRUN->print_results_to_schedule();
CRUN->run_SCHEDULE(schedule);
int scoreA = CRUN->score();
int score_input = scoreA;


print_args_run_param(args_run);



int count_stagnation = 0;
for(int i =0;i<PARAM.optimization_rounds;i++){

float time;
cudaEvent_t start, stop;
cudaEventCreate(&start);
cudaEventCreate(&stop);
cudaEventRecord(start, 0);


//-----------------------------------------------------------//
//------------------run on cuda -----------------------------//
//-----------------------------------------------------------//
if(PARAM.size_of_mutations > PARAM.number_of_mutations){PARAM.size_of_mutations = PARAM.number_of_mutations;}
printf("PARAM.size_of_threads_GPU %d, PARAM.size_of_mutations  %d, PARAM.optimization_rounds %d \n", PARAM.size_of_threads_GPU, PARAM.size_of_mutations, PARAM.optimization_rounds);

// -- prepare mutations on host (randomly select <number_of_mutations>) --/ 
CRUN->fill_mutations();
CRUN->sort_mutations_bycriteria(CRUN->number_of_mutations);

// -- copy prepared mutations to cuda -- //
copy_cuda_city_data_nodesSchedule( args_run,  CRUN, PARAM );


// -- test mutations on cuda, and copy mutations scores back to host -- //
city_run_on_cuda(  PARAM, CC, CRUN, args_run, args_run_cuda);

//-----------------------------------------------------------//
//------------------End run on cuda --------------------------//
//-----------------------------------------------------------//


std::vector<int> indexes_to_mutate;
std::map<std::string, int> counter;
counter["max_i"] = 0;
counter["count_positive"] = 0;
counter["count_nonenegative"] = 0;  

max_item_byscore(PARAM.size_of_mutations, scoreA, CRUN->mutations, counter, indexes_to_mutate);

printf("count_positive: %d \n", counter["count_positive"]);
printf("count_nonenegative: %d \n", counter["count_nonenegative"]);
printf("indexes_to_remove.size(): %d \n", (int)indexes_to_mutate.size());

CRUN->mutate_list_filter_conflicts(indexes_to_mutate);

printf("indexes_to_remove.size(): %d \n", (int)indexes_to_mutate.size());




if(counter["count_positive"] > 0){ 

    int max_i = counter["max_i"];
    
    //----mutate **nodesSchedule on host  ---//
    CRUN->mutate(&CRUN->mutations[max_i]);
    CRUN->init_time();
    CRUN->run();
    
    if (CRUN->score() != CRUN->mutations[max_i].score){
        printf("ERROR, host run not equals cuda score: %d  CRUN->mutations[max_i].score: %d ", CRUN->score(), CRUN->mutations[max_i].score);
        exit(0);
    } 
      
    std::string file_results_best = PARAM.dir_results + "/current_best_M";
    CRUN->print_results(file_results_best);
    
}
else if (counter["count_nonenegative"] > 0){
    
    int block_size = 512;
    int count_nonenegative = counter["count_nonenegative"];
    
    while(count_nonenegative <= block_size){
        block_size /= 2;
        printf("change block_size: %d  count_nonenegative %d\n", block_size, count_nonenegative);
    }
     
    
    
    std::map<int, std::vector<int>> schedule = CRUN->print_results_to_schedule();

    

    int status = -1;
    while(status == -1){
         
        CRUN->fill_nodesSchedule(schedule);        
        CRUN->mutate_list(indexes_to_mutate, 0, block_size);
        CRUN->init_time();
        CRUN->run();

    
        if( CRUN->score() < scoreA  ){


            /*---report error and exit  ---*/
            if(block_size == 1){
                
                    printf("ERROR:   block_size=1, CRUN->score() != scoreA %d %d\n", CRUN->score(), scoreA );
                    exit(0);
               
                
            }else{ 

                block_size /= 2;
                status = -1;
            }

        }else{
            
            status = 1;
            printf("Block_size : %d scoreA: %d  \n", block_size, scoreA);
    
        }
    }
    



}
else{

printf("Break: No NoneNegative Mutations");
break;

}

int score_new = CRUN->score();
//std::cout << "score_new:" << score_new  << "\n";
int score_diff = score_new - scoreA; 


//--------------------------- ---------//
//---------Checking for Break ---------//
//--------------------------- ---------//

if(score_diff < PARAM.initial_rounds_score_diff_MIN){
    count_stagnation++;
    printf("count_stagnation:  %d \n", count_stagnation);
}else{
    count_stagnation = 0;
}

if(count_stagnation > PARAM.initial_rounds_count_stagnation_MAX){
    printf("best score (optimize_MUTATION):  %d \n", scoreA);
    break;
}
//---------Checking for Break END---------//
//--------------------------- ------------//



cudaEventRecord(stop, 0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&time, start, stop);

scoreA = score_new; 
int commulative_diff = scoreA - score_input;
printf("i score:  %d %d score_diff: %d  time:  %d commulative_diff %d\n", i, scoreA, score_diff, (int)time/1000, commulative_diff);
printf("____________________________\n");



std::string file_results_best = dir_results + "/current_best";
CRUN->print_results(file_results_best);
//exit(1);
}

std::string file_results_best = dir_results + "/res_" + std::to_string(scoreA) + "_best";
CRUN->print_results(file_results_best);

free_cuda_city_data_fixed(args_run, PARAM);
free_cuda_city_data_TIME(args_run, PARAM );
free_cuda_city_data_nodesSchedule(args_run, PARAM);
free_cuda_city_data_MUTATION(args_run, PARAM);

}


