#include <pthread.h>
#include <stdio.h>

// Anaphat Sueakhamron 6480228
// Akkhrawin Nair 6580013
// Pakin Panawattanakul 6580043
// Pannawish Kriengyakul 6580117

// structure for argument max : range of finding prime for each tread
// pointer *prime point to array that prime number will be store for each tread
struct thread_arg {
  int max;
  int *prime;
};

// compute prime take structure of arguement which contain max and *prime pointer  
// the pointer point to the array(This code use seperate array for each tread) prime_array[5][200] in main
// max range of each tread and calculate forom max-9999 to max eg. 1-10000 10001-20000 ... 40001-50000
// return count to know how many prime is found
void *compute_prime(void *arg) {
  struct thread_arg *ptr = (struct thread_arg *)arg;
  int count = 0, candidate = ptr->max - 9999;
  if (candidate == 1)
    candidate++;
  while (candidate <= ptr->max) {
    int is_prime = 1;
    for (int factor = 2; factor < candidate; ++factor)
      if (candidate % factor == 0) {
        is_prime = 0;
        break;
      }
    if (is_prime) {
      ptr->prime[count] = candidate;
      count++;
    }
    ++candidate;
  }
  return (void*)count;
}

// function() to print array
// arguement pass count to know how many primes are stored in each array
// arguement pass array to print
void print_array(int count, int array[]) {
  for (int i = 0; i < count; i++)
    printf("%d ", array[i]);
}

// main function declare thread array and thread arguements all in array structure
// divided into 5 thread and each tread compute 1-10000 10001-20000 20001-30000 30001-40000 40001-50000
// then join using pthread_jpom) and pass count to n[5] by second arguement of pthread_join()
int main() {
  pthread_t thread[5];
  int prime_array[5][2000];
  struct thread_arg thread_arg[5];
  for (int i = 0; i < 5; i++) {
    thread_arg[i].max = 10000 + (10000 * i);
    thread_arg[i].prime = prime_array[i];
  }
  for (int i = 0; i < 5; i++)
    pthread_create(&thread[i], NULL, &compute_prime, &thread_arg[i]);

  int n[5];
  for (int i = 0; i < 5; i++)
    pthread_join(thread[i], (void *)&n[i]);
  for (int i = 0; i < 5; i++)
    print_array(n[i], prime_array[i]);
  return 0;
}

// Answering question : Is the run-time of a multi-threaded program faster than a single-threaded program?
// Ans : it might be because creating and managing threads has some extra work that outweighs the benefits. 
// Also, if threads are fighting for the same resources or if the tasks they do are too small, it can slow things down. 
// Try looking at how you split the work between threads and check if there's too much extra stuff going on.