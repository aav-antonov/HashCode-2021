//---------------------------------------------------------------//
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <thread>
#include <math.h>
#include <cmath>   
#include <map>
#include <unordered_map>

#include <time.h>
#include <sys/time.h>
#include <chrono>
#include <unistd.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
//---------------------------------------------------------------//


//---------------------------------------------------------------//
#include "src/city/car.h"
#include "src/city/mutation.h"
#include "src/city/fixedsid.h"
#include "src/city/cudavars.h"
#include "src/city/param_screen.h"
#include "src/city/City.h"
#include "src/city/CityC_run.h"

#include "src/city/cudavars.cpp"
#include "src/city/read_files.cpp"
#include "src/city/City.cpp"
#include "src/city/cityC_schedule2fixedstreet.cpp"
#include "src/cuda/checkErrorCuda.cpp"
#include "src/cuda/CityRunMemAllocCuda.cpp"
#include "src/cuda/CityRunKernelCuda.cpp"
#include "src/cuda/CityRunOnCuda.cpp"

#include "src/FixedSidSelection.cpp"
#include "src/MutationSelection.cpp"
#include "src/MaxItemByScore.hpp"
#include "src/optimize_FIFG.cpp"
#include "src/optimize_mutation.cpp"
//---------------------------------------------------------------//




