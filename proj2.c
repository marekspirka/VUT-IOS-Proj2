/**
*@author Marek Spirka (xspirk01)
*@brief VUT FIT IOS
*@date 2022-04-29
*/

#include "proj2.h"

int main(int argc, char *argv[]){
    //argument control
    if (arguments_checker(argc, argv)){
        fprintf(stderr, "Wrong number of arguments or their correctness! \n");
        exit(1);
    }
    //open the proj2.out file for printing output, if fails stop programm
    output = fopen("proj2.out", "w");
    if (output == NULL){
        fprintf(stderr, "Can't open the output proj2.out. \n");
        fclose(output);
        exit(1);
    }

    //creating semaphores, allocating shared memory, and setting variables to default values
    if (init()){
        clean_all();
        fclose(output);
        exit(1);
    }
    
    //creating a process for oxygen
    for (int i = 1; i <= NO; i++){
        variables.type = 'O';
        variables.ID = i;

        pid_t pid = fork();
        if (pid == 0){
            oxyProcess(&variables);
            if (++(*num_of_proccess) == NO+NH) 
                sem_post(sem_ko);
            return 0; 
        }
    }

    //creating a process for hydrogen
    for (int i = 1; i <= NH; i++){
        variables.type = 'H';
        variables.ID = i;

        pid_t pid = fork();
        if (pid == 0){
            hydroProcess(&variables);
            if (++(*num_of_proccess) == NO+NH) 
                sem_post(sem_ko);
            return 0;
        }
    }
    
    sem_wait(sem_ko); 
    fclose(output);
    clean_all();

    return 0;
}

/**
*@brief function controls the number of arguments and the range of arguments
*/
int arguments_checker(int argc, char *argv[]){
    
    long long int tmp;
    char *wrong;
    //checking number of arguments /.proj2 NO NH TI TB
    if(argc != 5){
        return 1;
    }
    //checking the first argument, NO > 0
    tmp = strtol(argv[1], &wrong, 10);
    if(tmp <= 0 || wrong[0] != '\0'){
        return 1;
    }else {
        NO = tmp;
    }
    //checking the second argument, NH > 0
    tmp = strtol(argv[2], &wrong, 10);
    if(tmp <= 0 || wrong[0] != '\0'){
        return 1;
    }else {
        NH = tmp;
    }
    //checking the third argument, 0<=TI<=1000
    tmp = strtol(argv[3], &wrong, 10);
    if((tmp < 0 || tmp > 1000) || wrong[0] != '\0'){
        return 1;
    }else {
        TI = tmp;
    }
    //checking the fourth argument, 0<=TB<=1000
    tmp = strtol(argv[4], &wrong, 10);
    if((tmp < 0 || tmp > 1000) || wrong[0] != '\0'){
        return 1;
    }else {
        TB = tmp;
    }
    return 0;
}

/**
*@brief creation of predefined semaphores, mapping of shared variables, if the creation of semaphores fails, we will end the program
*/

