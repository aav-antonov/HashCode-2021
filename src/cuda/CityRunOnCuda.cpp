





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

     //d stands for batches if number of screened mutation exeeds GPU memory
     // this option is not used and not properly tested 
     // the number of mutations on Host is selected to be screened in one round: d == 0;

     //Copy Mutations (Prepared On Host) To Device
     //Run City with each Mutation  
     for(int dev=0; dev < number_of_gpu; dev++) {

        cudaSetDevice(dev);
        
        index = (dev * size_of_threads) + d  * (size_of_threads * number_of_gpu); 
                 
        // size_of_run - number of mutations to screen, commonly equals to size_of_threads;  
        int size_of_run = size_of_threads;
        if( index + size_of_threads >  size_of_mutations ){size_of_run = size_of_mutations - index;}

        if(size_of_run <= 0 ){break;}
        
        //Mutation Screen:  type == 1
        MUTATION * mutations_batch;
        if(args_run[dev]->type == 1){
          mutations_batch = &CRUN->mutations[args_run[dev]->mutationA_N *index];
          CHECK_CUDA_ERROR(cudaMemcpy(args_run[dev]->mutations, mutations_batch, args_run[dev]->mutationA_N * size_of_run * sizeof(MUTATION), cudaMemcpyHostToDevice));
          cuda_city_run<<<gridSize, blockSize>>>( args_run_cuda[dev] , size_of_run);
          cudaError_t err = cudaGetLastError();  
          if (err != cudaSuccess){std::cout << "cudaGetLastError():" << dev << " " << size_of_run << " " << cudaGetErrorString(err )<< "\n";} 
          
        } 
        
        //FIFG Screen:  type == 2
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
    
    //Copy Back RunTimes for each Mutation To Host
    // On host Top Mutation(s) would be selected
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







