#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;

#define PIVOT 300

// compilazione: g++ -xc++ consegna1-loader.c 

// Il programma carica il file data.txt contenente 107 righe con i dati di contagi covid per provincia
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file



int ct_swap=0;
int ct_cmp=0;
int ct_read=0;

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
    printf("\n");
}


void swap(int &a, int &b){
  int tmp = a;
  a = b;
  b = tmp;
  /// aggiorno contatore globale di swap 
  ct_swap++;
}





void merge(int* A, int p, int q, int r, int* L, int* R){
  ct_read++;
  /// copia valori delle due meta p..q e q+1..r
  int i=0;
  int j=0;
  int k=0;
  
  for (i=0;i<q-p+1;i++){
    ct_read++;
    L[i]=A[p + i];
    ct_read++;
  }
    
    
  L[i]=1000000; /// un numero grande
  
  for (i=0;i<r-q;i++){
    ct_read++;
    R[i]=A[q+1 + i];
    ct_read++;
  }
    
  R[i]=1000000; /// un numero grande


  //// dettagli
  if (details){
    printf("Array L (%d .. %d): ",p,q);
    print_array(L,q-p+1);
    printf("Array R (%d .. %d): ",q+1,r);
    print_array(R,r-q);   
  }

  
  i=0; /// usato per gestire array L
  j=0; /// usato per gestire array R

  for (k=p;k<=r;k++){
    ct_cmp++;
    if (L[i]<=R[j]){
      ct_cmp++;
      ct_read++;
      A[k]=L[i];
      i++;
    }
    else{
      A[k]=R[j];
      ct_read++;
      j++;
    }
  }
  
}


void merge_sort(int* A, int p, int r, int* L, int* R) {
  /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
  if (p<r){
    int q=(p+r)/2;
    merge_sort(A,p,q,L,R);
    merge_sort(A,q+1,r,L,R);
    merge(A,p,q,r,L,R);
    if (details)
      print_array(A,n);

  }
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
    
    B[C[A[j]]-1]=A[j];
    ct_read++;
    ct_read++;
    ct_read++;
    ct_opw++;
    C[A[j]]=C[A[j]]-1;
    ct_read++;
    ct_read++;
    ct_read++;
    ct_opw++;
  }

  if (details){
    printf("array con conteggi accumulati dopo il decremento\n");
    print_array(C,k+1);
  }
  
  
  
}

int partition(int* A, int p, int r){

  /// copia valori delle due meta p..q e q+1..r
  ct_read++;
  int x=A[r];
  int i=p-1;
  
  for (int j=p;j<r;j++){
    ct_cmp++;
    ct_read++;
    if (A[j]<=x){
      i++;
      ct_read++;
      ct_read++;
      swap(A[i],A[j]);
    }    
  }
  ct_read++;
  ct_read++;
  swap(A[i+1],A[r]);

  return i+1;
}


void quick_sort(int* A, int p, int r) {
  /// gli array L e R sono utilizzati come appoggio per copiare i valori: evita le allocazioni nella fase di merge
  if (p<r) {
    int q= partition(A,p,r);
    quick_sort(A,p,q-1);
    quick_sort(A,q+1,r);
  }
}

int ordinamento_minoredi(int* A, int dimensione, int* Arrayminore, int* Arraymaggiore, int Valore){
  int contatore = 0;

  for (int i = 0 ; i<dimensione; i++){
    if (A[i]< Valore){
      if(A[i] >= 0){
      Arrayminore[contatore] = A[i];
      contatore++;
      }
    }// if
  }// for


  int contatoreMaggiore = 0;
  for (int i = 0; i < dimensione; i++)
  {
    if (A[i]>= Valore){
      Arraymaggiore[contatoreMaggiore] = A[i];
      contatoreMaggiore++;
    } // if 
  } // for 



  return contatore;
}





int main(int argc, char **argv) {
  int i,test;
  int* A;  // dove andrÃ  memorizzato l'array. 
  int* output_minore_ordinati;
  int* C;
  int* L;  /// usato come buffer di appoggio
  int* R;  /// usato come buffer di appoggio


  int* ArrayminorediPIVOT; // array contenete elementi minori di 200
  int* maggiori_di_PIVOT; // array contenete elementi maggiori di 200

  if (parse_cmd(argc,argv))
    return 1;
      
  /// allocazione array
  A = new int[max_dim];
  
  //A = new int[max_dim];
  
  C = new int[PIVOT];
  L = new int[max_dim+1];
  R = new int[max_dim+1];
  

  n=max_dim;
  
  ifstream input_data;
  input_data.open("data.txt");
  
    int read_min=-1;
    int read_max=-1;
    long read_avg=0;

    //// lancio ntests volte per coprire diversi casi di input random
    for (test=0;test<ntests;test++){ // for dei 107 test
    
    
      ArrayminorediPIVOT = new int[max_dim];
      maggiori_di_PIVOT = new int[max_dim];
      output_minore_ordinati = new int[max_dim];

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

      int n_minore_PIVOT = ordinamento_minoredi(A,n,ArrayminorediPIVOT,maggiori_di_PIVOT,PIVOT);
      int n_maggiore_PIVOT = n-n_minore_PIVOT;

      
      //merge_sort(maggiori_di_PIVOT, 0, n_maggiore_PIVOT-1, L, R );
      quick_sort(maggiori_di_PIVOT, 0, n_maggiore_PIVOT-1);

      // counting sort per i numeri minori di 200
      counting_sort(ArrayminorediPIVOT,output_minore_ordinati,C,n_minore_PIVOT,PIVOT);

      

      if (details){
	        printf("Output:\n");
	        print_array(output_minore_ordinati,n_minore_PIVOT); 
       }
      if (details){
	        printf("Output:\n");
	        print_array(maggiori_di_PIVOT,n_maggiore_PIVOT);
       }


      delete [] ArrayminorediPIVOT;
      delete [] maggiori_di_PIVOT;
      delete [] output_minore_ordinati;


        
      
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
  delete [] L;
  delete [] R;

  return 0;
}