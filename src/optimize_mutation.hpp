



void  clean_round_FIFG
(
PARAM_SCREEN & PARAM, 
cityC * CC,
cityC_run * CRUN, 
std::vector<FIXEDSID> & fixed_streets,
std::vector<CUDAVARS*> & args_run, 
std::vector<CUDAVARS*> & args_run_cuda,
int scoreA
)
{
printf("-----\nclean_round_FIFG\n");

// -- set flag for cleanning on args on GPU: args_run[dev]->clean_FIFG = 1; --- // 
for(int dev=0; dev < PARAM.number_of_gpu; dev++) {args_run[dev]->clean_FIFG = 1;}

copy_cuda_city_data_fixedstreets_current( fixed_streets, args_run,  CRUN,  PARAM );
copy_args_run_to_cuda(PARAM,args_run, args_run_cuda);
// ---------------------------------------------------------------------------- //


 
int size_of_mutations = PARAM.size_of_mutations;

int go = 1;
while(go == 1){

go = -1;

PARAM.size_of_mutations = fixed_streets.size();
printf("clean_fixed_streets_FIFG: size_of_threads_GPU %d, size_of_mutations %d \n", PARAM.size_of_threads_GPU, size_of_mutations);
city_run_on_cuda(  PARAM, CC, CRUN, args_run, args_run_cuda);

//int minDF = 0;
//std::vector<int> indexes_to_remove;
//int count_positive = get_indexes_to_remove(indexes_to_remove,PARAM, CRUN, scoreA,minDF);
//printf("count_positive: %d \n", count_positive);

std::vector<int> indexes_to_remove;
std::map<std::string, int> counter;
counter["max_i"] = 0;
counter["count_positive"] = 0;
counter["count_nonenegative"] = 0;  

max_item_byscore(PARAM.size_of_mutations, scoreA, CRUN->fixedstreets, counter, indexes_to_remove);

printf("count_positive: %d \n", counter["count_positive"]);
printf("count_nonenegative: %d \n", counter["count_nonenegative"]);
printf("indexes_to_removeB.size(): %d \n", (int)indexes_to_remove.size());


if(counter["count_positive"] > 0){ 

    int max_i = counter["max_i"];

    printf("fixed_streets.size() A: %d max_i %d score %d \n", (int)fixed_streets.size(), max_i, CRUN->fixedstreets[max_i].score);
    
    fixed_streets.erase (fixed_streets.begin()+max_i);

    CRUN->run_FIFG(fixed_streets);

    printf("new score %d , scoreA %d\n", CRUN->score(),scoreA);

    if( CRUN->score() < scoreA  ){
        printf("ERROR:  CRUN->score() not better scoreA %d %d\n", CRUN->score(), scoreA );
        exit(0);
    }

    scoreA = CRUN->score();
    go = 1;

    std::string file_results_best = PARAM.dir_results + "/current_best";
    CRUN->print_results(file_results_best);

    std::string file_fixedstreets = PARAM.dir_results + "/current_fixedstreets";
    CRUN->print_fixedstreets(file_fixedstreets, fixed_streets);
}

else if (counter["count_nonenegative"] > 0)
{
    int block_size = 256;
    int count_nonenegative = counter["count_nonenegative"];
    
    while(count_nonenegative < block_size){
        block_size /= 2;
        printf("change block_size: %d  count_nonenegative %d\n", block_size, count_nonenegative);
    }
     
    std::vector<FIXEDSID> fixedstreets_copy = fixed_streets;

    int status = -1;
    while(status == -1){
        int j1 = indexes_to_remove.size()-1;
        int j2 = indexes_to_remove.size()-1 - block_size;
        //printf("j1: %d j2: %d\n",  j1, j2);  
        for(int j = j1;j > j2;j--){

            fixed_streets.erase (fixed_streets.begin()+indexes_to_remove[j]);
            //printf("remove j: %d %d\n",  j, indexes_to_remove[j]); 

        }
  
        CRUN->run_FIFG(fixed_streets);

        printf("block_removed: fixed_streets.size() %d CRUN->score() %d\n",  (int)fixed_streets.size(), CRUN->score());
        //exit(0);
    
        if( CRUN->score() < scoreA  ){


            /*---report error and exit  ---*/
            if(block_size == 1){
                printf("ERROR: block_size=1, CRUN->score() != scoreA %d %d\n", CRUN->score(), scoreA );
            } 

        fixed_streets = fixedstreets_copy;
        block_size /= 2;
        
        status = -1;

        }else{
            scoreA =  CRUN->score();
            status = 1;
            
             
            std::string file_results_best = PARAM.dir_results + "/current_best";
            CRUN->print_results(file_results_best);

            std::string file_fixedstreets = PARAM.dir_results + "/current_fixedstreets";
            CRUN->print_fixedstreets(file_fixedstreets, fixed_streets);

            printf("Block_size removed: %d fixed_streets.size() %d scoreA: %d\n", block_size, (int)fixed_streets.size(), scoreA);
    
        }
    }
 
    

    go = 1;
    
}
else{
 go = -1;
}




if(go == 1){
copy_cuda_city_data_fixedstreets_current( fixed_streets, args_run,  CRUN,  PARAM );
copy_args_run_to_cuda(PARAM,args_run, args_run_cuda);

}

}


// -- set flag back for  standard_round_FIFG on args on GPU: args_run[dev]->clean_FIFG = -1; --- // 
for(int dev=0; dev < PARAM.number_of_gpu; dev++) {args_run[dev]->clean_FIFG = -1;}
copy_args_run_to_cuda(PARAM,args_run, args_run_cuda);
// ---------------------------------------------------------------------------- //


PARAM.size_of_mutations = size_of_mutations;

printf("-----\nclean_round_FIFG\n");

}


