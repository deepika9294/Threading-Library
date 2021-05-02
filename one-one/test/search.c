// Base Code referred from tutorials point 


#include "../dthread.h"
#include <stdio.h>


#define GREEN "\033[0;32;32m"
#define RED "\033[0;31;31m"
#define NONE "\033[m"

//creation of 4 threads for searching a number

#define INARRAY 1
#define NOTINARRAY 0

#define MAX 16

int arr[] = { 1, 6, 8, 11, 13, 14, 15, 19, 21, 23, 26, 28, 31, 65, 108, 220 };

int key = 31;
int is_found = 0;
int part = 0;

int expected_result = INARRAY;   //as 31 is already there 
void* binary_search(void* arg) {
   int thread_part = part++;
   int mid;
   int start = thread_part * (MAX / 4); 
   int end = (thread_part + 1) * (MAX / 4);
   while (start < end && !is_found) { 
      mid = (end - start) / 2 + start;
      if (arr[mid] == key) {
         is_found = 1;
         break;
      }
      else if (arr[mid] > key)
         end = mid - 1;
      else
         start = mid + 1;
   }
}
int main() {
   dthread_t threads[4];
   dthread_init();
   for (int i = 0; i < 4; i++)
      dthread_create(&threads[i],binary_search, NULL);
   for (int i = 0; i < 4; i++)
      dthread_join(threads[i], NULL); 

   //as we know key used is already in the array
   if (is_found == expected_result)
		printf(GREEN "\n**PASSED**: BINARY SEARCH test with 4 threads" NONE);
   else
		printf(RED "\n**FAILED**: BINARY SEARCH test with 4 threads" NONE);
}