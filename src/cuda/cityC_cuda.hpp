
#define CUDA_CALL(x) do { if((x)!=cudaSuccess) { printf("Error at %s:%d\n",__FILE__,__LINE__); return EXIT_FAILURE;}} while(0)


#define CHECK_CUDA_ERROR(val) check((val), #val, __FILE__, __LINE__)
template <typename T>
void check(T err, const char* const func, const char* const file,const int line)
{
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line << std::endl;
        std::cerr << cudaGetErrorString(err) << " " << func << std::endl;
        // We don't exit when we encounter CUDA errors in this example.
        std::exit(EXIT_FAILURE);
    }
}

#define CHECK_LAST_CUDA_ERROR() checkLast(__FILE__, __LINE__)
void checkLast(const char* const file, const int line)
{
    cudaError_t err{cudaGetLastError()};
    if (err != cudaSuccess)
    {
        std::cerr << "CUDA Runtime Error at: " << file << ":" << line
                  << std::endl;
        std::cerr << cudaGetErrorString(err) << std::endl;
        // We don't exit when we encounter CUDA errors in this example.
        // std::exit(EXIT_FAILURE);
    }
}





__global__ void print_cuda_data(int * var)

{
     int tid = blockIdx.x * blockDim.x + threadIdx.x;
     if (tid < 100){printf("var:  %d \n",  var[tid] );}
}  

int copy_args_run_to_cuda(PARAM_SCREEN & PARAM, std::vector<CUDAVARS*> &args_run, std::vector<CUDAVARS*> &args_run_cuda){

    for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
        cudaSetDevice(dev);
        CUDAVARS * args_run_cuda_dev;
        CHECK_CUDA_ERROR(cudaMalloc( &args_run_cuda_dev , sizeof(CUDAVARS) ));
        CHECK_CUDA_ERROR(cudaMemcpy(args_run_cuda_dev, args_run[dev], sizeof(CUDAVARS), cudaMemcpyHostToDevice));
        args_run_cuda[dev] = args_run_cuda_dev;
    }
return EXIT_SUCCESS;
}

void print_args_run_param(std::vector<CUDAVARS*> & args_run){
 CUDAVARS*args = args_run[0];
 printf("size_of_nodesSchedule:  %d\n",  args->size_of_nodesSchedule );
 printf("number_of_cars:  %d\n",  args->number_of_cars );
 printf("number_of_streets:  %d\n",  args->number_of_streets );
 printf("type:  %d\n",  args->type );
 printf("mutationA_N:  %d\n",  args->mutationA_N );
 printf("fixedstreets_current_size:  %d\n",  args->fixedstreets_current_size );
 printf("------------------------\n");
}


