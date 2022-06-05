// Gruppo di lavoro:   Andrea Ciccarello, Jacopo Arcari

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#define PIVOT 400

// compilazione: g++ -xc++ Consegna1-Arcari_Ciccarello.cc

// Il programma carica il file data.txt contenente 107 righe con i dati di contagi covid per provincia
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

// negativi Insertion sort
// Sotto il valore del Pivot counting_sort
// Sopra il valore del Pivot quick_sort


int ct_swap=0;
int ct_cmp=0;
int ct_read=0;
int ct_minori=0;
int ct_maggiori=0;
int ct_negativi=0;

int max_dim=0;
int ntests=107; // numero di test
int ndiv=1;
int details=0;
int graph=0;
int ct_opw=0; // operazini in lettura per counting sort

int n=0; /// dimensione dell'array

void print_array(int* A, int dim){
  
    for (int j=0; j<dim; j++) {
      cerr<<A[j]<<" ";
    }
    
}


void swap(int &a, int &b){
  int tmp = a;
  a = b;
  b = tmp;
  /// aggiorno contatore globale di swap 
  ct_swap++;
}


int parse_cmd(int argc, char **argv){

  /// parsing argomento
  max_dim = 743;

  for (int i=1;i<argc;i++){
    if (argv[i][1]=='d')
      ndiv = atoi(argv[i]+3);
    if (argv[i][1]=='t')
      ntests = atoi(argv[i]+3);
    if (argv[i][1]=='v')
      details=1;    
    if (argv[i][1]=='g'){
      graph=1;
      ndiv=1;
      ntests=1;
    }
  }

  return 0;
}

void counting_sort(int* A, int* B, int* C, int n, int k) {
  /// A: array in input 0..n-1
  /// B: array in output 0..n-1
  /// C: array per conteggi 0..k
  /// n: elementi da ordinare
  /// k: valore massimo contenuto in A

  for (int i=0;i<=k;i++){ /// reset array conteggi
    C[i]=0;
    ct_opw++;
  }

  for (int j=0;j<n;j++){ /// conteggio istogramma
    C[A[j]]++;
    ct_read++;
    ct_read++;
    ct_opw++;
  }

  if (details){
    printf("array conteggi\n");
    print_array(C,k+1);
  }
  for (int i=1;i<=k;i++){ /// C[i] contiene il numero di elementi <= i
    C[i]+=C[i-1];
    ct_read++;
    ct_read++;
    ct_opw++;
  }
  
  if (details){
    printf("array con conteggi accumulati\n");
    print_array(C,k+1);
  }
  
  for (int j=n-1;j>=0;j--){ /// per ogni elemento originale in A ->
                            /// mi chiedo nel conteggio C quanti sono gli elementi minori o uguali:
                            /// questo corrisponde alla posizione dell'elemento in B
    if (details)
      printf("A[%d]=%d, C[A[%d]]=%d --> scrivo B[%d-1]=%d\n",j,A[j],j,C[A[j]],C[A[j]],A[j]);

    ct_read++;
    int A_j = A[j]; // contiene il j-esimo elemento di A
    
    B[C[A_j]-1]=A_j;
    ct_read++;
    ct_opw++;
    C[A_j]=C[A_j]-1;
    ct_read++;
    ct_opw++;
  } // for

  if (details){
    printf("array con conteggi accumulati dopo il decremento\n");
    print_array(C,k+1);
  } // if
  
} // counting 

int partition(int* A, int p, int r){

  /// copia valori delle due meta p..q e q+1..r
  ct_read++;
  int x=A[r];
  int i=p-1;
  
  for (int j=p;j<r;j++){
    ct_cmp++;
    ct_read++;
    if (A[j] <=x){
      i++;
      ct_read++;
      ct_read++;
      swap(A[i],A[j]);
    } // if    
  } //for
  ct_read++;
  ct_read++;
  swap(A[i+1],A[r]);

  return i+1;
}


void quick_sort(int* A, int p, int r) {
  if (p<r) {
    int q= partition(A,p,r);
    quick_sort(A,p,q-1);
    quick_sort(A,q+1,r);
  }
}

