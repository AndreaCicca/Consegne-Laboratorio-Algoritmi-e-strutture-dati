#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// compilazione: g++ -xc++ lezione1-1-insertion_sort-simple.c 
//
// Obiettivo:

// Lanciare con
// > ./a.out 10
// > ./a.out 100

// > time ./a.out 1000




void swap(int* a, int* b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void insertion_sort(int* A, int n) {
  int i,j;
  for (i=0; i<n; i++) {
    for (j=i-1; j>=0; j--) {
      if (A[j]>A[j+1]) {
	swap (&A[j],&A[j+1]);
      } else {
	break;
      }
    }    
  }
}


int main(int argc, char **argv) {
  int n,i;
  int* A;

  /// controllo argomenti
  if (argc != 2) {
    printf("Usage: %s dim-array\n",argv[0]);
    return 1;
  }

  /// parsing argomento
  n = atoi(argv[1]);
  
  /// allocazione array
  A = (int*) malloc(n*sizeof(int));

  /// inizializzazione array
  for (i=0; i<n; i++) {
    A[i]=n-i;
  }

  /// input
  printf("L'input e':\n");
  for (i=0; i<n; i++) {
    printf("%d ",A[i]);
  }
  printf("\n");
  
  /// sort
  insertion_sort(A,n);

  /// output
  printf("L'output e':\n");
  for (i=0; i<n; i++) {
    printf("%d ",A[i]);
  }
  printf("\n");

  return 0;
}
