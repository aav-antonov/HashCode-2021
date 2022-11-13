

//######## MUTATION management on host ##############
// To select set of screened mutations (to screen on GPU) each possible mutation is assigned a random value
// Top (PARAM.number_of_mutations)  are selected to be screened on GPU
// 


int CompareMutationsCriteria(  const MUTATION & M1, const MUTATION & M2){return M1.criteria > M2.criteria;}

//select a random set (top mutation sorted by random value) of Mutations: size = size_of_mutations 
void  cityC_run::sort_mutations_bycriteria(int size_of_mutations){
    std::sort( mutations, mutations+size_of_mutations,  &CompareMutationsCriteria );
} 


//Compute number of possible mutations
void  cityC_run::malloc_mutations(){
      number_of_mutations = 0;
      for (int nid = 0 ; nid < number_of_nodes  ; nid++) {
        int nid_size = nodesSize[nid];
        
        if(nid_size < 1){continue;}
        
        for (int s1 = 0 ; s1 < nid_size  ; s1++) {
          for (int s2 = 0 ; s2 < nid_size  ; s2++) {
             
             if(s1 >= s2){continue;} 
                
             number_of_mutations++;
        }}
     }  

mutations = (MUTATION *) malloc(sizeof(MUTATION) * number_of_mutations );
printf("init_mutations(): %d  \n", number_of_mutations );
}
   

//Assign to each mutation random score
void   cityC_run::fill_mutations(){
 
   
    int count_permutation = 0;
    for (int nid = 0 ; nid < number_of_nodes  ; nid++) {
        
        //int nid_index = nodesIndex[nid];
        int nid_size = nodesSize[nid];
        
        if(nid_size < 1){continue;}
        
        for (int s1 = 0 ; s1 < nid_size  ; s1++) {
          for (int s2 = 0 ; s2 < nid_size  ; s2++) {
             
             if(s1 >= s2){continue;} 
                          
             mutations[count_permutation].nid = nid;
             mutations[count_permutation].i1 = s1;
             mutations[count_permutation].i2 = s2;
             mutations[count_permutation].criteria = rand() % 100 + 1;  
             count_permutation++;
          
        }}
     }
                
 }


//-------------------------------------------------------//
//-------------- mutate schedule by best mutation -----------//
//-------------------------------------------------------//

void  cityC_run::mutate(MUTATION * mutation_best){
  
     int nid = mutation_best->nid ;
    
     int i1 = mutation_best->i1;
     int i2 = mutation_best->i2; 
     //int score = mutation_best->score;  
       
     int nid_index = nodesIndex[nid];
     
     int sid1 = nodesSchedule[nid_index +i1];
     int sid2 = nodesSchedule[nid_index +i2];    
     //std::cout << "mutate:" << nid  << " " << sid1 << " " << sid2 << " " << i1 << " " << i2 << " " <<  score << "\n";
     //std::cout << "nodesSchedule:" << nodesSchedule[nid_index +i1]  << " " << nodesSchedule[nid_index +i2] << "\n"; 
     nodesSchedule[nid_index +i1] = sid2;

     nodesSchedule[nid_index +i2] = sid1;
    
     

     mutation_best->i1 = i2;
     mutation_best->i2 = i1;
     
}

//-------------------------------------------------------//
//-------------- mutate schedule by list of mutations -----------//
//-------------------------------------------------------//

void  cityC_run::mutate_list_filter_conflicts(std::vector<int> & indexes_to_mutate){
     

     std::map<std::string, int> filtr_the_same_street_mutations;

     int i_max = indexes_to_mutate.size() -1; 
     for(int i=i_max; i>=0 ;i--){

       int i_mutate = indexes_to_mutate[i];

       MUTATION * mutation_i = &mutations[i_mutate];

       int nid = mutation_i->nid ;
    
       int i1 = mutation_i->i1;
       int i2 = mutation_i->i2;

       std::string nid_i1 = std::to_string(nid) + std::to_string(i1);
       std::string nid_i2 = std::to_string(nid) + std::to_string(i2);

       int del = -1;
       if (filtr_the_same_street_mutations.count(nid_i1)) {del =1;}
       if (filtr_the_same_street_mutations.count(nid_i2)) {del =1;} 
       
       if(del == 1){
         indexes_to_mutate.erase(indexes_to_mutate.begin() + i);
         
        }   
       filtr_the_same_street_mutations[nid_i1] = 1;
       filtr_the_same_street_mutations[nid_i2] = 1;    
       
       
     }
     
}


void  cityC_run::mutate_list(std::vector<int> & indexes_to_mutate, int i_start, int size_to_mutate ){

     if(indexes_to_mutate.size() < size_to_mutate){ size_to_mutate = indexes_to_mutate.size();} 


     //printf("mutate_list of size: i_start %d size_to_mutate %d  \n", i_start , size_to_mutate);

     
     for(int i=i_start; i< i_start + size_to_mutate;i++){

       int i_mutate = indexes_to_mutate[i];

       MUTATION * mutation_i = &mutations[i_mutate];

       mutate(mutation_i); 
     }
     
}









