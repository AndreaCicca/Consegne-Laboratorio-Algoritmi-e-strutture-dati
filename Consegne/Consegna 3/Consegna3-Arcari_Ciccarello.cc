// Gruppo di lavoro:   Andrea Ciccarello, Jacopo Arcari

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
using namespace std;



// compilazione: g++ -xc++ Consegna3-Arcari_Ciccarello.cc; ./a.out -graph; neato graph.dot -Tpdf -o graph.pdf; open graph.pdf

// Il programma carica il file data.txt contenente 107 righe con i dati di contagi covid per provincia
// ./a.out
// In output viene mostrato il numero di accessi in read alla memoria per eseguire il sorting di ciascuna riga

// Obiettivo:
// Creare un algoritmo di sorting che minimizzi la somma del numero di accessi per ogni sorting di ciascuna riga del file

ofstream output_visit;

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

/// file di output per grafo
ofstream output_graph;
int n_operazione=0; /// contatore di operazioni per visualizzare i vari step

const int MAX_SIZE=10000;  /// allocazione statica
int heap[MAX_SIZE];
int heap_size=0;   /// dimensione attuale dell'heap

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

int child_L_idx(int n){
  if (2*n+1>=heap_size)
    return -1;
  return 2*n+1;
}

int child_R_idx(int n){
  if (2*n+2>=heap_size)
    return -1;
  return 2*n+2;
}

int is_leaf(int n){
  return ( child_L_idx(n)==-1 );  // non c'e' bisogno di controllare il figlio R
}

/// uso -1 per indicare un indice non esistente
int parent_idx(int n){
  if (n==0)
    return -1;
  return (n-1)/2;
}



int heap_remove_min(){

  if (heap_size<=0){   /// heap vuoto!
    printf("Errore: heap vuoto\n");
    return -1;
  }
  
  ct_read++;
  int minimo = heap[0];

  if (details)
    printf("minimo identificato %d\n",minimo);
  
  /// scambio la radice con l'ultima foglia a destra
  /// il minimo e' stato spostato in fondo --> pronto per l'eliminazione

  ct_read++;
  int tmp = minimo; heap[0]=heap[heap_size-1]; heap[heap_size-1]=tmp;
 
  // elimino il minimo (ora in fondo all'array)

  heap_size--;
  
  //    tree_print_graph(0);  // radice 
  //  n_operazione++;
  
  
  /// nella radice c'e' un valore piccolo (minimo?)
  int i=0; // indice di lavoro (parto dalla root)

    while (i >=0 && !is_leaf(i)){ /// garantisco di fermarmi alla foglia

      if (details)
        printf("Lavoro con il nodo in posizione i = %d, valore %d\n",i,heap[i]);
      
      int con_chi_mi_scambio=-1; // -1 === non mi scambio con nessuno
      
      int Child_L = child_L_idx(i);
      int Child_R = child_R_idx(i);

      ct_read++;
      ct_read++;
      int h_l = heap[Child_L];
      int h_r = heap[Child_R];

      // max L R --> indice

      // Confronto i con L

      ct_read++;
      if(h_l<heap[i]) // Confronto i con L
        con_chi_mi_scambio = Child_L;

      // considero il figlio destro

      if (Child_R>=0)
          // controllare L <-> R e i <-> R
          ct_read++;
          if ( h_r < h_l && h_r < heap[i])
            con_chi_mi_scambio = Child_R;
      

      if(con_chi_mi_scambio >=0){
          /// swap tra i e con_chi_mi_scambio
        ct_read++;
        ct_read++;
        int tmp = heap[i]; heap[i]=heap[con_chi_mi_scambio]; heap[con_chi_mi_scambio]=tmp;
      }
        
      
      
    
      i=con_chi_mi_scambio;
      
    //   tree_print_graph(0);  // radice 
    //   n_operazione++;
      
    }

  return minimo;
}