int main (int argc, char * argv[]){


int number_of_gpu = 1;
std::string file_input, file_schedule = "no", file_fixedstreets = "no";
if (argc > 1){file_input = std::string(argv[1]);}
else{
std::cerr << "Usage: " <<  " ./solver <file_input>(required) <number_of_gpu> (required) <file_schedule>(optional) <file_fixedstreets>(optional)" << std::endl;
return(0);
}

if (argc > 1){number_of_gpu = std::stoi(std::string(argv[2]));}
if(argc == 4){file_schedule = std::string(argv[3]);}
if(argc == 5){file_fixedstreets = std::string(argv[4]);}



std::cout << "file_input:" << file_input << " number_of_gpu:" << number_of_gpu << "\n";
std::cout << "file_schedule:" << file_schedule << " file_fixedstreets:" << file_fixedstreets << "\n";


//-------------------------------------------------------------//
//-------load PARAM from file: "config_solver"----------------//
std::string file_config = "config_solver";
PARAM_SCREEN PARAM = PARAM_SCREEN();
read_config_file(file_config,  PARAM);
PARAM.number_of_gpu = number_of_gpu;
PARAM.size_of_mutations = PARAM.number_of_gpu * PARAM.size_of_threads_GPU;
printf("PARAM.size_of_mutations %d\n", PARAM.size_of_mutations);
//-------------------------------------------------------------//



//------------Create results dir-------------------------------//
std::cout << "Output dir:" << PARAM.dir_results << "\n";
boost::filesystem::create_directory(PARAM.dir_results);
//-------------------------------------------------------------//



//-------------------------------------------------------------//
//--------------- create cityC object -------------------------//
//----- if argc > 2: add schedule from <file_schedule>---------// 
//------------else : start from random schedule ---------------//
//-------------------------------------------------------------//
cityC * CC =  new cityC(file_input);
//-------------------------------------------------------------//


//-------------------------------------------------------------//
//----create cityC_run object----------------------------------//
//----allocate memory -----------------------------------------//
//----allocate  mutations (FIFG)-------------------------------//
//----allocate  mutations (all possible pairs)-----------------//
//-------------------------------------------------------------//
cityC_run * CRUN =  new cityC_run(CC);
//-------------------------------------------------------------//


//-------------------------------------------- ---------------------------------------------//
//----------------if start from predifine schedule: load schedule --------------------------//
std::map<int, std::vector<int>> schedule;
if(file_schedule != "no"){load_file_schedule(file_schedule, CC->map_sname2sid, schedule  );}
//-------------------------------------------- ---------------------------------------------//


//----------------if start from predifine schedule (fixed_streets): load schedule -------//
std::vector<FIXEDSID>  fixed_streets;
if(file_fixedstreets != "no"){load_file_fixedstreets(file_fixedstreets, fixed_streets  );}
//-------------------------------------------- ---------------------------------------------//


//-------------------------------------------- ----------------------------------------------//
//-------------------------------------------- ----------------------------------------------//
//---------------------------------initial run ----------------------------------------------//

if(file_schedule != "no"){

    printf("Start from predifined schedule file: file_schedule %s\n", file_schedule.c_str());
    CRUN->run_SCHEDULE(schedule);
    fixed_streets = CRUN->fixed_streets_from_cityC_run();
    PARAM.initial_rounds = 0;
}
else if (file_fixedstreets != "no"){

    printf("Start from predifined fixedstreets file: file_fixedstreets %s\n", file_fixedstreets.c_str());
    CRUN->run_FIFG(fixed_streets);
    printf("Start from predifined fixedstreets file: Score: %d fixed_streets.size(): %d \n", CRUN->score(), (int)fixed_streets.size());
    PARAM.initial_rounds = 0;
}
else{

    printf("Start run_FIFG from emty fixedstreets: fixed_streets.size %d\n", (int)fixed_streets.size());
    CRUN->run_FIFG(fixed_streets);
    printf("Start from emty fixedstreets: FIFG score %d\n", CRUN->score());

}

//-----------------------------initial run END-----------------------------------------------//
//-------------------------------------------- ----------------------------------------------//
//-------------------------------------------- ----------------------------------------------//



//---------------------Initial Rounds: --------------------------//
//-----"optimize_FIFG" round is followed by 
// --- "optimize_MUTATION" round
// --- PARAM.initial_rounds set to 2 rounds to get fast to score 4.5 million
//----------------------------------------------------------------// 
printf("PARAM.initial_rounds %d\n", PARAM.initial_rounds);


for(int k = 0; k < PARAM.initial_rounds; k++){

    test_correctness(CRUN);


//---------------------------------------------------------------//
// CRUN->number_of_fixedstreets: number of all possible fixed_street mutations
//---------------------------------------------------------------//
    PARAM.number_of_mutations = CRUN->number_of_fixedstreets;
    PARAM.optimization_rounds = 10000;
    optimize_FIFG(PARAM, CC, CRUN,  PARAM.dir_results, fixed_streets);
//---------------------------------------------------------------//


//---------------------------------------------------------------//
// CRUN->number_of_mutations: number of all possible mutations
//---------------------------------------------------------------//
    PARAM.number_of_mutations = CRUN->number_of_mutations;
    PARAM.optimization_rounds = 10000;
    optimize_MUTATION(PARAM, CC, CRUN,  PARAM.dir_results);
//---------------------------------------------------------------//


    PARAM.initial_rounds_count_stagnation_MAX *= 2;
    if(PARAM.initial_rounds_count_stagnation_MAX > 32){PARAM.initial_rounds_count_stagnation_MAX = 32;}
    PARAM.initial_rounds_score_diff_MIN /= 2;
    if(PARAM.initial_rounds_score_diff_MIN < 1){break;} 

}

//---------------------END of: Initial Rounds: ------------------//


//---------------------Main optimization rounds -----------------//
// final rounds run by BLOCKS of 1000 optimization_rounds;
// stops if progress per BLOCK(1000 rounds ) less then PARAM.stop_criteria_per_1000_optimization_rounds (default 5) 


//-- Set No break by stagnation-- //
PARAM.initial_rounds_score_diff_MIN = 0;
PARAM.initial_rounds_count_stagnation_MAX = 1e6;
//--------------------------------//


for(int k = 0; k < 1e6; k++){

    printf("PARAM.stop_criteria_per_1000_optimization_rounds %d \n", PARAM.stop_criteria_per_1000_optimization_rounds);

    schedule = CRUN->print_results_to_schedule();
    CRUN->run_SCHEDULE(schedule);
    int score_START = CRUN->score();


//---------------------------------------------------------------//
// CRUN->number_of_mutations: number of all possible mutations
// PARAM.optimization_rounds: number of tested mutation per round
// PARAM.stop_criteria_per_1000_optimization_rounds: minimal progress to break
//---------------------------------------------------------------//
    PARAM.number_of_mutations = CRUN->number_of_mutations;
    PARAM.optimization_rounds = 1000;
    optimize_MUTATION(PARAM, CC, CRUN,  PARAM.dir_results);
//---------------------------------------------------------------//

    schedule = CRUN->print_results_to_schedule();
    CRUN->run_SCHEDULE(schedule);
    int score_END = CRUN->score();

    int score_DIFF = score_END - score_START;
    printf("score_START %d score_END %d DIFF %d\n", score_START , score_END, score_DIFF);

    // break if progress per 1000 optimization rounds is less then 
    if(score_DIFF < PARAM.stop_criteria_per_1000_optimization_rounds){
        break;
    }


}



return EXIT_SUCCESS;
}

