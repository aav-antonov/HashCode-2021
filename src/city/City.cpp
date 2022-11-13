
cityC::cityC(std::string file_input){
        
        //-------------------------------------------------------------//
        //--------------- read input file: city -----------------------//
        //-------------------------------------------------------------//
        
        std::map<std::string, int> map_sname2nid1;
        std::map<std::string, int> map_sname2nid2;
        std::map<std::string, int> map_sname2L;
        std::map<int, std::vector<std::string>> map_cid2sname;
        std::vector<int> TimeBonus;
        load_file(file_input , map_sname2nid1 , map_sname2nid2, map_sname2L, map_cid2sname ,TimeBonus );
        
        
        int D = TimeBonus[0];
        int F = TimeBonus[1];
        int N = TimeBonus[2];
        int C = TimeBonus[3];

        std::cout << "Time:" << D  << " Nodes:" << N << " Cars:" << C << " CarFinishedBonus:" << F << "\n";
        
        
        RunningTime = TimeBonus[0];
        Bonus = TimeBonus[1];
        number_of_nodes = TimeBonus[2];
        number_of_cars = TimeBonus[3];

         
         // street_car_stat: counts the number of cars used the street; 
         // street_car_node_stat: counts the number of cars used the street and cross the junction next to street 
         //( only used streets needs to be scheduled , do not count last street in car path as it does not cross the junction);
 
         std::map<std::string, int> street_car_stat , street_car_node_stat;

         for( auto cid_ref :  map_cid2sname){
            int size = map_cid2sname[cid_ref.first].size();
            int street_k = 1;
            for( auto sname :  map_cid2sname[cid_ref.first]){
                street_car_stat[sname]++;
                if (street_k == size){continue;}
                street_car_node_stat[sname]++;
                street_k++;
            }
         }
        
        
        number_of_streets =  street_car_stat.size();
        streets_n2 = (int *) malloc(number_of_streets * sizeof(int));
        streets_L = (int *) malloc(number_of_streets * sizeof(int));

        // sid -> new street id (int); 
        // nid -> node id (int);
        // map_nid2sids: sids regulated at nid (), filtering out unused streets
        // map_sname2sid: street name -> sid;
        // map_sid2sname: sid -> street name;   
 
        int sid = 0;  
        for( auto sname :  street_car_stat){
            int nid1 = map_sname2nid1[sname.first];
            int nid2 = map_sname2nid2[sname.first];
            int L = map_sname2L[sname.first];

            streets_n2[sid] = nid2;
            streets_L[sid] = L;

            map_sname2sid[sname.first] = sid;
            map_sid2sname[sid] = sname.first;

            
            if (street_car_node_stat.count(sname.first)){map_nid2sids[nid2].push_back(sid);}
            sid++;
        }

      
      nodesSize = (int *) malloc(sizeof(int) * number_of_nodes);
      nodesIndex = (int *) malloc(sizeof(int) * number_of_nodes); 

      size_of_nodesSchedule = 0;   
      for (int i = 0 ; i < number_of_nodes ; i++) {
           nodesIndex[i] = size_of_nodesSchedule;
           nodesSize[i] = map_nid2sids[i].size(); 
           size_of_nodesSchedule += map_nid2sids[i].size();
           
      }         
     
     nodesSIDS = (int *) malloc(sizeof(int) * size_of_nodesSchedule);


     carsSize = (int *) malloc(sizeof(int) * number_of_cars); 
     carsIndex = (int *) malloc(sizeof(int) * number_of_cars);       

     
     size_of_carsStreet = 0;;
     for (int i = 0 ; i < number_of_cars ; i++) {
       carsSize[i] = map_cid2sname[i].size();
       carsIndex[i] = size_of_carsStreet;
       size_of_carsStreet += map_cid2sname[i].size();
     }

     carsStreet = (int *) malloc(sizeof(int) * size_of_carsStreet);  
       
     for (int i = 0 ; i < number_of_cars ; i++) {
            
           int CID = carsIndex[i];
           int s_count = 0; 
           for( auto sname :  map_cid2sname[i]){
            carsStreet[CID + s_count] = map_sname2sid[sname];
            s_count++;
           }
     } 

        
        for( auto nid_ref :  map_nid2sids){
            int index = nodesIndex[nid_ref.first];
            int size = nodesSize[nid_ref.first];
            int sizeSch = map_nid2sids[nid_ref.first].size();
            
            if (size != sizeSch ){std::cout << "cityC::add_schedule: ERROR: nid:" << nid_ref.first << " size:" << size  << " sizeSch:" << sizeSch << "\n";exit(0);}

            int s_count = 0;
            for( auto sid :  map_nid2sids[nid_ref.first]){
                nodesSIDS[index + s_count] = sid;
                s_count++;
            }
            
        }
}




