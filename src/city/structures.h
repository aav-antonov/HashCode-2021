/*
 * structures.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */

#ifndef STRUCTURES_H_
#define STRUCTURES_H_



//------------- MUTATION ----------------------------//
// MUTATION - specify mutation in schedule i1->i2

typedef struct
{
    int nid;        // nid - node id, i1,i2 - permutational indexs in  *nodesSchedule
    int  i1;        // sid1 = nodesSchedule[nodesIndex[nid] + i1]
    int  i2;        // sid2 = nodesSchedule[nodesIndex[nid] + i2]
    int criteria;    
    int score;      // sid2 = nodesSchedule[nodesIndex[nid] + i2]

} MUTATION;
//------------END MUTATION    ------------//


//--------------- CAR --------------------------//
// typedef struct CAR - state of the car at current time: 

typedef struct
{
    int i;    // i - index of current street
    int sid;  // sid - id of current street
    int l;    // l - distnace to junction, -1 -> at junction
    int q;    // q - junction query (q = 0 -> first, 1 -> second, ..., -1 -> othervice)
} CAR;

//------------END CAR    ------------//


//---------------- CUDAVARS -------------------------//
// typedef struct CUDAVARS: 
// pointers to all city variables at GPU:
// to copy to cuda
// to pass to cuda __global__ or __device__ functions  
//-----------------------------------------//

typedef struct
{   
    int type; // 1- MUTATION; 2- FIXEDSID( FIFG);
    int cuda_device; // id of cuda device;
    int size_of_threads; // set to be equal to PARAM.size_of_threads_GPU -> the number of tested mutation/fixedstreets per GPU
    int clean_FIFG = -1; // 1- clean; -1 standard FIFG;  
    int RunningTime, Bonus;
    
    int size_of_nodesSchedule, number_of_cars, number_of_streets;
    int * streets_n2; // size=number_of_streets
    int * streets_L;  // size=number_of_streets
    int * nodesSize;  // size=number_of_nodes
    int * nodesIndex; // size=number_of_nodes
    int * nodesSchedule; // size_of_nodesSchedule : used for mutation screen; OR size_of_nodesSchedule * size_of_Threads;  used for FIFG (first in first green) screen
    
    int * carsSize; // number_of_cars
    int * carsIndex; // size=number_of_cars
    int * carsStreet; // size= size_of_carsStreet = number_of_cars * size_of_paths
    

    int * T_nodesStreetQ; //size=number_of_streets * size_of_Threads
    CAR * T_state_car;    //size=number_of_cars * size_of_Threads
    int * carsFinished;   //size=number_of_cars *  size_of_Threads 
    
    int fixedstreets_current_size = 0;
    FIXEDSID * fixedstreets_current; // 50k - assumed that the number of prefixed street will not exeed 10k 
    FIXEDSID * fixedstreets; // size_of_Threads
     
    int mutationA_N = 1; 
    MUTATION * mutations; // size_of_Threads * mutationA_N ,  mutationA_N -> number of mutions in one test (commonly mutationA_N =1, mutationA_N = 2 is not used anymore)
    
    
    int * StreetSchedule; //size streets * size_of_Threads;  used for FIFG (first in first green) screen
       

} CUDAVARS;

//------------END CUDAVARS  ------------//


#endif /* STRUCTURES_H_ */





