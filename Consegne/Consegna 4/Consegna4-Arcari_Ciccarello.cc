// Gruppo di lavoro:   Andrea Ciccarello, Jacopo Arcari
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <array>

using namespace std;

// compilazione: g++ -xc++ g++ -xc++ Consegna4-Arcari_Ciccarello.cc; ./a.out -graph; neato graph.dot -Tpdf -o graph.pdf; open graph.pdf
//
// Obiettivo:
// 1) rappresentazione grafo con lista di adiacenza
// 2) visita ricorsiva (DFS)

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

int ct_visit=0; // contatore durante visita 



int* V; // elenco dei nodi del grafo
int* V_visitato; // nodo visitato?


const int n_nodi=5;
int matrice_di_adiacenza[n_nodi][n_nodi];




/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili


void node_print(int n){

  output_graph << "node_" << n << "_" << n_operazione <<  endl;
  output_graph << "[ shape = oval; ";
  if (V_visitato[n])
    output_graph << "fillcolor = red; style=filled; ";
  output_graph << "label = " << "\"Idx: " << n << ", val: " << V[n] << "\" ];\n";

  for(int i = 0; i <n_nodi; i++) { /// disegno arco
    if(matrice_di_adiacenza[n][i] == 1) {
    output_graph << "node_" << n << "_" << n_operazione << " -> ";
    output_graph << "node_" << i << "_" << n_operazione <<" [  color=gray ]\n";
    }
    
  }
  
}


void graph_print(){
  for (int i=0;i<n_nodi;i++)
    node_print(i);
  n_operazione++;
}


void stampa_matrice_indice (int n){

  printf("Stampa matrice\n");

  for (int i = 0; i < n_nodi; i++)
  {
    if (matrice_di_adiacenza[n][i] == 1)
    {
      printf("%d  ", i);
    }
    
  }
  printf("\n");
}


// Deep first search
void DFS(int n){

  if (details)
    printf("DFS: lavoro sul nodo %d (visitato %d)\n",n,V_visitato[n]);
  
  if (V_visitato[n])
    return;
  
  V_visitato[n]=1;   // prima volta che incontro questo nodo

  if (details)
    printf("Visito il nodo %d (val %d)\n",n,V[n]);

  /// esploro la lista di adiacenza
  // node_t* elem=E[n]->head;


  for(int i = 0; i < n_nodi; i++) {
    if(matrice_di_adiacenza[n][i] == 1) {
    output_graph << "dfs_"<< n << " -> dfs_"<< i;
    if(V_visitato[i])
      output_graph << "[color=gray, label = \""<< ct_visit++<< "\"]";
    else
      output_graph << "[color=red, label = \""<< ct_visit++<< "\"]";
    output_graph  <<endl;

    DFS(i);
    }

  }
  
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

  if (graph){
    output_graph.open("graph.dot");
    /// preparo header
    output_graph << "digraph g"<<endl; 
    output_graph << "{ "<<endl;
    output_graph << "node [shape=none]"<<endl;
    output_graph << "rankdir=\"LR\""<<endl;;
    //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;    
  }



  V= new int[n_nodi]; //(int*)malloc(n_nodi*sizeof(int));
  V_visitato=new int[n_nodi];//(int*)malloc(n_nodi*sizeof(int));

  // inizializzazione
  for (int i=0;i<n_nodi;i++){
    V[i]=2*i;
    V_visitato[i]=0;  // flag = non visitato

    for (int j=0;j<n_nodi;j++){
    
    matrice_di_adiacenza[i][j]=1;

    }
    
  }


   graph_print();
  
  for (int i=0;i<n_nodi;i++){

    printf("Sono il nodo di indice %d nell'array\n",i);
    printf("Il valore del nodo e' %d\n",V[i]);
    printf("La lista di adiacenza e'\n");  
    //list_print(E[i]);
    stampa_matrice_indice(i);
    printf("\n");
  }


  DFS(1);



  
    
  // init random
  srand((unsigned) time(NULL));

  if (graph){
    /// preparo footer e chiudo file
    output_graph << "}"<<endl; 
    output_graph.close();
    cout << " File graph.dot scritto" << endl<< "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf"<<endl;
  }



  return 0;
}
