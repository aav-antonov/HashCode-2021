


// Code to convert input schedule into fixed street for FIFO optimization
// Input: Schedule (*nodesSchedule) -> Output: std::vector<FIXEDSID>  fixed_streets 
// --  FIFO optimization -- //
// -- initialize nodesSchedule and StreetSchedule  --//

void  cityC_run::fill_nodesScheduleB(){

        printf("fill_nodesScheduleB(): %d %d \n", size_of_nodesSchedule, number_of_streets );         

        for (int i = 0 ; i < size_of_nodesSchedule ; i++) { 
            nodesScheduleB[i] = -1;
        }
        for (int i = 0 ; i < number_of_streets ; i++) { 
            StreetScheduleB[i] = -1;
        }  
        
}
// -- -------------------------------------------  --//




void  cityC_run::update_node_t_B(int t, std::vector<FIXEDSID> & fixed_streets){ 
        
        //printf("update_node_t_B car: %d \n", t );
            
        for(int i=0; i< number_of_cars;i++){
    
            //printf("update_node_t_B car: %d T_state_car[i].l %d\n", i , T_state_car[i].l);
            if (T_state_car[i].l == -1){
                
                //printf("update_node_t_B car: %d \n", i );
                int sid = T_state_car[i].sid;
                int nid = streets_n2[sid];
                
                int nSize = nodesSize[nid];
                int nid_index = nodesIndex[nid];
                int i_time = t  % nSize;
                int sid_green = nodesSchedule[nid_index  + i_time];
                //printf("HERE 2\n" );
                int sid_greenB = nodesScheduleB[nid_index  + i_time];
                
                //printf("sid_green: %d  sid_greenB %d \n", sid_green, sid_greenB );

                // --FIFG optimization --//

                if(sid_greenB == -1){
                  if(StreetScheduleB[sid] == -1){  
                    
                    //printf("sid_greenB: %d %d \n", sid_greenB, StreetScheduleB[sid] );

                    if(sid_green == sid){
                        nodesScheduleB[nid_index  + i_time] = -2;
                        StreetScheduleB[sid] = -2;

                        //int i_fixed_time = StreetSchedule[sid];
                        //if(i_fixed_time != i_time){
                        // printf("i_fixed_time: %d  i_time %d \n", i_fixed_time, i_time );
                        // exit(1);
                        //}

                    }
                    else{
                        
                        int i_sid_fix = StreetSchedule[sid];
                        FIXEDSID f = {.nid = nid, .sid = sid, .i = i_sid_fix };
                        fixed_streets.push_back(f); 
                        
                        nodesScheduleB[nid_index  + i_sid_fix] = -2;//maybe ERROR ??!!!??!! check
                        StreetScheduleB[sid] = -2;
                        //printf("fixed_streets.push_back car %d i_time %d sid %d nid %d sid_green %d %d \n", i, i_time , sid, nid, sid_green, sid_greenB);   
                    }
                    
                    
                }}

                // -- ---------------- --//
                
                if(mutationA->nid == nid){
                    if(mutationA->i1 == i_time){ sid_green = nodesSchedule[nid_index  + mutationA->i2];}
                    if(mutationA->i2 == i_time){ sid_green = nodesSchedule[nid_index  + mutationA->i1];}
                }            
                
            
            int cid_index = carsIndex[i];     
            if (sid == sid_green){
                if (T_state_car[i].q == 0){
                    
                    T_state_car[i].i++;
                    T_state_car[i].sid = carsStreet[cid_index + T_state_car[i].i]; ;
                    T_state_car[i].l = streets_L[T_state_car[i].sid];
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

std::vector<FIXEDSID>  cityC_run::fixed_streets_from_cityC_run(){


    std::cout << "run_SCHEDULE score!!!!!!:" << this->score()  << "\n";

    std::vector<FIXEDSID>  fixed_streets;

    this->fill_nodesScheduleB();
    this->init_time();
    

    for(int t = 0; t< this->RunningTime;t++ ){
        this->update_node_t_B(t,fixed_streets);
        this->update_streets_t(t);
     }

    std::cout << "run_B score:" << this->score() << " fixed_streets.size():" << fixed_streets.size() << "\n";
     
    this->run_FIFG(fixed_streets);
    printf("this->run_FIFG(fixed_streets)): %d\n", this->score());

    
    
    //--- check for duplicate, only for ERROR control
    std::map<std::string,int> filtr;
    for(int f = 0; f< fixed_streets.size();f++ ){
        int nid = fixed_streets[f].nid;
        int sid = fixed_streets[f].sid;
        std::string s = std::to_string(nid) + " " + std::to_string(sid);
        filtr[s] +=1;
        if(filtr[s]  > 1){
        std::cout << "std::vector<FIXEDSID>  cityC_run::fixed_streets_from_cityC_run() duplicate:" << s << " " << filtr[s] << "\n";
        exit(1);
        }
    }


return fixed_streets;
}





