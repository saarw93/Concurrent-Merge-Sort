
/*author: Saar Weitzman
*ID: 204175137
*Description: This program represent a server that reads input from file about an array and the level of parallelism it need
to be sorted. The server send to the clients who coonect to it parts of the big array for them to sort. After he get all
the parts of the array sorted by those clients, he does the last merge sort for all the sorted array he got and prints it. 
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*global variables*/
int parallelism = 0;
int array_size = 0;
int sub_array_size = 0;


/*functions declerations*/
int* input_file(int argc, char* argv[]);
void print_before_sort(int *array_before_sort);
int* divide_array(int *array, int start);
void make_sockets(int *array, int argc, char* argv);
void print_client_array(int *arr, int socket_num);
void printArray(int *arr);
void arrange_client_sorted_array(int *array, int *client_array, int start);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);


/*prints the error recieved*/
void error(char *msg)
{
    perror(msg);
    exit(1);
}


int main(int argc, char *argv[])
{

    int* array_before_sort = input_file(argc, argv);
    print_before_sort(array_before_sort);
    make_sockets(array_before_sort, argc, argv[2]);
    free(array_before_sort);

    return 0;
}

/*read the input data from the file and put it in the program's variables*/
int* input_file(int argc, char* argv[])
{
    FILE *file_input;

    if (argc < 3)
    {
        perror("invalid input file");
    }

    file_input = fopen(argv[1], "r");    //open the file that it's name is in index 1 of argv
    if (file_input == NULL)
    {
        perror("Can't open file");
    }


    fscanf(file_input, "%d\n%d", &array_size, &parallelism);    //initialize the array_size and parallelism variables by the input from the file
    int *array = (int*)malloc((array_size) * sizeof(int));
    assert(array != NULL);
    for (int i = 0; i < array_size ; i++)
    {
        fscanf(file_input, "%d,", &array[i]);        //initialize the array's indexes by the input from the file
    }

    sub_array_size = (array_size / parallelism);    //the size of the array which will be send to each client
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
    return;
}

/*make the socket and listen to clients who want to connect to it. Get the sorted 
arrays from the clients and sort the big array using those sub-sorted arrays*/
void make_sockets(int *array, int argc, char* argv)
{

    int sockfd, newsockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr cli_addr;
    socklen_t clilen;
    int n,rc;

    if (argc < 3)
    {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);   //create an endpoint for communication
    if (sockfd < 0)
        error("ERROR with opening socket");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv));    //htons function converts the unsigned short integer (the port) from host byte order to network byte order


    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)     //connect the socket "sockfd" to the port
        error("ERROR in binding");


    fd_set rfd;		//set of open sockets
    FD_ZERO(&rfd);


	if(listen(sockfd,parallelism) == -1)   //listen for connections on a socket
	{
		perror("listen() failed");
		exit(0);
	}

    int i;
	int *array_for_client;
	for( i = 0; i < parallelism; i++)
	{
		struct sockaddr_in cli_addr;		//variable from type 'struct sockaddr_in' to get the client's ip address
		newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);  //accept a connection on a socket for client
		if (newsockfd < 0)
			perror("ERROR on accept");
		           
		char client_ip[100];
		printf("Got request from %s\n",inet_ntop(AF_INET,&cli_addr.sin_addr, client_ip,sizeof(client_ip)));  //inet_ntop() convert IP from an Internet address in binary to String format 
		printf("Sending '%d' numbers to socket '%d'\n", sub_array_size, newsockfd);

		int *sub_array_size_ptr = &sub_array_size;
		array_for_client = divide_array(array, i*sub_array_size);     //get the sub-array needed to be send to the client for sort
            
		write(newsockfd, sub_array_size_ptr, sizeof(int));     //write the size of the sub-array to the client
	
		write(newsockfd, array_for_client, sizeof(int)*sub_array_size);      //write the the sub-array to the client
            
		FD_SET(newsockfd, &rfd);     //add the new socket to the set of open sockets
		free(array_for_client);
	}
         	
	for (int j = sockfd+1, k = 0; j <= sockfd + parallelism; j++, k++)      //check which sockets are ready for reading
	{
		if (FD_ISSET(j, &rfd))
		{
			rc = read(j, array_for_client, sizeof(int)*sub_array_size);   //read the sub-array back from the client, after the client sorted it
			print_client_array(array_for_client, j);
			arrange_client_sorted_array(array, array_for_client, k*sub_array_size);  //replace the unsorted part in the array with the sorted part got from client   
           	close(j); 	 
			
			FD_CLR(j, &rfd);
		}
	}
		
	close(sockfd);
	mergeSort(array, 0, array_size-1);      //merge all the sub-sortd arrays to one big sorted array
	printArray(array);
	
	
}


/*get the sub-array needed to be send to the client for sort*/
int* divide_array(int *array, int start)
{
	int *client_array = (int*)malloc(sizeof(int)* sub_array_size);
    assert(client_array != NULL);

    for(int i = start, j = 0 ; i < start+sub_array_size; i++, j++)
    {
        client_array[j] = array[i];

    }
    int *ptr;
    ptr = client_array;
    return ptr;
}


/*print the sorted sub-array the server gets from the client*/
void print_client_array(int *arr, int socket_num)
{
    int i;
	printf("Read from socket '%d'\n", socket_num);
    
    for (i = 0; i < sub_array_size-1; i++)
    {
        printf("%d,", arr[i]);
    }
    printf("%d\n", arr[i]);
}


/*print the sorted array*/
void printArray(int *arr)
{
    int i;
	printf("Final answer: ");

    for (i = 0; i < array_size-1; i++)
    {
        printf("%d,", arr[i]);
    }
    printf("%d\n", arr[i]);
}

/*replace the unsorted part in the array with the sorted part got from client*/
void arrange_client_sorted_array(int *array, int *client_array, int start)
{
	int i, j;
	for (i = start, j = 0; i < start + sub_array_size; i++, j++)
		array[i] = client_array[j];
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
    if (l+sub_array_size < r)
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