void insertion_sort(int* A, int n) {
  int i,j;
  for (i=0; i<n; i++) {
    for (j=i-1; j>=0; j--) {
      ct_read++;
      ct_read++;
      if (A[j]>A[j+1]) {
        ct_read++;
        ct_read++;
	      swap (A[j],A[j+1]);
      } else {
	        break;
      }
    }    
  }
}

// divide A in due parti e le memorizza in due array diversi in base al PIVOT
void dividi_array(int* A, int dim, int* minore, int* maggiore, int* negativi, int P){

  for (int i = 0 ; i < dim; i++){ 
    ct_read++;
    int val = A[i];
    if ( val >= 0 && val < P ){ // array numeri minori
      minore[ct_minori] = val;
      ct_minori++;
    }// if
    if(val > P) { // array numeri maggiori
      maggiore[ct_maggiori] = val;
      ct_maggiori++;
    } // if
    if (val < 0){
      negativi[ct_negativi] = val;
      ct_negativi++;
    }// if
  }// for

}


int main(int argc, char **argv) {
  int i,test;
  int* A;  // dove andrà  memorizzato l'array. 
  int* Minori_ordinato; // array per output counting sort
  int* C; // array contatore counting sort
  int* Minori_PIVOT; // array contenete elementi minori di 200
  int* Maggiori_PIVOT; // array contenete elementi maggiori di 200
  int* Negativi; // array contenente i numeri negativi

  if (parse_cmd(argc,argv))
    return 1;
      
  /// allocazione array
  A = new int[max_dim];
  C = new int[PIVOT];
  
  n=max_dim;
  
  ifstream input_data;
  input_data.open("data.txt");
  
    int read_min=-1;
    int read_max=-1;
    long read_avg=0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test=0;test<ntests;test++){ // for dei 107 test

      /// allocazione array

      Minori_PIVOT = new int[max_dim];
      Maggiori_PIVOT = new int[max_dim];
      Minori_ordinato = new int[max_dim];
      Negativi = new int[max_dim];

      /// inizializzazione array: numeri random con range dimensione array
      for (i=0; i<n; i++) {
	      char comma;
      	input_data >> A[i];
	      input_data >> comma;
      }

      if (details){
	      printf("caricato array di dimensione %d\n",n);
	      print_array(A,n);
      }


      ct_swap=0; // swap effettuati
      ct_cmp=0; // confronti effettuati
      ct_read=0; // letture effettuate
      ct_minori=0; // numeri minori di PIVOT
      ct_maggiori=0; // numeri maggiori di PIVOT
      ct_negativi=0; // numeri negativi


      // divido A in due array in base a PIVOT
      dividi_array(A,n,Minori_PIVOT,Maggiori_PIVOT,Negativi,PIVOT); 

      // insertion sort per i numeri negativi
      insertion_sort(Negativi, ct_negativi);

      // counting sort per i numeri minori di PIVOT
      counting_sort(Minori_PIVOT,Minori_ordinato,C,ct_minori,PIVOT);

      //quick sort per i numeri maggiori di PIVOT
      quick_sort(Maggiori_PIVOT, 0, ct_maggiori-1);

      // stampa array
      if (details){
	        printf("Output:\n");
          print_array(Negativi, ct_negativi);
	        print_array(Minori_ordinato,ct_minori); 
          print_array(Maggiori_PIVOT,ct_maggiori);
          
       }


      delete [] Minori_PIVOT;
      delete [] Maggiori_PIVOT;
      delete [] Minori_ordinato;
      delete [] Negativi;

      
      /// statistiche
      read_avg+=ct_read;
      if (read_min<0 || read_min>ct_read) read_min=ct_read;
      if (read_max<0 || read_max<ct_read) read_max=ct_read;
      printf("Test %d %d\n",test,ct_read);

    } // for dei test

    printf("%d,%d,%.1f,%d\n",
	    ntests,
	    read_min ,(0.0+read_avg)/ntests ,read_max);

  
  delete [] A;
  delete [] C;

  return 0;
}
