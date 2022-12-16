/**
*@author Marek Spirka (xspirk01)
*@brief VUT FIT IOS
*@date 2022-04-29
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <semaphore.h>
#include <time.h>
#include <stdbool.h>
#include <ctype.h>


#define MAPPING(pointer) {(pointer) = mmap(NULL, sizeof(*(pointer)), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);} //define constant MAPPING to allocate memory
#define UNMAPPING(pointer) {munmap(pointer, sizeof(pointer));} //define constant UNMAPPING to free memory

//define name of sempahores to avoid collisions with others 
#define SEM_OXYGEN          "/xpsirk01_sem_oxygen"
#define SEM_MUTEX           "/xspirk01_mutex"
#define SEM_HYDROGEN        "/xspirk01_sem_hydrogen"
#define SEM_MOLECULE        "/xpsirk01_sem_molecule"
#define SEM_NEXT_MOLECULE   "/xspirk01_sem_next_molecule"
#define SEM_BARRIER         "/xspirk01_barrier"
#define SEM_KO              "/xspirk01_finisher"

int NO = 0; //input from argv[1], number of oxygen
int NH = 0; //input from argv[2], number of hydrogen
int TI = 0; //input form argv[3], time in milliseconds, that an oxygen/hydrogen atom waits before forming queues for the formation of molecules
int TB = 0; //input from argv[4], time in milliseconds required to create a single molecule

FILE *output; // define output file for printing 

//creating variables that I use
static int *count_line; //used for count line 
static int *num_of_hydro; //used for hydrogen count
static int *num_of_oxy; //used for oxygen count
static int *counter_for_hydro; //used to find out how many times H was called O
static int *num_of_molecule;  //used for molecul count
static int *num_of_proccess; //used for proccess count

//structure for determining the type and number of Oxygen and Hydrogen
typedef struct{
    int ID; 
    char type;
}variables_t;

variables_t variables;

//Defining all the functions that have been used, more detail will be described in the code
int arguments_checker(int argc, char *argv[]);
int init();
void oxyProcess(variables_t* variables);
void hydroProcess(variables_t *variables);
void bonding(variables_t* variables, int num_of_molecule);
void print_output(int num_of_mess, int *count_line, int who_id,  int molecul, variables_t* variables );
void clean_all();

//Defining all semaphores that have been used
sem_t *sem_oxygen = NULL;
sem_t *sem_hydrogen = NULL;
sem_t *sem_mutex = NULL;
sem_t *sem_molecule = NULL;
sem_t *sem_next_molecule = NULL;
sem_t *sem_barrier = NULL;
sem_t *sem_ko = NULL;