
This software is divided into 3 assignments:
assignment 1: a concurrent merge sort using shared memory
assignment 2: a concunrrent merge sort using sockets with server and clients
assignment 3: a concurrent merge sort using threads


how to install assignment 1:
open linux terminal, navigate to the folder containing ex3q1.c
using the "cd" command (confirm it by using ls command)
incase you want to use the makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall ex3q1.c -o ex3q1
and your program will automaticily be compiled

how to install assignment 2:
open linux terminal, navigate to the folder containing ex3q2s.c and ex3q2c.c
using the "cd" command (confirm it by using ls command)
incase you want to use the makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall ex3q2s.c -o ex3q2s
and your program will automaticily be compiled.
after you compile the server, open another linux terminal
and type make and the program will automaticily be compiled,
if you don't, type gcc -Wall ex3q2c.c -o ex3q2c
and your progam will automaticily be compiled.

how to install assignment 3:
open linux terminal, navigate to the folder containing ex3q3.c
using the "cd" command (confirm it by using ls command)
incase you want to use the makefile, type make and the program will
automaticily be compiled, if you don't, type gcc -Wall ex3q3.c -o ex3q3 -lpthread -lm
and your program will automaticily be compiled
 

to activate assignment 1:
open linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q1 *name of input file* (remove *)

to activate assignment 2:
open linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q2s *name of input file* (remove *)

open another linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q2c localhost *port number* (remove *)

to activate assignment 3:
open linux terminal, navigate to ex3 executeable file
location using "cd" command (confirm it using ls command) and type
./ex3q3 *name of input file* (remove *)


------------------assignment 1 global variables -------------------------------
int array_size - the size of the array
int parallelism - the level of parallelism
int divide_to_forks - flag to know when to use fork
int shm_id - the shared memory id


------------------assignment 1 functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"


int* input_file(int, char**);
input: int - argument counter, char** - list in arguments
output: returns the array from the input file

void print_before_sort(int*);
input: int* - the array of numbers before sort
output: prints the array to the terminal

int* shared_memory(int*, char*);
input: int* - the array we need to insert into the shared memory section, char* - the name of file key
output: returns the array in the shared memory section

void mergeSort(int*, int, int, int);
input: int* - the array to be sorted, int - left index, int - right index, int- the depth we need to make forks
output: sorting the array


void mergeSortForks(int*, int, int);
input: int* - the array to be sorted, int- left index, int- right index
output: sorting the array


void merge(int*, int , int , int );
input: int* - the array to sort, int - left index, int - middle index, int - right index
output: merging the sub-arrays


void printAfter(int*);
input: int* - the sorted array
output: prints the sorted array


void free_memory();
output: frees the shared memory allocation

int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.



------------------assignment 2 server global variables -------------------------------
int parallelism - the level of parallelism
int array_size - the size of the array
int sub_array_size- the size of the clients' arrays 


------------------assignment 2 server functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"

void error(char*);
input: char[] - error messege
output: prints in stderr format the error


int* input_file(int, char**);
input: int - argument counter, char** - list in arguments
output: returns the array from the input file


void print_before_sort(int*);
input: int* - the array of numbers before sort
output: prints to the terminal the array


int* divide_array(int*, int);
input: int* - the array to divide, int start - from where to start the divided array
output: returns a divided array


void make_sockets(int*, int, char*);
input:int* - the input array, int - argc, char* - argv
output: does the socket to connect with the clients


void print_client_array(int*, int);
input: int* - the sub array the supposed to be printed, int - which sub array is supposed to be printed (by knowing which client send request through its socket with server)
output: prints the sub array


void printArray(int*);
input: int* - the sorted array
output: prints the sorted array


void arrange_client_sorted_array(int*, int*, int);
input: int* - array, int* - client array, int - start index of array
output: puts the client array into the array


void merge(int*, int , int , int );
input: int* - the array to sort, int - left index, int - middle index, int - right index
output: merging the sub-arrays


void mergeSort(int*, int, int);
input: int* - the array to be sorted, int - left index, int - right index
output: sorting the array


int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.



------------------assignment 2 client functions -------------------------------
* incase of memory allocation failure, program will exit with
message "Memory allocation failed"

void error(char*);
input: char[] - error messege
output: prints in stderr format the error


void printArray(int*, int, int);
input: int* - the array to be printed, int size - size of the array, int check - if the array is received or send
output: prints the array into the terminal


void merge(int*, int , int , int );
input: int* - the array to sort, int - left index, int - middle index, int - right index
output: merging the sub-arrays


void mergeSort(int*, int, int);
input: int* - the array to be sorted, int - left index, int - right index
output: sorting the array


int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.



-----------------assignment 3 global variables----------------------------------
int array_size - the size of the array
int parallelism - the level of parallelism
int  divide_to_threads - flag to know when to make the threads
int *array - pointer to the array



-----------------------assignment 3 structs-------------------------------------

struct args - struct to deliver the arguments to the function of the threads
when using "pthread_create"  
has the arguments of int left, int right for the start and end index of the array


----------------------assignment 3 functions -------------------------------
* incase of memory allocation failure, program will exit with message "Memory allocation failed"


int* input_file(int, char**);
input: int - argument counter, char** - list in arguments
output: returns the array from the input file

void print_before_sort(int*);
input: int* - the array of numbers before sort
output: prints the array to the terminal


void mergeSort(int*, int, int, int);
input: int* - the array to be sorted, int - left index, int - right index, int- the depth we need to make threads
output: sorting the array


void mergeSortThreads(int*, int, int);
input: int* - the array to be sorted, int- left index, int- right index
output: sorting the array


void merge(int*, int , int , int);
input: int* - the array to sort, int - left index, int - middle index, int - right index
output: merging the sub-arrays


void make_threads(int, int);
input: int - left index of the array, int - right index of the array
output: sort the array with merge sort and threads


void* initialize_varaiables(void*);
input: void* - the struct with the arguments of left and right indexes of array
output: print which thread was created and send the thread with the sub-array to the mergeSortThreads function 


void printAfter(int*);
input: int* - the sorted array
output: prints the sorted array



int main(int argc, char* argv[]) command:
input: char* argv[] - the arguments, int argc - number of argument
output: concurrent merge sort that has been fully tested with valgrind to test for memory
leak, no leak was found.
