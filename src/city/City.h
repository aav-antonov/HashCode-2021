/*
 * City.h
 *
 *  Created on: Aug 23, 2022
 *      Author: aav
 */


#ifndef CITY_H_
#define CITY_H_


// Abreviations:
// sid -> street id (int)
// nid -> node (junction) id (int)

class cityC
{
    private:
    
    public:
    int RunningTime, Bonus;
    int number_of_cars, number_of_streets, number_of_nodes , size_of_nodesSchedule; 
    
    // int arrays for street info, sid (StreetID) -> int street id , the count of street in the input file:
    // streetsNode -> to find by sid position of street in nodesStreet array (which is schedule)
    int *streets_n2, *streets_L;

    // nodes_schedule -> traffic schedule , nodes_schedule[nid] -> array of sids scheduled at the node, order corresponds to schedule
    int size_of_carsStreet; 
    int *nodesSize, *nodesIndex, *nodesSIDS;
    int *carsSize , *carsIndex,  *carsStreet;
    
    // mapping info , only for info
    std::map<std::string, int> map_sname2sid ;
    std::map<int, std::string> map_sid2sname ;
    std::map<int, std::vector<int>> map_nid2sids ;
    std::map<int, int> map_nid2nidC ;
    
    

    void  fill_nodesSIDS();

    void  init(
                std::map<std::string, int> &map_sname2nid1, 
                std::map<std::string, int> &map_sname2nid2, 
                std::map<std::string, int> & map_sname2L, 
                std::map<int, std::vector<std::string>> & map_cid2sname,
                std::vector<int> TimeBonus
    );

    cityC(){};
    cityC(std::string file_input);

     ~cityC(){
      
      free(streets_n2); 
      free(streets_L);     
      free(nodesSize); 
      free(carsSize); 
      free(carsStreet); 
     }
};




#endif

