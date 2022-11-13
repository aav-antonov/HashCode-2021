


//-------------------Memeory Alooc//Copy On Cuda ----------------------------///

int copy_args_run_to_cuda(PARAM_SCREEN & PARAM, std::vector<CUDAVARS*> &args_run, std::vector<CUDAVARS*> &args_run_cuda);

int  malloc_cuda_city_data_fixed( std::vector<CUDAVARS*> & args_run, cityC * CC, PARAM_SCREEN & PARAM );
int  free_cuda_city_data_fixed( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM );

int  malloc_cuda_city_data_TIME( std::vector<CUDAVARS*> & args_run, cityC * CC,  PARAM_SCREEN & PARAM );
int  free_cuda_city_data_TIME( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM );

int  malloc_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run,   cityC_run * CRUN, PARAM_SCREEN & PARAM );
int  copy_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run,  cityC_run * CRUN, PARAM_SCREEN & PARAM );
int  free_cuda_city_data_nodesSchedule( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM );

int  malloc_cuda_city_data_MUTATION( std::vector<CUDAVARS*> & args_run,  cityC_run * CC, PARAM_SCREEN & PARAM );
int  free_cuda_city_data_MUTATION( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM );

int  copy_cuda_city_data_fixedstreets_current( std::vector<FIXEDSID> & fixed_streets, std::vector<CUDAVARS*> & args_run,  cityC_run * CRUN, PARAM_SCREEN & PARAM  );

int  malloc_cuda_city_data_FIFG( std::vector<CUDAVARS*> & args_run,  cityC_run * CC, PARAM_SCREEN & PARAM );
int  free_cuda_city_data_FIFG( std::vector<CUDAVARS*> & args_run, PARAM_SCREEN & PARAM );

//-------------------Memeory Alooc//Copy On Cuda ----------------------------///


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