void heap_insert(int elem){
  /// inserisco il nuovo nodo con contenuto elem
  /// nell'ultima posizione dell'array
  /// ovvero continuo a completare il livello corrente

  if (details)
    printf("Inserisco elemento %d in posizione %d\n",elem,heap_size);
  
  if (heap_size<MAX_SIZE){
    int i=heap_size;
    heap_size++;
    
    heap[i]=elem;

    //// sistema relazione con genitori!

    int p = parent_idx(i);
    while( p>=0 ){
        ct_read++;
        ct_read++;
        int h_i=heap[i];
        int h_p=heap[p];
        if (h_i < h_p){ // genitore è più grade --> scambio!
        int tmp=h_i; heap[i]=h_p; heap[p]=tmp; // swap

        i = p;
        p = parent_idx(i);
        

        } else 
            break;
      
    }

  }// if else
  else
    printf("Heap pieno!\n");

}// heap_insert

int* heap_sort (int dimensione, int* array_da_ordinare){
  ct_read = 0;

  for (int i = 0; i < dimensione; i++)
  {
    ct_read++;
    heap_insert(array_da_ordinare[i]);
  }

  int* array_ordinato = new int[dimensione];

  for (int i = 0; i < dimensione; i++)
  {
    array_ordinato[i] = heap_remove_min();
  }

  return array_ordinato;
}


// stampa di un array di dimensione data su uno stream di output OS dato
void print_array(int dimensione, int* array, ostream& OS){
  for (int i = 0; i < dimensione; i++)
  {
    OS << array[i];
    OS << endl;
  }
  
}




int main(int argc, char **argv) {

  // algoritmo di sorting con random input (heap)
  int DIMESIONE = 100;
  int MAX_ITERAZION = 10000;
  int* array_da_ordinare;
  int* array_ordinato_heap; 

  int* array_letture_heap;
  int* array_letture_quick;



  array_letture_quick = new int[MAX_ITERAZION];
  array_letture_heap = new int[MAX_ITERAZION];



// for dei 1000 array che devono essere ordinati da heap-sort e quick-sort

for (int j = 0; j < MAX_ITERAZION; j++)
{
  

  if (j%1000 == 0)
  {
    DIMESIONE = DIMESIONE+100;
  }

  
  array_da_ordinare = new int[DIMESIONE];
  array_ordinato_heap = new int[DIMESIONE];
  
  

  for(int i = 0; i<DIMESIONE; i++){
  array_da_ordinare[i] = rand()%100;
  }

  ct_read = 0;

  array_ordinato_heap = heap_sort(DIMESIONE,array_da_ordinare);

  array_letture_heap[j] = ct_read;

  ct_read = 0;

  quick_sort(array_da_ordinare, 0, DIMESIONE-1); // inplace quindi array_da_ordinare diventa quello ordinato tramite quick

  array_letture_quick[j] = ct_read;


  // stampo array output

  // for (int i = 0; i < DIMESIONE; i++)
  // {
  //   printf("%d ",array_ordinato_heap[i]);
  // }

  //DIMESIONE = DIMESIONE+100;

  // eliminazione array
  delete[] array_ordinato_heap;
  delete[] array_da_ordinare;

  

} 
  
  // elaborazione delle read

  // calcolo delle read medie

  double media_heap = 0;
  double media_quick = 0;

  for (int i = 0; i < MAX_ITERAZION; i++)
  {
    media_heap = media_heap + array_letture_heap[i];
    media_quick = media_quick + array_letture_quick[i];
  }

  // stampa delle read medie

  media_heap = media_heap/MAX_ITERAZION;
  media_quick = media_quick/MAX_ITERAZION;

  cout << "Media heap: " << media_heap << endl;
  cout << "Media quick: " << media_quick << endl;


  output_visit.open("Letture_heap.txt");
  print_array(MAX_ITERAZION,array_letture_heap,output_visit);

  output_visit.close();

  output_visit.open("Letture_quick.txt");
  print_array(MAX_ITERAZION,array_letture_quick,output_visit);

  output_visit.close();



  // eliminazione degli array che hanno memorizzato le letture

  delete[] array_letture_heap;
  delete[] array_letture_quick;

}