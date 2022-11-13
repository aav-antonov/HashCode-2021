



// Check screened set of mutation (T * items == either mutations or fixedstreets )
// Count positive mutation : Score has increased
// Count nonenegative mutation : Score did not changed
// For  nonenegative mutations record all inedexes (to introduce them further  in blocks, in case no positive mutation is found ) 


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



