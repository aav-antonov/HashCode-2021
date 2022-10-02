

void read_config_file(std::string file_config, PARAM_SCREEN & PARAM){

std::string param_name;
std::string param_value;

std::ifstream FIN(file_config);

while ( FIN >> param_name >> param_value )
{ 
       if(param_name == "size_of_threads_GPU"){PARAM.size_of_threads_GPU = std::stoi( param_value ); }
  else if(param_name == "stop_criteria_per_1000_optimization_rounds"){PARAM.stop_criteria_per_1000_optimization_rounds = std::stoi( param_value); }
  else if(param_name == "fixedstreets_current_MAXSIZE"){PARAM.fixedstreets_current_MAXSIZE = std::stoi( param_value); }
  else if(param_name == "initial_rounds"){PARAM.initial_rounds = std::stoi( param_value); }   
  else if(param_name == "clean_round_FIFG"){PARAM.clean_round_FIFG = std::stoi( param_value); }
  else if(param_name == "initial_rounds_score_diff_MIN"){PARAM.initial_rounds_score_diff_MIN = std::stoi( param_value); }
  else if(param_name == "initial_rounds_count_stagnation_MAX"){PARAM.initial_rounds_count_stagnation_MAX = std::stoi( param_value); } 
  else if(param_name == "dir_results"){PARAM.dir_results = param_value; } 
     else{std::cout << "unknown parameter\n";}   
}

FIN.close(); 

}



void load_file
(
std::string fileA,
std::map<std::string, int> & map_sname2nid1,
std::map<std::string, int> & map_sname2nid2,
std::map<std::string, int> & map_sname2L,
std::map<int, std::vector<std::string>> & map_cid2sname,
std::vector<int> & TP
){
int count_line = 0; 
int count_i = 0;
int cid = 0;



int T , N, S, C, P;

std::ifstream F(fileA);
if (F.is_open()) {
    
    std::string line;
    while (std::getline(F, line)) {
        
        if (boost::starts_with(line, "EOF")){break;} 
        
        if(count_line == 0){     
        
         std::vector<std::string> strs;
         boost::split(strs, line, boost::is_any_of("\t "));
         
          T = std::stoi( strs[0] );
          N = std::stoi( strs[1] );
          S = std::stoi( strs[2] );
          C = std::stoi( strs[3] );
          P = std::stoi( strs[4] );
          std::cout << "Streets!!!!!!!!!!!!!!!!! " << S << "\n";
          TP.push_back(T);
          TP.push_back(P);
          TP.push_back(N);
          TP.push_back(C);   
        
        }
       
      if(count_line > 0){ 
       
       //std::cout << line << "\n";
       std::vector<std::string> strs;
       boost::split(strs, line, boost::is_any_of("\t "));  
      
       if(count_i < S){
         int nid1 = std::stoi( strs[0] );
         int nid2 = std::stoi( strs[1] );
         std::string name = strs[2];
         int L = std::stoi(strs[3]);
         
         map_sname2nid1[name] = nid1;
         map_sname2nid2[name] = nid2;
         map_sname2L[name] = L;
         
         
         count_i++;
         
       }else{ 
         
              
         int l = std::stoi( strs[0] );
         std::vector<std::string> strscid;
         for(int i = 0; i< l;i++){strscid.push_back(strs[i+1]);}
         map_cid2sname[cid] = strscid;
         cid++;
       }
      } 
       
       count_line++; 
     }
    F.close();
    
    std::cout << C << " "  << cid << "\n";
    std::cout << "map_cid2sname.size() " << map_cid2sname.size() << "\n";
}

}


void load_file_schedule
(
std::string fileA,
std::map<std::string, int> & map_sname2sid, 
std::map<int, std::vector<int>> & map_nid2sids 
)
{


std::ifstream F(fileA);
if (F.is_open()) {
    int line_count =0;
    int size = 0;
    int nid = -1;
    int node_SIZE = -1;
    std::string line;
    while (std::getline(F, line)) {
        //std::cout << "line_count: " << line_count << " nid: " << nid << "\n";
        if (boost::starts_with(line, "EOF")){break;} 
        if (line_count == 0){line_count++;size = 0;continue;}
        if (node_SIZE == 1 ){size = std::stoi( line );node_SIZE=-1;continue;}
        if (size == 0 ){nid = std::stoi( line );node_SIZE=1;continue;}
         //std::cout << "line: " << line << " size:" << size << " node_SIZE:" << node_SIZE << "\n";           
         
         size--;
         std::vector<std::string> strs;
         boost::split(strs, line, boost::is_any_of("\t "));
         std::string sname = strs[0] ;
         int sid =  map_sname2sid[sname];
         //std::cout << "map: " << sid << " " << sname << "\n";   

         map_nid2sids[nid].push_back(sid);
              
                  
        }
    }

std::cout << "load_file_schedule: " << map_nid2sids.size() << "\n";
//exit(0);
}


void load_file_fixedstreets
(
std::string fileA,
std::vector<FIXEDSID> & fixed_streets
)
{


std::ifstream F(fileA);
if (F.is_open()) {
        
    std::string line;
    while (std::getline(F, line)) {
        //std::cout << "line_count: " << line_count << " nid: " << nid << "\n";
        if (boost::starts_with(line, "EOF")){break;} 
        
        
         std::vector<std::string> strs;
         boost::split(strs, line, boost::is_any_of("\t "));
         
         int nid = std::stoi( strs[0] );
         int sid = std::stoi( strs[1] );
         int i = std::stoi( strs[2] ); 
         FIXEDSID  F = FIXEDSID();
         F.nid = nid;
         F.sid = sid;
         F.i = i;     
         fixed_streets.push_back(F);         
        }
    }

std::cout << "load_file_fixedstreets loaded : " << fixed_streets.size()  << " fixed_streets\n";
//exit(0);
}


