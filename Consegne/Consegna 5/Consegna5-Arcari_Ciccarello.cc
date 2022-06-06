// Gruppo di lavoro:   Andrea Ciccarello, Jacopo Arcari

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// compilazione: g++ -xc++ Consegna5-Arcari_Ciccarello.cc; ./a.out -graph; neato graph.dot -Tpdf -o graph.pdf; open graph.pdf
//
// Obiettivo:
// 1) grafo con archi pesati
// 2) implementazione shortest path

#define INFTY 1000000

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

const int MAX_SIZE=10000;  /// allocazione statica
int heap[MAX_SIZE];
int heap_size=0;   /// dimensione attuale dell'heap

/// file di output per grafo
ofstream output_graph;
int n_operazione=0; /// contatore di operazioni per visualizzare i vari step

int ct_visit=0; // contatore durante visita 

//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
  int val;   /// prossimo nodo
  float w;   /// peso dell'arco
  struct node * next;
} node_t;


/// struct per la lista
typedef struct list {
     node *head;
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int* V; // elenco dei nodi del grafo
int* V_prev; // nodo precedente dalla visita
float* V_dist; // distanza da sorgente

//list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t** E;  /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;


//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////


/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t* global_ptr_ref=NULL;  /// usato per memorizzare il puntatore alla prima lista allocata

int get_address( void* node){
  return (int)((long)node-(long)global_ptr_ref);
}

/// controllo se il nodo si trova nell'heap
bool isInMinHeap(int idx){
  for (int i=0; i<heap_size; i++)
    if (heap[i]==idx)
      return true;
  return false;
}

void node_print(int n){

  /// calcolo massima distanza (eccetto infinito)
  float max_d=0;
  for (int i=0;i<n_nodi;i++)
    if (max_d< V_dist[i] && V_dist[i]<INFTY)
      max_d=V_dist[i];
  
  output_graph << "node_" << n << "_" << n_operazione <<  endl;
  output_graph << "[ shape = oval; ";

  if (!isInMinHeap(n))
    output_graph << "penwidth = 4; ";
    
  float col=V_dist[n]/max_d; /// distanza in scala 0..1
  output_graph << "fillcolor = \"0.0 0.0 "<<col/2+0.5<<"\"; style=filled; ";
  if (V_dist[n]<INFTY)
    output_graph << "label = " << "\"Idx: " << n << ", dist: " << V_dist[n] << "\" ];\n";
  else
    output_graph << "label = " << "\"Idx: " << n << ", dist: INF\" ];\n";

  node_t* elem=E[n]->head;
  while (elem!=NULL){ /// disegno arco
    output_graph << "node_" << n << "_" << n_operazione << " -> ";
    output_graph << "node_" << elem->val << "_" << n_operazione <<" [ label=\""<< elem->w <<"\", len="<<elem->w/100*10<< " ]\n";
    elem=elem->next;
  }

  if (V_prev[n]!=-1){ // se c'e' un nodo precedente visitato -> disegno arco
    
    float len = 0;
    /*
    // cerco arco V_prev[n] --> V[n]
    
    node_t* elem = E[ V_prev[n] ]->head;
  while(elem!=NULL){
        
        int v = elem->val; /// arco u --> v
        if(v == V[n])
          len = elem->w;

        elem=elem->next;
      }
    */

    len = 1;
    output_graph << "node_" << n << "_" << n_operazione << " -> ";
    output_graph << "node_" << V_prev[n] << "_" << n_operazione <<" [ color=blue, penwidth=5, len="<< len/100*10 << " ]\n";
  }

  
}


void graph_print(){
  for (int i=0;i<n_nodi;i++)
    node_print(i);
  n_operazione++;
}


void list_print(list_t *l){
  printf("Stampa lista\n");
  
  if (l->head == NULL){
    printf("Lista vuota\n");
  }
  else{
    node_t* current = l->head;

    while (current != NULL) {
      if (!details)
      printf("%d w:%f, ", current->val, current->w);
      else{ /// stampa completa 
	if (current->next==NULL)
	printf("allocato in %d [Val: %d, W: %f, Next: NULL]\n",
	       get_address(current),
	       current->val,
	       current->w
	       );
	  else
	printf("allocato in %d [Val: %d, W: %f, Next: %d]\n",
	       get_address(current),
	       current->val,
	       current->w,
	       get_address(current->next)
	       );
      }
      current = current->next;
    }
    printf("\n");
  }
  
}

list_t * list_new(void){
  list_t* l=new list;
  if (details){
    printf("Lista creata\n");
  }
  
  l->head=NULL;   //// perche' non e' l.head ?
  if (details){
    printf("Imposto a NULL head\n");
  }

  return l;
}

void list_insert_front(list_t *l, int elem, float w){
  /// inserisce un elemento all'inizio della lista
  node_t* new_node = new node_t;
  new_node->next=NULL;
    
  new_node->val=elem;
  new_node->w=w;

  new_node->next=l->head;

  l->head = new_node;

} 



void print_array(int* A, int dim){
  for (int j=0; j<dim; j++) {
    printf("%d ",A[j]);
  }
  printf("\n");
}

void print_array_graph(int* A, int n, string c, int a, int l,int m, int r){
  /// prepara il disegno dell'array A ed il suo contenuto (n celle)
  /// a e' il codice del nodo e c la stringa
  /// l,m,r i tre indici della bisezione

  //return ;
  
  output_graph << c << a << " [label=<"<<endl;

  /// tabella con contenuto array
  output_graph <<"<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > "<<endl;
  /// indici
  output_graph <<"<TR  >";
  for (int j=0; j<n; j++) {
    output_graph << "<TD ";
    output_graph <<  ">" << j << "</TD>"<<endl;
  }
  output_graph << "</TR>"<<endl;
  output_graph <<"<TR>";
  //contenuto
  for (int j=0; j<n; j++) {
    output_graph << "<TD BORDER=\"1\"";
    if (j==m) output_graph << " bgcolor=\"#00a000\"";  /// valore testato
    else
      if (j>=l && j<=r ) output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
    output_graph <<  ">" << A[j] << "</TD>"<<endl;
  }
  output_graph << "</TR>"<<endl;
  output_graph << "</TABLE> "<<endl;
  
  output_graph << ">];"<<endl;
	  
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
      if(V_dist[h_l]<V_dist[heap[i]]) // Confronto i con L
        con_chi_mi_scambio = Child_L;

      // considero il figlio destro

      if (Child_R>=0)
          // controllare L <-> R e i <-> R
          ct_read++;
          if ( V_dist[h_r] < V_dist[h_l] && V_dist[h_r] < V_dist[heap[i]])
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

void heap_insert(int idx){
  /// inserisco il nuovo nodo con contenuto elem
  /// nell'ultima posizione dell'array
  /// ovvero continuo a completare il livello corrente

  if (details)
    printf("Inserisco elemento %d in posizione %d\n",idx,heap_size);
  
  if (heap_size<MAX_SIZE){
    int i=heap_size;
    heap_size++;
    
    heap[i]=idx;

    //// sistema relazione con genitori!

    int p = parent_idx(i);
    while( p>=0 ){
        ct_read++;
        ct_read++;
        int h_i=heap[i];
        int h_p=heap[p];
        if (V_dist[h_i] < V_dist[h_p]){ // genitore è più grade --> scambio!
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



void decrease_key(int v){
  /// riordino se necessario il nodo nell'heap altrimenti non faccio nulla

  int i;
  // Cerco l'indice dell'elemento v nell'heap
  for(i = 0; i < heap_size; i++)
    if (heap[i] == v)
      break;

  int p = parent_idx(i);
  while( p>=0 ){
    int h_i=heap[i];
    int h_p=heap[p];
    if(V_dist[h_i] < V_dist[h_p]) {
      int tmp=h_i; heap[i]=h_p; heap[p]=tmp; // swap

      i = p;
      p = parent_idx(i);
    } else 
        break;
  } 
 }// decrease_key


void Dijkstra(int s){

  V_dist[s] = 0; // distanza del nodo sorgente da se stesso

  while(heap_size > 0) { // finchè la coda non è vuota

    //graph_print();

    int u = heap_remove_min(); // rimuovo il nodo con peso minore

      /// esploro la lista di adiacenza
      node_t* elem = E[u]->head;
      while(elem!=NULL){
        int v = elem->val; /// arco u --> v
        
        
        float alt = V_dist[u] + elem->w; 
        if(alt < V_dist[v] && V_dist[u] != INFTY) {
            V_dist[v] = alt;
            V_prev[v] = u;
            decrease_key(v); // log n
          }
              
        elem=elem->next;
      }
  }

  graph_print();
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

  // init random
  srand((unsigned) time(NULL));

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
  
  int N=10;
  n_nodi=N*N;

  //// init nodi
  V= new int[n_nodi]; 
  V_prev=new int[n_nodi];
  V_dist=new float[n_nodi];
  
  //// init archi
  E= new list_t*[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

  // costruzione grafo
  for (int i=0;i<n_nodi;i++){
    V[i]=2*i;
    V_dist[i] = INFTY; // distanza del nodo i da s
    V_prev[i] = -1; // predecessore di i
    heap_insert(i); // inserisco i in coda

    E[i]=list_new();

    if (i==0)
      global_ptr_ref=E[i];

    int x = i % N;
    int y = i / N;

    for(int dx = -1; dx <= 1; dx += 1)
    for(int dy = -1; dy <= 1; dy += 1)
      if(abs(dx) + abs(dy) >= 1){ // limito gli archi ai vicini con una variazione assoluta sulle coordinate
       
        int nx = x + dx;
        int ny = y + dy;

        if(nx >= 0 && nx < N &&
          ny >= 0 && ny < N) { // coordinate del nuovo nodo sono nel grafo

            int j = nx + N * ny; // indice del nuovo nodo
            list_insert_front( E[i] , j, 15*sqrt(abs(dx) + abs(dy)));
          }
      }

/*
    for (int j=0;j<n_nodi;j++){
      //if (rand()%2==0)
      if (i<j)
      	list_insert_front( E[i] , j,10*(3+j+i));
    }
    */
    
  }


  /* /// inserimento manuale 
  list_insert_front(E[0], 1, 50);
  list_insert_front(E[0], 2, 10);
  list_insert_front(E[2], 3, 10);
  list_insert_front(E[3], 1, 10);
  list_insert_front(E[1], 4, 20);
  */


  
  graph_print();
  
  for (int i=0;i<n_nodi;i++){
    printf("Sono il nodo di indice %d nell'array\n",i);
    printf("Il valore del nodo e' %d\n",V[i]);
    printf("La lista di adiacenza e'\n");  
    list_print(E[i]);
  }


  Dijkstra(0);


  
  if (graph){
    /// preparo footer e chiudo file
    output_graph << "}"<<endl; 
    output_graph.close();
    cout << " File graph.dot scritto" << endl<< "****** Creare il grafo con: neato graph.dot -Tpdf -o graph.pdf"<<endl;
  }


  return 0;
}
