

//######## MUTATION management on host ##############


int CompareMutationsCriteria(  const MUTATION & M1, const MUTATION & M2){return M1.criteria > M2.criteria;}

void  cityC_run::sort_mutations_bycriteria(int size_of_mutations){
    std::sort( mutations, mutations+size_of_mutations,  &CompareMutationsCriteria );
} 


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

//-------------- mutate schedule by best mutation -----------//

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



//######## fixedstreets management on host ##############

int CompareMutations_FIFG_Criteria(  const FIXEDSID & M1, const FIXEDSID & M2){return M1.criteria > M2.criteria;}

void  cityC_run::sort_mutations_bycriteria_FIFG(int size_of_mutations){
    std::sort( fixedstreets, fixedstreets+size_of_mutations,  &CompareMutations_FIFG_Criteria );
} 
 

void  cityC_run::malloc_mutations_FIFG(){
      number_of_fixedstreets = 0;
      for (int nid = 0 ; nid < number_of_nodes  ; nid++) {
        
        
        int nid_size = nodesSize[nid];
        
        if(nid_size < 1){continue;}
        
        for (int s1 = 0 ; s1 < nid_size  ; s1++) {
         for (int s2 = 0 ; s2 < nid_size  ; s2++) {       
             number_of_fixedstreets++;
        }}
      }  

      fixedstreets = (FIXEDSID *) malloc(sizeof(FIXEDSID) * number_of_fixedstreets );
      printf("init_mutations_FIFG(): number_of_fixedstreets %d\n", number_of_fixedstreets) ; 
}



void  cityC_run::fill_mutations_FIFG(){
      
       
      int count = 0;
      for (int nid = 0 ; nid < number_of_nodes  ; nid++) {
        
        int nid_index = nodesIndex[nid];
        int nid_size = nodesSize[nid];
        
        if(nid_size < 1){continue;}
        
        for (int s1 = 0 ; s1 < nid_size  ; s1++) {
          int sid = nodesSIDS[nid_index + s1] ;
          for (int s2 = 0 ; s2 < nid_size  ; s2++) {
                      
            fixedstreets[count].nid = nid;
            fixedstreets[count].sid = sid;
            fixedstreets[count].i = s2;

            fixedstreets[count].criteria = rand() % 1000;
             
            count++; 

        }}

      }    
    
}




template<typename T>
void  max_item_byscore(int size_of_items, int scoreA, T * items, std::map<std::string, int> & counter, std::vector<int> & indexes_to_remove ){
      
      counter["max_i"] = 0;
      counter["count_positive"] = 0;
      counter["count_nonenegative"] = 0;
 
      int max_score = items[0].score;  

      for (int i = 0 ; i < size_of_items ;i++) {
          //printf("max_mutation_byscore: %d  %d \n", i, mutations[i].score );
          int df = items[i].score - scoreA;
          if(df > 0){
                counter["count_positive"]++;
          }
          else if(df == 0) {
                counter["count_nonenegative"]++; 
                indexes_to_remove.push_back(i);
          }  

          
         if(items[i].score > max_score){
           max_score = items[i].score;
           counter["max_i"] = i;
         }
      } 
         
} 