void  standard_round_FIFG
(
PARAM_SCREEN & PARAM, 
cityC * CC,
cityC_run * CRUN, 
std::vector<FIXEDSID> & fixed_streets,
std::vector<CUDAVARS*> & args_run, 
std::vector<CUDAVARS*> & args_run_cuda,
int scoreA
)
{

copy_cuda_city_data_fixedstreets_current( fixed_streets, args_run,  CRUN,  PARAM );
copy_args_run_to_cuda(PARAM,args_run, args_run_cuda);

// fill and randomly sort all mutations on host:  CRUN->fixedstreets size_of: CRUN->number_of_fixedstreets //
CRUN->fill_mutations_FIFG();
CRUN->sort_mutations_bycriteria_FIFG(CRUN->number_of_fixedstreets);


if(PARAM.size_of_mutations > PARAM.number_of_mutations){PARAM.size_of_mutations = PARAM.number_of_mutations;}

city_run_on_cuda(  PARAM, CC, CRUN, args_run, args_run_cuda);

//---- find MAX mutation and update fixed_streets on host  ---//

std::vector<int> indexes_to_remove;
std::map<std::string, int> counter;
counter["max_i"] = 0;
counter["count_positive"] = 0;
counter["count_nonenegative"] = 0;  

max_item_byscore(PARAM.size_of_mutations, scoreA, CRUN->fixedstreets, counter, indexes_to_remove);

int max_i = counter["max_i"]; //CRUN->max_fixedstreet_byscore(PARAM.size_of_mutations,scoreA);


if(CRUN->fixedstreets[max_i].score -scoreA > 0){fixed_streets.push_back(CRUN->fixedstreets[max_i]);}

printf("max_i %d  score %d\n", max_i, CRUN->fixedstreets[max_i].score);


}



void  optimize_FIFG
(
PARAM_SCREEN & PARAM, 
cityC * CC,
cityC_run * CRUN,
std::string dir_results,
std::vector<FIXEDSID> & fixed_streets

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
malloc_cuda_city_data_TIME( args_run, CC, PARAM );
malloc_cuda_city_data_nodesSchedule(  args_run,   CRUN,  PARAM );
malloc_cuda_city_data_FIFG(  args_run,  CRUN,  PARAM );


//set mutation type of screening
for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    args_run[dev]->size_of_threads = PARAM.size_of_threads_GPU;
    args_run[dev]->cuda_device = dev;
    args_run[dev]->type = 2;
    args_run[dev]->mutationA_N = PARAM.mutationA_N;
    args_run[dev]->fixedstreets_current_size = fixed_streets.size(); 
}

//------------------------------------------------------------------------//

CRUN->run_FIFG(fixed_streets);
int scoreA = CRUN->score();

print_args_run_param(args_run);

printf("CRUN->run_FIFG(fixed_streets); scoreA %d\n", scoreA);

if(fixed_streets.size() > 0){clean_round_FIFG(PARAM,CC,CRUN,fixed_streets,args_run, args_run_cuda, scoreA);}

CRUN->run_FIFG(fixed_streets);
scoreA = CRUN->score();
printf("CRUN->run_FIFG(fixed_streets); scoreA %d\n", scoreA);


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



int i_clean = PARAM.clean_round_FIFG; 

if( i % i_clean == (i_clean-1) ){clean_round_FIFG(PARAM,CC,CRUN,fixed_streets,args_run, args_run_cuda, scoreA);}
                            else{standard_round_FIFG(PARAM,CC,CRUN,fixed_streets,args_run, args_run_cuda , scoreA);} // 


//----------------run on cuda END----------------------------//
//-----------------------------------------------------------//

printf("fixed_streets.size(): %d \n", (int)fixed_streets.size() );

CRUN->run_FIFG(fixed_streets);
int score_diff = CRUN->score() - scoreA;

printf("CRUN->run_FIFG(fixed_streets); CRUN->score() %d\n", CRUN->score());



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
    printf("best score (optimize_FIFG):  %d \n", scoreA);
    break;
}

//---------Checking for Break END---------//
//--------------------------- ------------//



scoreA = CRUN->score(); 

std::string file_results_best = dir_results + "/current_best";
CRUN->print_results(file_results_best);

std::string file_fixedstreets = dir_results + "/current_fixedstreets";
CRUN->print_fixedstreets(file_fixedstreets, fixed_streets);


cudaEventRecord(stop, 0);
cudaEventSynchronize(stop);
cudaEventElapsedTime(&time, start, stop);

printf("i score:  %d %d score_diff: %d  time:  %3f\n", i, scoreA, score_diff, time/1000);
printf("____________________________\n");

}

std::string file_results_best = dir_results + "/res_" + std::to_string(scoreA) + "_best";
CRUN->print_results(file_results_best);

std::string file_fixedstreets = dir_results + "/current_fixedstreets" + std::to_string(scoreA) + "_best";
CRUN->print_fixedstreets(file_fixedstreets, fixed_streets);

free_cuda_city_data_fixed(args_run, PARAM);
free_cuda_city_data_TIME(args_run, PARAM );
free_cuda_city_data_nodesSchedule(args_run, PARAM);
free_cuda_city_data_FIFG(args_run, PARAM);

}














//####################################################



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