int  malloc_cuda_city_data_fixed( std::vector<CUDAVARS*> & args_run, cityC * CC, PARAM_SCREEN & PARAM )
{

// allocate the memory on each GPU and copy fixed data

printf("malloc_cuda_city_data_fixed number_of_gpu:%d\n", PARAM.number_of_gpu);

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {

    cudaSetDevice(dev);

    int * streets_n2;
    CHECK_CUDA_ERROR(cudaMalloc( &streets_n2 , CC->number_of_streets * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(streets_n2, CC->streets_n2, CC->number_of_streets * sizeof(int), cudaMemcpyHostToDevice));

    int * streets_L;
    CHECK_CUDA_ERROR(cudaMalloc( &streets_L , CC->number_of_streets * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(streets_L, CC->streets_L, CC->number_of_streets * sizeof(int), cudaMemcpyHostToDevice));

    int * nodesSize;
    CHECK_CUDA_ERROR(cudaMalloc( &nodesSize , CC->number_of_nodes * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(nodesSize, CC->nodesSize, CC->number_of_nodes * sizeof(int), cudaMemcpyHostToDevice));

    int * nodesIndex;
    CHECK_CUDA_ERROR(cudaMalloc( &nodesIndex , CC->number_of_nodes * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(nodesIndex, CC->nodesIndex, CC->number_of_nodes * sizeof(int), cudaMemcpyHostToDevice));

    int * carsSize;
    CHECK_CUDA_ERROR(cudaMalloc( &carsSize , CC->number_of_cars * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(carsSize, CC->carsSize, CC->number_of_cars * sizeof(int), cudaMemcpyHostToDevice));

    int * carsIndex;
    CHECK_CUDA_ERROR(cudaMalloc( &carsIndex , CC->number_of_cars * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(carsIndex, CC->carsIndex, CC->number_of_cars * sizeof(int), cudaMemcpyHostToDevice));

    int * carsStreet;
    CHECK_CUDA_ERROR(cudaMalloc( &carsStreet , CC->size_of_carsStreet * sizeof(int) ));
    CHECK_CUDA_ERROR(cudaMemcpy(carsStreet, CC->carsStreet, CC->size_of_carsStreet * sizeof(int), cudaMemcpyHostToDevice));

    CUDAVARS * args =  new CUDAVARS();

    args->number_of_cars = CC->number_of_cars;
    args->number_of_streets = CC->number_of_streets;
    args->RunningTime  = CC->RunningTime; 
    args->Bonus  = CC->Bonus;
    args->size_of_nodesSchedule  = CC->size_of_nodesSchedule;
 
    args->streets_n2 = streets_n2;
    args->streets_L = streets_L;
    args->nodesSize = nodesSize;
    args->nodesIndex = nodesIndex;
    args->carsSize = carsSize;
    args->carsIndex = carsIndex;
    args->carsStreet = carsStreet;

    

    args_run[dev] = args;
}
return EXIT_SUCCESS;
}

int  free_cuda_city_data_fixed( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM )
{

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);

    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->streets_n2));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->streets_L));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->nodesSize ));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->nodesIndex ));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->carsSize ));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->carsIndex ));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->carsStreet ));

    
}


return EXIT_SUCCESS;
}

int  malloc_cuda_city_data_TIME( std::vector<CUDAVARS*> & args_run, cityC * CC,  PARAM_SCREEN & PARAM )
{

// allocate the memory on each GPU for TIME data

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {

    cudaSetDevice(dev);

    int * T_nodesStreetQ;
    CHECK_CUDA_ERROR(cudaMalloc( &T_nodesStreetQ , PARAM.size_of_threads_GPU * CC->number_of_streets * sizeof(int) ));

    CAR * T_state_car; 
    CHECK_CUDA_ERROR(cudaMalloc( &T_state_car , PARAM.size_of_threads_GPU * CC->number_of_cars * sizeof(CAR) ));

    int * carsFinished;
    CHECK_CUDA_ERROR(cudaMalloc( &carsFinished , PARAM.size_of_threads_GPU * CC->number_of_cars * sizeof(int) ));

        

    args_run[dev]->T_nodesStreetQ = T_nodesStreetQ;
    args_run[dev]->T_state_car = T_state_car; 
    args_run[dev]->carsFinished = carsFinished;
    
}
return EXIT_SUCCESS;
}

int  free_cuda_city_data_TIME( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM )
{

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);

    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->T_nodesStreetQ));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->T_state_car));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->carsFinished ));
}
return EXIT_SUCCESS;
}

//-------------------------------------------------------//

int  malloc_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run,   cityC_run * CRUN, PARAM_SCREEN & PARAM )
{
// allocate the memory on each GPU and copy fixed data

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {

    cudaSetDevice(dev);
    
    int * nodesSchedule;
    CHECK_CUDA_ERROR(cudaMalloc( &nodesSchedule , PARAM.size_of_threads_GPU * CRUN->size_of_nodesSchedule * sizeof(int) ));
    args_run[dev]->nodesSchedule = nodesSchedule;
    //print_cuda_data<<<10,10>>>(nodesSchedule);
    
    
}
return EXIT_SUCCESS;
}

int  copy_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run,  cityC_run * CRUN, PARAM_SCREEN & PARAM )
{
// copy (only MUTATIONS) on each GPU current CRUN->nodesSchedule 

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);
    CHECK_CUDA_ERROR(cudaMemcpy(args_run[dev]->nodesSchedule, CRUN->nodesSchedule, CRUN->size_of_nodesSchedule * sizeof(int), cudaMemcpyHostToDevice));
}
return EXIT_SUCCESS;
}