//--------------------- --------- -----------------------------------//
//--------------------- cityC_run -----------------------------------//
//--------------------- --------- -----------------------------------//


//--------------------- --------- -----------------------------------//
// -- initialize nodesSchedule and StreetSchedule by external var: nid2sids_schedule  --//

void  cityC_run::fill_nodesSchedule(std::map<int, std::vector<int>> & nid2sids_schedule){
        //printf("fill_nodesSchedule: nid2sids_schedule\n");
        for( auto nid_ref :  nid2sids_schedule){
            int index = nodesIndex[nid_ref.first];
            int size = nodesSize[nid_ref.first];
            int sizeSch = nid2sids_schedule[nid_ref.first].size();
            
            //std::cout << "index:" << index << " " << size << "\n";
 
            if (size != sizeSch ){std::cout << "cityC::add_schedule: ERROR: nid:" << nid_ref.first << " size:" << size  << " sizeSch:" << sizeSch << "\n";exit(0);}
            
            int s_count = 0;
            for( auto sid :  nid2sids_schedule[nid_ref.first]){
                nodesSchedule[index + s_count] = sid;
                StreetSchedule[sid] = s_count;// added to convert schedule into fixedstreets
                s_count++;
            }
            //std::cout << "add_schedule:" << nid_ref.first << " " << sizeSch << "\n";
        }
}


//--------------------- --------- -----------------------------------//
// --  FIFG optimization -- //
// -- initialize nodesSchedule and StreetSchedule by external var:  fixed_streets --//

void  cityC_run::fill_nodesSchedule(std::vector<FIXEDSID> & fixed_streets){
        
        //printf("fill_nodesSchedule: fixed_streets %d, size_of_nodesSchedule %d, number_of_streets %d\n", (int)fixed_streets.size(), size_of_nodesSchedule, number_of_streets);
               
        for (int i = 0 ; i < size_of_nodesSchedule ; i++){nodesSchedule[i] = -1;}
        
        for (int i = 0 ; i < number_of_streets ; i++){StreetSchedule[i] = -1;}  
                
        for( auto FSID :  fixed_streets){
            int nid = FSID.nid;
            int sid = FSID.sid;
            int i = FSID.i;
            int nid_index = nodesIndex[nid];
            nodesSchedule[nid_index + i] = sid;
            StreetSchedule[sid] = i;
            
            //printf("nodesSchedule[k]: %d sid:%d i:%d \n",  nid_index + i, nodesSchedule[nid_index + i], i );            
                        
        }
     
}


//-------------------------------------------------------//
//-------------------------------------------------------//
//------------ Start of RUN (Simulation of City )---------//
//-------------------------------------------------------//
//-------------------------------------------------------//
//--------------------- --------- -----------------------//

//--------------------- --------- -----------------------------------//        
// --  set to initial state all time dependent vars for a new run -- //

void  cityC_run::init_time(){

        carsFinishedN = 0;
        for( int i=0;i< number_of_streets;i++){T_nodesStreetQ[i] = 0;}

        for(int i=0; i< number_of_cars;i++){
            
            carsFinished[i] = -1;
            int cid_index = carsIndex[i];
            int sid = carsStreet[cid_index + 0];
            
            T_state_car[i].i = 0; //count of street in car path
            T_state_car[i].sid = sid;//current street id 
            T_state_car[i].l = -1;// position on street, -1 waiting at node (junction)
            T_state_car[i].q = T_nodesStreetQ[sid];//position in quiry
            T_nodesStreetQ[sid]++;
            
        }
  
}




        
// --  the run consist of updates for every time point for streets and nodes states -- //


void  cityC_run::run(){
    
   

    for(int t = 0; t< this->RunningTime;t++ ){
        
        this->update_node_t(t);
        
        this->update_streets_t(t);

     }

}



void  cityC_run::update_streets_t(int t){ 
       
      for(int i=0; i< number_of_cars;i++){

         if(T_state_car[i].l > 0){T_state_car[i].l--;}  

         if(T_state_car[i].l == 0){

                int sid = T_state_car[i].sid;
                T_state_car[i].q = T_nodesStreetQ[sid];
                T_nodesStreetQ[sid]++;
                T_state_car[i].l--;
                //Q_street_car[sid][i] = t;
                
                // -- finished -- //
                if(T_state_car[i].i == carsSize[i]-1){
                    T_state_car[i].l = -100;
                    carsFinished[i] = t;
                    carsFinishedN++;
                    T_nodesStreetQ[sid]--;
                }
                
           }
      }  
} 


