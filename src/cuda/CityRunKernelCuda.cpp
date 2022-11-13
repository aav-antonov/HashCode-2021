







__device__  void  score(int tid, CUDAVARS * args_run){

        int * carsFinished = &args_run->carsFinished[tid * args_run->number_of_cars] ;
        int score  = 0; 
        for(int i=0; i< args_run->number_of_cars;i++){
           if(carsFinished[i] > 0){
             score += args_run->RunningTime - carsFinished[i] -1;
             score += args_run->Bonus; 
            }
        }
        
        if(args_run->type == 1){
         for (int o = 0;o<args_run->mutationA_N;o++){
            args_run->mutations[tid*args_run->mutationA_N+o].score = score;
         }
        } 
        

        if(args_run->type == 2){
         args_run->fixedstreets[tid].score = score;
        }  
 
} 

__device__  void  update_streets_t(int tid, int t, CUDAVARS * args_run,  int * carsFinishedN)
{ 

        int * T_nodesStreetQ = &args_run->T_nodesStreetQ[tid * args_run->number_of_streets];
        CAR * T_state_car = &args_run->T_state_car[tid * args_run->number_of_cars];
        int * carsFinished = &args_run->carsFinished[tid * args_run->number_of_cars] ;
        

        for(int i=0; i< args_run->number_of_cars;i++){

            if (T_state_car[i].l > 0){T_state_car[i].l--;}  

            if (T_state_car[i].l == 0){
                int sid = T_state_car[i].sid;
                T_state_car[i].q = T_nodesStreetQ[sid];
                T_nodesStreetQ[sid]++;
                T_state_car[i].l--;

                if(T_state_car[i].i == args_run->carsSize[i]-1){
                    T_state_car[i].l = -100;//finished
                    T_nodesStreetQ[sid]--;// remove finished car from junqtion
                    carsFinished[i] = t;
                    (*carsFinishedN)++;

                }
           }
        }
  
} 


__device__ void  update_node_t(int tid, int t, CUDAVARS * args_run)
{ 
   
   int * T_nodesStreetQ = &args_run->T_nodesStreetQ[tid * args_run->number_of_streets];
   CAR * T_state_car = &args_run->T_state_car[tid * args_run->number_of_cars];
   
   MUTATION * mutationA = &args_run->mutations[tid*args_run->mutationA_N];

   int * nodesSchedule = &args_run->nodesSchedule[0];
   if(args_run->type == 2){  nodesSchedule = &args_run->nodesSchedule[tid * args_run->size_of_nodesSchedule] ;}
  
   int * StreetSchedule = &args_run->StreetSchedule[tid * args_run->number_of_streets] ;
   
   
   for(int i=0; i< args_run->number_of_cars;i++){

            
            if (T_state_car[i].l == -1){
                
                int sid = T_state_car[i].sid;
                int nid = args_run->streets_n2[sid];
                
                int nSize = args_run->nodesSize[nid];
                int nid_index = args_run->nodesIndex[nid];
                int i_time = t  % nSize;
                int sid_green = nodesSchedule[nid_index  + i_time];

              

                if(args_run->type == 2){
                 if(sid_green == -1){
                  if(StreetSchedule[sid] == -1){  
                    
                    
                    nodesSchedule[nid_index  + i_time] = sid;
                    StreetSchedule[sid] = i_time;
                    sid_green = sid;
                }}}
                
                
                
                if(args_run->type == 1){
                   for (int o = 0;o<args_run->mutationA_N;o++){
                    if(mutationA[o].nid == nid){
                        if(mutationA[o].i1 == i_time){sid_green = nodesSchedule[nid_index  + mutationA[o].i2]; }
                        if(mutationA[o].i2 == i_time){ sid_green = nodesSchedule[nid_index  + mutationA[o].i1];}
                    }      
                   }      
                } 
                
            
            int cid_index = args_run->carsIndex[i];     
            if (sid == sid_green){
                if (T_state_car[i].q == 0){
                    T_state_car[i].i++;
                    
                    T_state_car[i].sid = args_run->carsStreet[cid_index + T_state_car[i].i]; ;
                    T_state_car[i].l = args_run->streets_L[T_state_car[i].sid];
                    T_state_car[i].q = -1;
                    T_nodesStreetQ[sid]--;
                    }
                    else{
                    T_state_car[i].q--;
                    }
   
            }
         }
     }      
        
}   


