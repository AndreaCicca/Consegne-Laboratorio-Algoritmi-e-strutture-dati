#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// compilazione: g++ -xc++ lezione13-heap.c 
//
// Obiettivo:
// 1) analisi rappresentazione albero completo <---> array
// 2) implementazione inserimento su heap OK
// 3) estrazione massimo con funzione "heapify" (discesa del valore minimo dalla radice)


int ct_swap=0;
int ct_cmp=0;
int ct_op=0;  /// operazioni per la ricerca

int max_dim=0;
int ntests=1;
int ndiv=1;
int details=0;
int graph=0;


int n=0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione=0; /// contatore di operazioni per visualizzare i vari step

const int MAX_SIZE=256;  /// allocazione statica
int heap[MAX_SIZE];
int heap_size=0;   /// dimensione attuale dell'heap

/// uso -1 per indicare un indice non esistente
int parent_idx(int n){
  if (n==0)
    return -1;
  return (n-1)/2;
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

/// restituisce 0 se il nodo in posizione n e' un nodo interno (almeno un figlio)
/// restituisce 1 se il nodo non ha figli
int is_leaf(int n){
  return ( child_L_idx(n)==-1 );  // non c'e' bisogno di controllare il figlio R

  /* versione equivalente
  if (child_L_idx(n)==-1)
    return 1;
  return 0;
  */
  
}


/// stampa il codice del nodo per dot
void print_node_code(int n){
  output_graph << "node_" << n << "_" << n_operazione;
}

void node_print_graph(int n){
  if (details)
    printf("Stampo su grafo nodo %d\n",n);
  print_node_code(n);
  output_graph << "\n[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
  output_graph << n << "</TD> </TR><TR>\n<TD PORT=\"val\" bgcolor=\"#a0FFa0\">";
  output_graph << heap[n] << "</TD>\n</TR></TABLE>>];\n";

  /// visualizzazione figli sullo stesso piano
  if (child_L_idx(n)!=-1 && child_R_idx(n)!=-1){
    output_graph << "rank = same; ";
    print_node_code(child_L_idx(n));
    output_graph <<";";
    print_node_code(child_R_idx(n));
    output_graph  <<";\n";
  }

  // mostro archi uscenti
  
  if (child_L_idx(n)!=-1){ /// disegno arco left
    print_node_code(n);
    output_graph  << ":s -> ";
    print_node_code(child_L_idx(n));
    output_graph  <<":id ;\n";
  }

  if (child_R_idx(n)!=-1){ /// disegno arco R
    print_node_code(n);
    output_graph << ":s -> ";
    print_node_code(child_R_idx(n));
    output_graph <<":id ;\n";
  }
  
}

void tree_print_rec_graph(int n){
  if (n!=-1){
    node_print_graph(n);
    tree_print_rec_graph(child_L_idx(n));
    tree_print_rec_graph(child_R_idx(n));
  }
}

void tree_print_graph(int n){
  /// stampa ricorsiva del nodo
  tree_print_rec_graph(n);
  n_operazione++;

}


void node_print(int n){
  if (n == -1)
    printf("nodo vuoto\n");  
  else
    printf("allocato in %d [Val: %d, L: %d, R: %d]\n",
	   n,
	   heap[n],
	   child_L_idx(n),
	   child_R_idx(n));  
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
    if (p>=0){
      if (heap[i] > heap[p]){ // genitore è più piccolo --> scambio!
      int tmp=heap[i]; heap[i]=heap[p]; heap[p]=tmp; // swap

      i = p;
      p = parent_idx(i);
      

      } else 
          break;
    } // while
  } // if

  }// if else
  else
    printf("Heap pieno!\n");

}// heap_insert

void increase_key(int indice_nodo, int key){
  //key = nuovo valore

  if (indice_nodo<0 || indice_nodo>=heap_size){
    printf("Nodo non esistente\n");
    return;
  }
  
  if (heap[indice_nodo]> key){
    printf("la chiave non e' piu' grande!\n");
    return;
  }

  heap[indice_nodo] = key;

  int i=indice_nodo;
  while (i!=0){ // non sono sulla radice

    i=parent_idx(i);      
  }

  
}


