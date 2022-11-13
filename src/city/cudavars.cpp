




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



