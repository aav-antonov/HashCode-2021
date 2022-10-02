/*
 * CityC_run.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */


#ifndef  CITYC_RUN_H_
#define CITYC_RUN_H_



 
// Abreviations:
// sid -> street id (int)
// nid -> node (junction) id (int)

class cityC_run: public cityC
{
    private:
    
    public:

    cityC * C;
    
    // number_of_mutations: counts all possible mutations (pair: street1 <--> street2)
    // number_of_fixedstreets: counts all possible mutations ( street1 )
    int number_of_mutations , number_of_fixedstreets ;

    
    // convert schedule into fixed_street
    int * StreetFirstCar;//current queries at node-> street junction

    //individual CRUN vars
    int *nodesSchedule , size_of_nodesSchedule , *StreetSchedule;
  
    

    // time(T) vars
    int *carsFinished, carsFinishedN;
    int *T_nodesStreetQ;//current queries at node-> street junction
    CAR * T_state_car; //
    
    FIXEDSID * fixedstreetsA;

    MUTATION * mutationA ;//current mutation  
    void  set_mutationA(MUTATION * mutation){mutationA = mutation;}
    
     
        
    //############ cityC.hpp #####################//
    void  fill_nodesSchedule(std::vector<FIXEDSID> & fixed_schedule); 
    void  fill_nodesSchedule(std::map<int, std::vector<int>> & nid2sids_schedule);
    
    void  init_time();
    void  update_streets_t(int t);
    void  update_node_t(int t);
    void  run();
    int   score();
    
    void  run_FIFG(std::vector<FIXEDSID> & fixed_schedule)  ;
    void  run_SCHEDULE(std::map<int, std::vector<int>> & schedule) ;
   

    std::map<int, std::vector<int>>   print_results_to_schedule(); 
    void  print_results(std::string fileOUT_solution);
    void  print_fixedstreets(std::string fileOUT, std::vector<FIXEDSID> & fixed_schedule);

     

    //######## MUTATION management on host ##############
    MUTATION *mutations;//top mutation generated for current run
    void  malloc_mutations();
    void  fill_mutations();
    void  sort_mutations_bycriteria(int size_of_mutations);

    void  mutate(MUTATION * mutation_best);
    void  mutate_list_filter_conflicts(std::vector<int> & indexes_to_mutate);
    void  mutate_list(std::vector<int> & indexes_to_mutate, int i_start, int size_to_mutate );

    

    //######## fixedstreets management on host ##############
    FIXEDSID * fixedstreets;
    void  malloc_mutations_FIFG();
    void  fill_mutations_FIFG();
    void  sort_mutations_bycriteria_FIFG(int size_of_mutations);
    
    
    //convert current CITY schedule into fixed_street vector 
    int *nodesScheduleB, *StreetScheduleB;
    void  fill_nodesScheduleB();
    void  update_node_t_B(int t, std::vector<FIXEDSID> & fixed_streets);
    std::vector<FIXEDSID>  fixed_streets_from_cityC_run();

    
    void malloc_nodesSchedule(){
      
      std::cout << "size_of_nodesSchedule:" << size_of_nodesSchedule << "\n";
      nodesSchedule = (int *) malloc(sizeof(int) * size_of_nodesSchedule);
      StreetSchedule = (int *) malloc(sizeof(int) * number_of_streets); 

      nodesScheduleB = (int *) malloc(sizeof(int) * size_of_nodesSchedule);
      StreetScheduleB = (int *) malloc(sizeof(int) * number_of_streets);   
    } 
    
   


    cityC_run(cityC * CA){
       C = CA;
       RunningTime = CA->RunningTime;
       Bonus = CA->Bonus;
       number_of_cars = CA->number_of_cars;
       number_of_streets = CA->number_of_streets; 
       number_of_nodes = CA->number_of_nodes;
       size_of_nodesSchedule = CA->size_of_nodesSchedule;
       
       streets_n2 = CA->streets_n2;
       streets_L = CA->streets_L;
       nodesSize = CA->nodesSize;
       nodesIndex = CA->nodesIndex; 
       carsSize = CA->carsSize;
       carsIndex = CA->carsIndex;
       carsStreet = CA->carsStreet;
       nodesSIDS =  CA->nodesSIDS; 

       T_nodesStreetQ = (int *) malloc( number_of_streets * sizeof(int));
       T_state_car = (CAR *) malloc(  number_of_cars * sizeof(CAR));
       carsFinished = (int *) malloc( number_of_cars * sizeof(int));
      
       
       /* initialize random seed: */
       srand (time(NULL));

       this->malloc_nodesSchedule();
       this->malloc_mutations_FIFG();
       this->malloc_mutations();


       /* set no mutation for standard run */
       //mutationA = MUTATION();
       mutationA = (MUTATION *) malloc(sizeof(MUTATION)  );
       mutationA->nid = -1;
       

       
        
    }
     ~cityC_run(){
      
      free(carsFinished);     
      free(T_state_car);
      free(T_nodesStreetQ);
            
      free(nodesSchedule);
      free(StreetSchedule);
      free(nodesScheduleB);
      free(StreetScheduleB);
          
      free(mutationA);
      free(mutations);  
      free(fixedstreets);
    
     }
};




#endif

