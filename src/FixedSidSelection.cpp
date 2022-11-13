





//######## FixedStreets management on host ##############
// To select set of screened FixedStreets (mutations for FIFG optimization) each possible mutation (FixedStreet)  is assigned a random value
// Top (PARAM.number_of_mutations)  are selected to be screened on GPU
// 

int CompareMutations_FIFG_Criteria(  const FIXEDSID & M1, const FIXEDSID & M2){return M1.criteria > M2.criteria;}


//select a random set (top mutation sorted by random value) of FixedStreets Mutations: size = size_of_mutations 
void  cityC_run::sort_mutations_bycriteria_FIFG(int size_of_mutations){
    std::sort( fixedstreets, fixedstreets+size_of_mutations,  &CompareMutations_FIFG_Criteria );
} 
 
//Compute the number of FixedStreets Mutations
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


//assigned a random value to  FixedStreets Mutations and sort them by this value

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