void  cityC_run::update_node_t(int t){ 
         
        

        for(int i=0; i< number_of_cars;i++){
            
           
            if (T_state_car[i].l == -1){
                
                int sid = T_state_car[i].sid;
                int nid = streets_n2[sid];
                
                int nSize = nodesSize[nid];
                int nid_index = nodesIndex[nid];
                int i_time = t  % nSize;
                int sid_green = nodesSchedule[nid_index  + i_time];

                

                 
                
                // --FIFG optimization --//
                // -- if(sid_green == -1)--> time point <i_time> is not scheduled  --//
                // -- StreetSchedule[sid] == -1)--> street  <sid> is not scheduled  --//
                // -- if both true: schedule time point to the street and let the car go  --//    

                if(sid_green == -1){
                  if(StreetSchedule[sid] == -1){  
                    nodesSchedule[nid_index  + i_time] = sid;
                    StreetSchedule[sid] = i_time;
                    sid_green = sid;
                }}

               
                // --If Mutaion are specified (mutationA->nid != -1) then test mutated (1 mutation) scheduled --//
                
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


//-------------------------------------------------------//
//-------------------------------------------------------//
//------------ END of Simulation Code--------------------//
//-------------------------------------------------------//
//-------------------------------------------------------//


//-- Run FIFG (including all initilizations) for a given set of fixed streets --- /// 
void  cityC_run::run_FIFG(std::vector<FIXEDSID> & fixed_streets){

this->fill_nodesSchedule(fixed_streets);
this->init_time();
this->run();

}

//-- Run  (including all initilizations) for a given SCHEDULE --- ///
void  cityC_run::run_SCHEDULE(std::map<int, std::vector<int>> & schedule){

this->fill_nodesSchedule(schedule);
this->init_time();
this->run();

}

//------------ Compute Score Metric-------------------------------------------//

int  cityC_run::score(){
        int score  = 0; 
        for(int i=0; i< number_of_cars;i++){
           if(carsFinished[i] > 0){
             score += RunningTime- carsFinished[i]-1;
             score += Bonus; 
            }
        }
return score; 
} 

// ----------- check correctness --------------------//
// ----------- check if conversition from schedule->fixed_streets (FIFG) is correct  --------------------//
// ----------- check correctness --------------------//
void  test_correctness(cityC_run * CRUN){

std::map<int, std::vector<int>> schedule = CRUN->print_results_to_schedule();
CRUN->run_SCHEDULE(schedule);
int score_SCHEDULE = CRUN->score();

std::vector<FIXEDSID> fixed_streetsB = CRUN->fixed_streets_from_cityC_run();
CRUN->run_FIFG(fixed_streetsB);
int score_FIFG = CRUN->score();

if(score_FIFG != score_SCHEDULE){
printf("ERROR: score_SCHEDULE != score_FIFG:  %d != %d \n", score_SCHEDULE, score_FIFG);
exit(0);
}

}
// ----------- check correctness --------------------//


//-------------------------------------------------------//
//-------------- printing schedule/fixed streets to files/schedule---//
//-------------------------------------------------------//


void  cityC_run::print_results(std::string fileOUT_solution){

            std::ofstream myfile;
            myfile.open (fileOUT_solution);
            
            int count_of_nodes = 0;
            for(int i =0;i< number_of_nodes; i++){if(nodesSize[i] > 0){count_of_nodes++;}}
  
            myfile   << count_of_nodes << "\n";
           
            for(int i =0;i< number_of_nodes; i++){
                
                if(nodesSize[i] ==0){continue;}
                myfile   << i << "\n";
                myfile   << nodesSize[i] << "\n";
                
                                
                for(int s =0;s< nodesSize[i]; s++){
                  int sid = nodesSchedule[nodesIndex[i] + s];
                  myfile   << C->map_sid2sname[sid] << " " << 1 << "\n";
                }
            }
  
  
            myfile.close();
}


std::map<int, std::vector<int>>   cityC_run::print_results_to_schedule(){
        std::map<int, std::vector<int>>  schedule;
        for(int i =0;i< number_of_nodes; i++){
                
                if(nodesSize[i] ==0){continue;}
                
                for(int s =0;s< nodesSize[i]; s++){
                  int sid = nodesSchedule[nodesIndex[i] + s];
                  schedule[i].push_back(sid);
                }
         }
  
 return   schedule;     

}

void  cityC_run::print_fixedstreets(std::string fileOUT, std::vector<FIXEDSID> & fixed_schedule){

            std::ofstream myfile;
            myfile.open (fileOUT);
            int count_FSID = 0;
            for( auto FSID :  fixed_schedule){
                int nid = FSID.nid;
                int sid = FSID.sid;
                int i = FSID.i;
                
                myfile   << nid << " " << sid << " " << i << "\n";
                count_FSID++;
            
            }
  
            myfile.close();
            //printf("cityC_run::print_fixedstreets: %d", count_FSID);
}