int init(){

    //deleting semaphores if they are the same as the ones we use
    clean_all();

    MAPPING(count_line)
    MAPPING(num_of_molecule)
    MAPPING(counter_for_hydro)
    MAPPING(num_of_oxy)
    MAPPING(num_of_hydro)
    MAPPING(num_of_proccess)

    srand(time(NULL));

    *count_line = 1;      
    *num_of_molecule = 0;
    *counter_for_hydro = 0;
    *num_of_proccess = 0;
    *num_of_hydro = NH;
    *num_of_oxy = NO; 

    bool error_sem = false;

    if ((sem_oxygen = sem_open(SEM_OXYGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    error_sem = true;
    if ((sem_hydrogen = sem_open(SEM_HYDROGEN, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    error_sem = true;
    if ((sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    error_sem = true;
    if ((sem_molecule = sem_open(SEM_MOLECULE, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    error_sem = true;
    if ((sem_next_molecule = sem_open(SEM_NEXT_MOLECULE, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
    error_sem = true;
    if ((sem_ko = sem_open(SEM_KO, O_CREAT | O_EXCL, 0666, 0)) == SEM_FAILED)
    error_sem = true;
     if ((sem_barrier = sem_open(SEM_BARRIER, O_CREAT | O_EXCL, 0666, 2 )) == SEM_FAILED)
    error_sem = true;
   
    if(error_sem == true){
        printf("Error with semaphores!!!!\n"); 
        exit(1);
    }
    return 0;
}

/**
*@brief oxygen function
*@param variables we take from the structure the type and ID of the element that we use in printing 
*/

void oxyProcess(variables_t* variables){
  
    print_output(1, count_line, variables->ID, 0, variables); 
  
    usleep(1000*(rand()% (TI + 1)));

    print_output(2, count_line, variables->ID, 0, variables); 

       
    sem_wait(sem_oxygen); 

     if ((*num_of_oxy == 1 && *num_of_hydro == 1) || (*num_of_hydro < 2)){
        print_output(6, count_line, variables->ID, 0, variables); 
        return ;
    }

    *num_of_oxy -=1;
    *num_of_hydro-=2;

    sem_wait(sem_next_molecule); 

    *num_of_molecule += 1; 
    sem_post(sem_hydrogen); 
    sem_post(sem_hydrogen);


    bonding(variables, *num_of_molecule);

    sem_post(sem_next_molecule);    

    if (*num_of_hydro < 2 ){
        for(int i = 0; i < *num_of_oxy; i++)
        
    
            sem_post(sem_oxygen);
            
    }
    return ;    
}

/**
*@brief doxygen function
*@param variables we take from the structure the type and ID of the element that we use in printing
*/

void hydroProcess(variables_t *variables){
    
    print_output(1, count_line, variables->ID, 0, variables); 
  
    usleep(1000*(rand()% (TI + 1)));

    print_output(2, count_line, variables->ID, 0, variables); 

    sem_wait(sem_barrier);

    if (*num_of_oxy == 0 || *num_of_hydro== 1){

        print_output(5, count_line, variables->ID, 0, variables); 
        return;
    }
    
    *counter_for_hydro += 1; 
    if (*counter_for_hydro >= 2)
        sem_post(sem_oxygen);
    
    sem_wait(sem_hydrogen); 

    *counter_for_hydro -= 1;       

    bonding(variables, *num_of_molecule);
 
    if (*num_of_oxy == 0){
        for (int i = 0; i <= *num_of_hydro ; i++){
            sem_post(sem_barrier);
        }
    }

    sem_post(sem_barrier); 
    return ;
}

/**
*@brief function to combine hydrogen and oxygen into a molecule, the function indicates that the molecule binds and consequently that it has been successfully joined
*@param variables we take from the structure ID of the element that we use in printing
*@param num_of_molecule counter for created molecules
*/
void bonding(variables_t* variables, int num_of_molecule){

    print_output(3, count_line,  variables->ID, num_of_molecule, variables); 
  
    if (variables->type == 'O'){
        usleep(1000*(rand()%(TB + 1)));
        sem_post(sem_molecule);
        sem_post(sem_molecule);
    }
    else{
        sem_wait(sem_molecule);   
    }
    print_output(4 , count_line,  variables->ID, num_of_molecule, variables); 
    return;
}

/**
*@brief function for writing output from the program, consists of a switch with 6 cases,
*@param num_of_mess division of outputs into 6 types, from which we choose
*@param count_line counter for number of lines
*@param who_id number of each element
*@param molecul number of created molecules
*@param variables we use the element type from the structure 
*/
void print_output(int num_of_mess, int *count_line, int who_id,  int molecul, variables_t* variables ){

    sem_wait(sem_mutex);

    switch(num_of_mess){

        case 1 : 
                 fprintf(output,"%d: %c %d: started\n",*count_line, variables->type, who_id);
                 break;
        case 2 :
                 fprintf(output,"%d: %c %d: going to queue\n",*count_line,variables->type, who_id);
                 break;
        case 3 : 
                 fprintf(output,"%d: %c %d: creating molecule %d\n", *count_line, variables->type, who_id ,molecul);
                 break; 
        case 4 : 
                 fprintf(output,"%d: %c %d: molecule %d created\n",*count_line, variables->type ,who_id ,molecul); 
                 break;   
        case 5 : 
                 fprintf(output,"%d: %c %d: not enough O or H\n",*count_line, variables->type, who_id);  
                 break;
        case 6 : 
                 fprintf(output,"%d: %c %d: not enough H\n",*count_line, variables->type, who_id);
                 break;
    }

    fflush(output);
    *count_line += 1;
    sem_post(sem_mutex); 
}

/**
*@brief unlink and close all sempahores and unmapping shared variables 
*/
void clean_all(){

    sem_unlink(SEM_HYDROGEN);
    sem_close(sem_hydrogen);

    sem_unlink(SEM_OXYGEN);
    sem_close(sem_oxygen);

    sem_unlink(SEM_MOLECULE);
    sem_close(sem_molecule);

    sem_unlink(SEM_NEXT_MOLECULE);
    sem_close(sem_next_molecule);

    sem_unlink(SEM_MUTEX);
    sem_close(sem_mutex);

    sem_unlink(SEM_BARRIER);
    sem_close(sem_barrier);

    sem_unlink(SEM_KO);
    sem_close(sem_ko);

    UNMAPPING(count_line) 
    UNMAPPING(num_of_molecule)
    UNMAPPING(counter_for_hydro)
    UNMAPPING(num_of_oxy)
    UNMAPPING(num_of_hydro)
}