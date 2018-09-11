
/*author: Saar Weitzman
*ID: 204175137
*Description: This program represent a client who gets a part of an array from server, sort it using merge sort, and send it back to the server
*/

#include <stdio.h>
#include <signal.h>
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
#include <sys/socket.h>
#include <assert.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <assert.h>


/*functions declerations*/
void printArray(int *arr, int array_size, int check);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);


/*prints the error recieved*/
void error(char *msg)
{
    perror(msg);
    exit(0);
}


int main(int argc, char *argv[])
{
    int sockfd, n,num,rc;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);     //create an endpoint for communication
    if (sockfd < 0)
        error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;


    bcopy((char *)server->h_addr, (char *)  &serv_addr.sin_addr.s_addr, server->h_length);    //copies h_length bytes from server->h_addr to &serv_addr.sin_addr.s_addr
    serv_addr.sin_port = htons(atoi(argv[2]));   //htons function converts the unsigned short integer (the port) from host byte order to network byte order

    if (connect(sockfd,(const struct sockaddr*)&serv_addr,sizeof(serv_addr)) < 0)      //initiate a connection on a socket
        error("ERROR connecting");

	int client_array_size = 0;
	int *ptr_client_array_size = &client_array_size;
	
	rc = read(sockfd, ptr_client_array_size, sizeof(int));      //read the array size (of the client) from the server
	
	int *recieved_array = (int*)malloc(sizeof(int)*client_array_size);
	assert(recieved_array != NULL);
	
	rc = read(sockfd, recieved_array, sizeof(int)*client_array_size);     //read the array (for the client) from the server
	
	printArray(recieved_array, client_array_size , 0);    //print the array recieved from the server before sort (in third variable :0 = array before sort)
        
	mergeSort(recieved_array, 0, client_array_size-1);    //sort the array with merge sort

	printArray(recieved_array, client_array_size, 1);    ////print the array recieved from the server after sort (in third variable :1 = array after sort)

	rc = write(sockfd, recieved_array, sizeof(int)*client_array_size);		//write the sorted array back to the server
	close(sockfd);

    free(recieved_array);
    return 0;
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
void mergeSort(int arr[], int l, int r)
{
    if (l < r)
    {
        // Same as (l+r)/2, but avoids overflow for
        // large l and h
        int m = l+(r-l)/2;

        // Sort first and second halves
        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}


/*print the array recieved from the server before sort (check = 0 means array before sort) or after sort (check = 1 means array after sort)*/
void printArray(int *arr, int array_size, int check)
{
    int i;
    if ( check == 0 )
        printf("Array received: ");
    else
        printf("Array Send: ");

    for (i = 0; i < array_size-1; i++)
    {
        printf("%d,", arr[i]);
    }
    printf("%d\n", arr[i]);
}