int  free_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM )
{

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->nodesSchedule));
}
return EXIT_SUCCESS;
}

//------------------------------------------------------//
//------------------------------------------------------//

int  malloc_cuda_city_data_MUTATION( std::vector<CUDAVARS*> & args_run,  cityC_run * CC, PARAM_SCREEN & PARAM )
{

// allocate the memory on each GPU and copy MUTATION data

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {

    cudaSetDevice(dev);

    MUTATION * mutations;
    CHECK_CUDA_ERROR(cudaMalloc( &mutations , PARAM.size_of_threads_GPU * PARAM.mutationA_N *sizeof(MUTATION) ));

    args_run[dev]->mutations = mutations;
    
    
}
return EXIT_SUCCESS;
}


int  free_cuda_city_data_MUTATION( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM )
{

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->mutations));
}
return EXIT_SUCCESS;
}

//------------------------------------------------------//
//------------------------------------------------------//
int  copy_cuda_city_data_fixedstreets_current( std::vector<FIXEDSID> & fixed_streets, std::vector<CUDAVARS*> & args_run,  cityC_run * CRUN, PARAM_SCREEN & PARAM  )
{
// copy (only MUTATIONS) on each GPU current CRUN->nodesSchedule 

//std::vector<FIXEDSID>::iterator it = fixed_streets.begin();
FIXEDSID *src = &(fixed_streets[0]);
for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);
    
    
    //int size = args_run[dev]->fixedstreets_current_size;
    int size = fixed_streets.size();
    //printf("copy_cuda_city_data_fixedstreets_current size: %d \n", size);
    CHECK_CUDA_ERROR(cudaMemcpy(args_run[dev]->fixedstreets_current, src, size * sizeof(FIXEDSID), cudaMemcpyHostToDevice));
    args_run[dev]->fixedstreets_current_size = fixed_streets.size();

    
}
return EXIT_SUCCESS;
}


int  malloc_cuda_city_data_FIFG( std::vector<CUDAVARS*> & args_run,  cityC_run * CC, PARAM_SCREEN & PARAM )
{

// allocate the memory on each GPU and copy fixed data



for(int dev=0; dev < PARAM.number_of_gpu; dev++) {

    cudaSetDevice(dev);

    int * StreetSchedule;
    CHECK_CUDA_ERROR(cudaMalloc( &StreetSchedule ,  PARAM.size_of_threads_GPU*CC->number_of_streets * sizeof(int) ));
    
    FIXEDSID * fixedstreets;
    CHECK_CUDA_ERROR(cudaMalloc( &fixedstreets ,  PARAM.size_of_threads_GPU*sizeof(FIXEDSID) ));

    FIXEDSID * fixedstreets_current;
    CHECK_CUDA_ERROR(cudaMalloc( &fixedstreets_current , PARAM.fixedstreets_current_MAXSIZE  * sizeof(FIXEDSID) ));
    
    args_run[dev]->StreetSchedule = StreetSchedule;
    args_run[dev]->fixedstreets = fixedstreets;
    args_run[dev]->fixedstreets_current = fixedstreets_current;

    
}
return EXIT_SUCCESS;
}



int  free_cuda_city_data_FIFG( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM )
{

for(int dev=0; dev < PARAM.number_of_gpu; dev++) {
    cudaSetDevice(dev);

    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->StreetSchedule));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->fixedstreets));
    CHECK_CUDA_ERROR(cudaFree(args_run[dev]->fixedstreets_current));
   
}
return EXIT_SUCCESS;
}


//------------------------------------------------------//
//------------------------------------------------------//
//------------------------------------------------------//
//------------------------------------------------------//
//------------------------------------------------------//
//------------------------------------------------------//



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