__device__ void  init_time_vars(int tid,  CUDAVARS * args_run)
{ 

   int * T_nodesStreetQ = &args_run->T_nodesStreetQ[tid * args_run->number_of_streets];
   CAR * T_state_car = &args_run->T_state_car[tid * args_run->number_of_cars];
   int * carsFinished = &args_run->carsFinished[tid * args_run->number_of_cars] ;

   for( int i=0;i< args_run->number_of_streets;i++){T_nodesStreetQ[i] = 0;}
       
       for(int i=0; i< args_run->number_of_cars;i++){
            carsFinished[i] = -1;
            int cid_index = args_run->carsIndex[i];
            int sid = args_run->carsStreet[cid_index + 0];
            
            T_state_car[i].i = 0; //count of street in car path
            T_state_car[i].sid = sid;//current street id 
            T_state_car[i].l = -1;// position on street, -1 waiting at node (junction)
            T_state_car[i].q = T_nodesStreetQ[sid];//position in quiry
            T_nodesStreetQ[sid]++;
        } 
   
}   

__device__ void  init_FIFG(int tid, CUDAVARS * args_run)
{ 
   int dev = args_run->cuda_device;
   int size_of_threads = args_run->size_of_threads;
   int * StreetSchedule = &args_run->StreetSchedule[tid * args_run->number_of_streets] ;
   for( int k=0;k< args_run->number_of_streets;k++){StreetSchedule[k] = -1;}
   
   int * nodesSchedule = &args_run->nodesSchedule[tid * args_run->size_of_nodesSchedule];
   for( int k=0;k< args_run->size_of_nodesSchedule;k++){nodesSchedule[k] = -1;}
    
   int * nodesIndex = args_run->nodesIndex;
  
   FIXEDSID * fixedstreets_current = args_run->fixedstreets_current;

      
   for( int k=0;k< args_run->fixedstreets_current_size;k++){
      int nid = fixedstreets_current[k].nid;
      int sid = fixedstreets_current[k].sid;
      int i = fixedstreets_current[k].i;
      
      
      if(StreetSchedule[sid] > 0){continue;}

      
      if(args_run->clean_FIFG == 1){
       if(tid + dev*size_of_threads == k){
         continue; 
      }}
        
      
      nodesSchedule[nodesIndex[nid] + i] = sid;
      StreetSchedule[sid] = i; 
      
    }
    
    if(args_run->clean_FIFG == -1){
    
    FIXEDSID  fixedstreets_plus = args_run->fixedstreets[tid];
    
    int nid = fixedstreets_plus.nid;
    int sid = fixedstreets_plus.sid;
    int i = fixedstreets_plus.i;

    if(StreetSchedule[sid]   == -1){  
     if(nodesSchedule[nodesIndex[nid] + i]  == -1){  
       nodesSchedule[nodesIndex[nid] + i] = sid;
       StreetSchedule[sid] = i;
       
     }}
   
    }   
        
}   


__device__ void  print_data(int tid, CUDAVARS * args_run){

   int * nodesSchedule = &args_run->nodesSchedule[tid * args_run->size_of_nodesSchedule];
   for( int k=0;k< args_run->size_of_nodesSchedule;k++){
      if (nodesSchedule[k] != -1){
         printf("tid: %d nodesSchedule[k]: %d %d size_of_nodesSchedule:%d \n", tid, k, nodesSchedule[k], args_run->size_of_nodesSchedule);
       }
    }

}

__global__ void cuda_city_run
(
CUDAVARS * args_run ,
int size_of_threads 
)

{
     int tid = blockIdx.x * blockDim.x + threadIdx.x;
     
     if (tid < size_of_threads){
        
      init_time_vars( tid, args_run);

      if(args_run->type == 2){
          
            init_FIFG(tid, args_run);
            args_run->fixedstreets[tid].score = -1;
       }
          
       // null counter of finished cars
       int carsFinishedN = 0;
       
       for(int t = 0; t< args_run->RunningTime;t++ ){
        
            update_node_t(tid,  t,  args_run );
            update_streets_t(tid,  t,  args_run, &carsFinishedN );
        

            if(carsFinishedN == args_run->number_of_cars){break;} 
       }
       
       score(tid, args_run);
       
       
    }
       
}  













