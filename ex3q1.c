/*
*author: Saar Weitzman
*ID: 204175137
*Description: The program use shared memory and forks to merge the input array it gets. In the right depth in the recursion,
 the program makes the forks and each fork gets a piece from the input array to merge. At the end, the program merge all
 the sorted arrays it gets from the forks. 
*/

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pwd.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>


/*global variables*/
int array_size = 0;
int parallelism = 0;
int divide_to_forks = 0;  //flag to know when to use fork
int shm_id;


/*functions declerations*/
int* input_file(int argc, char* argv[]);
void print_before_sort(int *array_before_sort);
int* shared_memory(int* array, char* argv);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r, int divide_to_forks);
void mergeSortForks(int arr[], int l, int r);
void make_forks(int arr[], int l, int r);
void printAfter(int *array);
void free_memory();



int main(int argc, char *argv[])
{
    int* array_before_sort = input_file(argc, argv);
    print_before_sort(array_before_sort);
	int* shared_array = shared_memory(array_before_sort, argv[1]);		//get the pointer to the shared memory
	divide_to_forks = log2(parallelism);			//initialize the variable which helps us know the depth in the recursion we need to do fork
	
    mergeSort(shared_array, 0, array_size - 1, divide_to_forks);
 
    printAfter(shared_array);
	shmdt(shared_array);		//disconnect the pointer from the shared memory
	free_memory();
	free (array_before_sort);
	

}


/*read the input data from the file and put it in the program's variables*/
int* input_file(int argc, char* argv[])
{
	FILE *file_input;

    if (argc < 2)
    {
        perror("invalid input file");
    }

    file_input = fopen(argv[1], "r");		//open the file that it's name is in index 1 of argv
    if (file_input == NULL)
    {
        perror("Can't open file");
    }


    fscanf(file_input, "%d\n%d", &array_size, &parallelism);		//initialize the array_size and parallelism variables by the input from the file
    int *array = (int*)malloc((array_size) * sizeof(int));
    assert(array != NULL);

    for (int i = 0; i < array_size ; i++)
    {
        fscanf(file_input, "%d,", &array[i]);		//initialize the array's indexes by the input from the file
    }

    fclose(file_input);
    return array;
}


/*print the starting format of the server. includes the array_size, parallelism and the array before the sort*/
void print_before_sort(int *array_before_sort)
{
    printf("Amount of numbers that sort: %d\n", array_size);
    printf("Degree of parallelism: %d\n", parallelism);
    int i;
    printf("Before sort: ");
    for (i = 0; i < array_size-1; i++)
    {
        printf("%d,", array_before_sort[i]);
    }
    printf("%d\n", array_before_sort[i]);
}


/*make the shared memory and the pointer to the shared memory*/
int* shared_memory(int* array, char* argv)
{

    key_t key;
    if ((key = ftok(argv, 'y')) == -1)     //making outer key with ftok
    {
        perror("ftok() failed");
        exit(0);
    }

    shm_id = shmget(key, sizeof(int) * array_size, IPC_CREAT | 0600);       //making a new shared memory
    if (shm_id == -1) 
    {
        perror("shmget() failed");
        exit(0);
    }

    int *shm_ptr;
    shm_ptr = (int *) shmat(shm_id, NULL, 0);           //making a pointer to the shared memory
    if (shm_ptr == -1)
    {
        perror("shmat failed");
        exit(0);
    }

    for (int i = 0; i < array_size; i++)
    {
    	shm_ptr[i] = array[i];
    }
    
    return shm_ptr;
}


// Merges two subarrays of arr[].
// First subarray is arr[l..m]
// Second subarray is arr[m+1..r]
void merge(int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    /* create temp arrays */
    int L[n1], R[n2];
 
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
 
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray
    j = 0; // Initial index of second subarray
    k = l; // Initial index of merged subarray
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    /* Copy the remaining elements of L[], if there
       are any */
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    /* Copy the remaining elements of R[], if there
       are any */
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

 
/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r, int divide_to_forks)
{
	if (divide_to_forks <= 1)      //check for knowing if it is the right depth in the recursion to do fork()
	{
		make_forks(arr, l, r);    //does the forks needed 
		return;	
	}

    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;
        
        divide_to_forks--;
 
        // Sort first and second halves
        mergeSort(arr, l, m, divide_to_forks);
        mergeSort(arr, m+1, r, divide_to_forks);
 
        merge(arr, l, m, r);
    }
}


/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSortForks(int arr[], int l, int r)
{	
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;
 
        // Sort first and second halves
        mergeSortForks(arr, l, m);
        mergeSortForks(arr, m+1, r);
 
        merge(arr, l, m, r);
    }
}


/*make 2 forks, each one get it's part from the array that he need to sort*/
void make_forks(int arr[], int l, int r)
{
	int status = 0;
	pid_t pid1;
	
	int j = (array_size/ parallelism);  //to get the start/end indexes for the mergeSort right
	pid1 = fork();
	
	if(pid1 < 0)
		{
			printf("pid1 fork() error\n");
			exit(0);
		}
	
	if (pid1 == 0)    //in child process
		{
			printf("Create a process: %d\n", getpid());
			mergeSortForks(arr, l, l+j-1);
			exit(1); 
	 	}
		 	
	waitpid(pid1, &status, 0);
	
	if ( parallelism != 1 )
	{
		status = 0;
	
		pid_t pid2;
		pid2 = fork();
	
		if(pid2 < 0)
			{
				printf("pid2 fork() error\n");
				exit(0);
			}
		
		if (pid2 == 0)    //in child process
			{
				printf("Create a process: %d\n", getpid());
				mergeSortForks(arr, l+j, r);
				exit(1);  
	 		}
		waitpid(pid2, &status, 0);
	
		merge(arr, l, l+(r-l)/2 , r);
	}
}	

	  
/* prints the sorted array */
void printAfter(int *array)
{
	int i;
	printf("After  sort: ");
    for (i = 0; i < array_size-1; i++)
    {
        printf("%d,", array[i]);
    }
    printf("%d\n", array[i]);
}


/*free the shared memory allocation*/
void free_memory()
{
	if (shmctl(shm_id,IPC_RMID, NULL) == -1)
	{
		perror("shmctrl failed\n");
		exit(0);
	}
}