int  city_run_on_cuda
(
    
    PARAM_SCREEN & PARAM,
    cityC * CC, 
    cityC_run * CRUN, 
    std::vector<CUDAVARS*> & args_run, 
    std::vector<CUDAVARS*> & args_run_cuda
)
{

int number_of_gpu =  PARAM.number_of_gpu;
int size_of_threads = PARAM.size_of_threads_GPU; 
int size_of_mutations = PARAM.size_of_mutations;


int blockSize = 32;
int gridSize = (size_of_threads / blockSize) +blockSize;
printf("gridSize blockSize %d %d\n", gridSize, blockSize); 

int index = 0;
for (int d = 0; d< 100;d++){ 
    
    for(int dev=0; dev < number_of_gpu; dev++) {

        cudaSetDevice(dev);
        
        index = (dev * size_of_threads) + d  * (size_of_threads * number_of_gpu); 
                 
        // size_of_run - number of mutations to screen, commonly equals to size_of_threads;  
        int size_of_run = size_of_threads;
        if( index + size_of_threads >  size_of_mutations ){size_of_run = size_of_mutations - index;}

        if(size_of_run <= 0 ){break;}
        
         
        MUTATION * mutations_batch;
        if(args_run[dev]->type == 1){
          mutations_batch = &CRUN->mutations[args_run[dev]->mutationA_N *index];
          CHECK_CUDA_ERROR(cudaMemcpy(args_run[dev]->mutations, mutations_batch, args_run[dev]->mutationA_N * size_of_run * sizeof(MUTATION), cudaMemcpyHostToDevice));
          cuda_city_run<<<gridSize, blockSize>>>( args_run_cuda[dev] , size_of_run);
          cudaError_t err = cudaGetLastError();  
          if (err != cudaSuccess){std::cout << "cudaGetLastError():" << dev << " " << size_of_run << " " << cudaGetErrorString(err )<< "\n";} 
          
        } 
        
        FIXEDSID * fixedstreets_batch;
        if(args_run[dev]->type == 2){
          fixedstreets_batch = &CRUN->fixedstreets[index];
          CHECK_CUDA_ERROR(cudaMemcpy(args_run[dev]->fixedstreets, fixedstreets_batch, size_of_run * sizeof(FIXEDSID), cudaMemcpyHostToDevice));
          cuda_city_run<<<gridSize, blockSize>>>( args_run_cuda[dev] , size_of_run);
          cudaError_t err = cudaGetLastError();  
          if (err != cudaSuccess){std::cout << "cudaGetLastError():" << dev << " " << size_of_run << " " << cudaGetErrorString(err )<< "\n";} 
            
        }

                  
        if( index + size_of_threads >=  size_of_mutations ){break;} 
     
    }

    cudaDeviceSynchronize();
    
    
    index = 0;
    for(int dev=0; dev < number_of_gpu; dev++) {

        cudaSetDevice(dev);

               
        index = (dev * size_of_threads) + d  * (size_of_threads * number_of_gpu);
        int size_of_run = size_of_threads;
        if( index + size_of_threads >  size_of_mutations ){size_of_run = size_of_mutations - index;}
        
        if(size_of_run <= 0 ){break;}
        
         
        MUTATION * mutations_batch;
        if(args_run[dev]->type == 1){
          mutations_batch = &CRUN->mutations[args_run[dev]->mutationA_N *index];
          CHECK_CUDA_ERROR(cudaMemcpy(mutations_batch, args_run[dev]->mutations, args_run[dev]->mutationA_N * size_of_run * sizeof(MUTATION), cudaMemcpyDeviceToHost));
        }
      
        FIXEDSID * fixedstreets_batch;
        if(args_run[dev]->type == 2){
          fixedstreets_batch = &CRUN->fixedstreets[index];
          CHECK_CUDA_ERROR(cudaMemcpy(fixedstreets_batch, args_run[dev]->fixedstreets, size_of_run * sizeof(FIXEDSID), cudaMemcpyDeviceToHost)); 
        }
         
        //printf("HERE2   %d\n", index + size_of_run);
        if( index + size_of_threads >=  size_of_mutations ){break;}
         
    }
    
    cudaDeviceSynchronize();
    
    
    if( index + size_of_threads >=  size_of_mutations ){break;}

   

}


return EXIT_SUCCESS;
}







