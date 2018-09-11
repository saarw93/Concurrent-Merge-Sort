/*
*author: Saar Weitzman
*ID: 204175137
*Description: The program use threads to merge the input array it gets. In the right depth in the recursion,
*the program makes the threads and each thread gets a piece from the input array to merge. At the end, the main thread 
*of the program merge all the sorted arrays it gets from the threads. 
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
#include <sys/ipc.h>
#include <sys/types.h>
#include <assert.h>
#include <math.h>
#include <pthread.h>


/*struct to deliver the arguments to the function of the threads*/
struct args 
{
int left, right;
};


/*global variables*/
int array_size = 0;
int parallelism = 0;
int divide_to_threads = 0;  //flag to know when to use fork
int* array;



/*functions declerations*/
int* input_file(int argc, char* argv[]);
void print_before_sort(int *array_before_sort);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r, int divide_to_threads);
void mergeSortThreads(int arr[], int l, int r);
void make_threads(int l, int r);
void* initialize_varaiables(void *args);
void printAfter(int *array);



int main(int argc, char *argv[])
{
    array = input_file(argc, argv);
    
    print_before_sort(array);
    
	divide_to_threads = log2(parallelism);			//initialize the variable which helps us know the depth in the recursion we need to do fork
	
    mergeSort(array, 0, array_size - 1, divide_to_threads);
 
    printAfter(array);
	
	free(array);
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
    int *arr = (int*)malloc((array_size) * sizeof(int));
    assert(arr != NULL);

    for (int i = 0; i < array_size ; i++)
    {
        fscanf(file_input, "%d,", &arr[i]);		//initialize the array's indexes by the input from the file
    }

    fclose(file_input);
    return arr;
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


/*make 2 threads, each one get it's part from the array that he need to sort*/
void make_threads(int l, int r)
{
	pthread_t thread1, thread2;
	int status;
		
	int j = (array_size/ parallelism);  //to get the start/end indexes for the mergeSort right
	struct args a;		//struct for the first thread arguments
	a.left = l;
	a.right = l+j-1;
	
	status = pthread_create(&thread1, NULL, initialize_varaiables,(void*) &a);
	if (status != 0)
	{
		fputs("pthread create failed in main", stderr);
		exit(0);
	}
	
	
	if (parallelism != 1)
	{
	status = -1;
	
	struct args b;		//struct for the second thread arguments
	b.left = l+j;
	b.right = r;
	
	status = pthread_create(&thread2, NULL, initialize_varaiables,(void*) &b);
	if (status != 0)
	{
		fputs("pthread create failed in main", stderr);
		exit(0);
	}
	
	pthread_join(thread1, NULL);		//wait to the end of thread1
	pthread_join(thread2, NULL);		//wait to the end of thread2
			
	merge(array, l, l+(r-l)/2 , r);
	}
	
	else
		pthread_join(thread1, NULL);		//wait to the end of thread1
}


/*connects between the threads and the mergeSortThreads function*/
void* initialize_varaiables(void *p_arguments)
{
struct args *ptr = (struct args*) p_arguments;
int l = ptr->left;
int r = ptr->right;

printf("Create a thread: %lu\n", pthread_self());
mergeSortThreads(array, l, r);
return NULL;
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
void mergeSort(int arr[], int l, int r, int divide_to_threads)
{
	if (divide_to_threads <= 1)      //check for knowing if it is the right depth in the recursion to make threads
	{
		make_threads(l, r);    //create the threads 
		return;	
	}

    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;
        
        divide_to_threads--;
 
        // Sort first and second halves
        mergeSort(arr, l, m, divide_to_threads);
        mergeSort(arr, m+1, r, divide_to_threads);
 
        merge(arr, l, m, r);
    }
}


/* l is for left index and r is right index of the
   sub-array of arr to be sorted */
void mergeSortThreads(int arr[], int l, int r)
{	
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSortThreads(arr, l, m);
        mergeSortThreads(arr, m+1, r);
 
        merge(arr, l, m, r);
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