int heap_remove_max(){

  if (heap_size<=0){   /// heap vuoto!
    printf("Errore: heap vuoto\n");
    return -1;
  }
  
  int massimo = heap[0];

  if (details)
    printf("Massimo identificato %d\n",massimo);
  
  /// scambio la radice con l'ultima foglia a destra
  /// il massimo e' stato spostato in fondo --> pronto per l'eliminazione

  int tmp = heap[0]; heap[0]=heap[heap_size-1]; heap[heap_size-1]=tmp;
 
  // elimino il massimo (ora in fondo all'array)

  heap_size--;
  
  //    tree_print_graph(0);  // radice 
  //  n_operazione++;
  
  
  /// nella radice c'e' un valore piccolo (minimo?)
  int i=0; // indice di lavoro (parto dalla root)

    while (i >=0 && !is_leaf(i)){ /// garantisco di fermarmi alla foglia

      if (details)
        printf("Lavoro con il nodo in posizione i = %d, valore %d\n",i,heap[i]);
      
      int con_chi_mi_scambio=-1;
      
      int Child_L = child_L_idx(i);
      int Child_R = child_R_idx(i);

      // max L R --> indice

      // Confronto i con L

      if(heap[Child_L]>heap[i]) // Confronto i con L
        con_chi_mi_scambio = Child_L;

      // considero il figlio destro

      if (Child_R>=0)
          // controllare L <-> R e i <-> R
          if ( heap[Child_R] > heap[Child_L] && heap[Child_R] > heap[i])
            con_chi_mi_scambio = Child_R;
      

      if(con_chi_mi_scambio >=0)
        /// swap tra i e con_chi_mi_scambio
        int tmp = heap[i]; heap[i]=heap[con_chi_mi_scambio]; heap[con_chi_mi_scambio]=tmp;
      
      
    
      i=con_chi_mi_scambio;
      
      //tree_print_graph(0);  // radice 
      //n_operazione++;
      
    }

  return massimo;
}




int parse_cmd(int argc, char **argv){
  /// controllo argomenti
  int ok_parse=0;
  for (int i=1;i<argc;i++){
    if (argv[i][1]=='v'){
      details=1;
      ok_parse=1;
    }
    if (argv[i][1]=='g'){
      graph=1;
      ok_parse=1;
    }
  }

  if (argc > 1 && !ok_parse) {
    printf("Usage: %s [Options]\n",argv[0]);
    printf("Options:\n");
    printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
    printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
    return 1;
  }

  return 0;
}

int main(int argc, char **argv) {
  int i,test;

  if (parse_cmd(argc,argv))
    return 1;

  // init random
  srand((unsigned) time(NULL));

  if (graph){
    output_graph.open("graph.dot");
    /// preparo header
    output_graph << "digraph g"<<endl; 
    output_graph << "{ "<<endl;
    output_graph << "node [shape=none]"<<endl;
    output_graph << "rankdir=\"TB\""<<endl;;
    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;    
  }
  
  

  for (int i=0;i<12;i++){
    //    heap_insert(rand()%100);
    heap_insert(10+i);
  }
    
  tree_print_graph(0);  // radice 
  n_operazione++;

  increase_key(3,20);

  tree_print_graph(0);  // radice 
  n_operazione++;

  
  for (int i=0;i<13;i++){
    int valore = heap_remove_max();
    printf("Il valore massimo estratto e' %d\n",valore);

    tree_print_graph(0);  // radice 
    n_operazione++;
  }
  
  
  // visualizzazione dell'array --> heapsort!
  for (int i=0;i<12;i++){
    printf("%d ",heap[i]);
  }
  printf("\n");
    
  if (graph){
    /// preparo footer e chiudo file
    output_graph << "}"<<endl; 
    output_graph.close();
    cout << " File graph.dot scritto" << endl<< "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf"<<endl;
  }



  return 0;
}

