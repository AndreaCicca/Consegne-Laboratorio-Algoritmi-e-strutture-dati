
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// compilazione: g++ -xc++ Consegna2-Arcari_Ciccarello.cc
//

// Alberi binari

// Obiettivo:
// 1) struttura dati, allocazione nodo, inserimento manuale nodi
// 2) creazione albero random

// 3) visita inorder
// 4) implementare visita preorder, postorder
// 5) delete albero
// 6) euler tour: stampa e ricostruzione albero da lista nodi

// 7) flip albero

// 8) height - depth
// 9) isBalanced




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

ofstream output_visit;
ifstream input_visit;





//////////////////////////////////////////////////
/// Definizione della struttura dati tree
//////////////////////////////////////////////////

/// struct per il nodo dell'albero
typedef struct node {
    int val;
    struct node * L;
    struct node * R;
} node_t;



//////////////////////////////////////////////////
/// Fine Definizione della struttura dati tree
//////////////////////////////////////////////////



/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

node_t* global_ptr_ref=NULL;  /// usato per memorizzare il puntatore alla prima lista allocata

int get_address( void* node){
  if (node==NULL)
    return 0;
  return (int)((long)node-(long)global_ptr_ref);
}

/// stampa il codice del nodo per dot
void print_node_code(node_t* n){
  output_graph << "node_" << get_address(n) << "_" << n_operazione;
}

void node_print_graph(node_t *n){

  print_node_code(n);
  output_graph << "\n[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
  output_graph << get_address(n) << "</TD> </TR><TR>\n<TD PORT=\"val\" bgcolor=\"#a0FFa0\">";
  output_graph << n->val << "</TD>\n <TD PORT=\"L\" ";
  if (n->L==NULL)
    output_graph << "bgcolor=\"#808080\"> NULL";
  else    
    output_graph << "> " << get_address(n->L);
  output_graph << "</TD>\n <TD PORT=\"R\" ";
  if (n->R==NULL)
    output_graph << "bgcolor=\"#808080\"> NULL";
  else
    output_graph << "> " << get_address(n->R);
  output_graph <<  "</TD>\n</TR></TABLE>>];\n";

  /// visualizzazione figli sullo stesso piano
  if (n->L!=NULL && n->R!=NULL){
    output_graph << "rank = same; ";
    print_node_code(n);
    output_graph <<";";
    print_node_code(n->L);
    output_graph  <<";\n";
  }

  // mostro archi uscenti
  
  if (n->L!=NULL){ /// disegno arco left
    print_node_code(n);
    output_graph  << ":L:c -> ";
    print_node_code(n->L);
    output_graph  <<":id ;\n";
  }

  if (n->R!=NULL){ /// disegno arco R
    print_node_code(n);
    output_graph << ":R:c -> ";
    print_node_code(n->R);
    output_graph <<":id ;\n";
  }
  
}

void tree_print_rec_graph(node_t* n){
  if (n!=NULL){
    node_print_graph(n);
    tree_print_rec_graph(n->L);
    tree_print_rec_graph(n->R);
  }
}

void tree_print_graph(node_t *n){
  /// stampa ricorsiva del noto
  tree_print_rec_graph(n);
  n_operazione++;

}


void node_print(node_t *n){
  if (n == NULL)
    printf("Puntatore vuoto\n");  
  else
    printf("allocato in %d [Val: %d, L: %d, R: %d]\n",
	   get_address(n),
	   n->val,
	   get_address(n->R),
	   get_address(n->L));  
}

node_t* node_new(int elem){ /// crea nuovo nodo
  node_t* t=new node_t;
  if (details){
    printf("nodo creato\n");
  }
  
  t->val=elem;
  t->L=NULL;
  t->R=NULL;
  if (details){
    printf("Imposto a NULL children\n");
  }

  return t;
}


void tree_insert_child_L(node_t *n, int elem){
  /// inserisco il nuovo nodo con contenuto elem
  /// come figlio Left del nodo n

  /// creo nodo
  n->L=node_new(elem);

}
  
void tree_insert_child_R(node_t *n, int elem){
  /// inserisco il nuovo nodo con contenuto elem
  /// come figlio Right del nodo n  
  n->R=node_new(elem);
}
  

int max_nodes=10;
int n_nodes=0;

void insert_random_rec(node_t* n){
  //// inserisce in modo random un nodo L e R e prosegue ricorsivamente
  /// limito i nodi interni totali, in modo da evitare alberi troppo grandi
  
  printf("inserisco %d\n",n_nodes);
  
  if (n_nodes++ >= max_nodes) /// limito il numero di nodi
    return;
  printf("inserisco %d\n",n_nodes);
  
  float probabilita=0.5; /// tra 0 e 1
  
  if (rand()%100<probabilita*100){ // se numero random e' minore della probabilita' -> aggiungo nodo R con valore a caso
    tree_insert_child_R(n,rand()%100);
  }
  if (rand()%100<probabilita*100){ // se numero random e' minore della probabilita' -> aggiungo nodo L con valore a caso
    tree_insert_child_L(n,rand()%100);
  }
  if (n->L!=NULL)
    insert_random_rec(n->L);
  if (n->R!=NULL)
    insert_random_rec(n->R);
}

int ct_visit=0;

void inOrder(node_t* n){

  if (n->L!=NULL) {
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":id:w -> ";
      print_node_code(n->L);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

    inOrder(n->L);

    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->L);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

  }

  if (details)
    printf("%d ",n->val);

  if (graph){
    node_print_graph(n);
  }
  
  if (n->R!=NULL){
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":val:s -> ";
      print_node_code(n->R);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    inOrder(n->R);
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->R);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    
  }
}

void preOrder(node_t* n){

  if (details)
    printf("%d ",n->val);

  if (graph){
    node_print_graph(n);
  }
  
  if (n->L!=NULL) {
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":id:w -> ";
      print_node_code(n->L);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

    preOrder(n->L);

    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->L);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

  }

  
  
  if (n->R!=NULL){
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":val:s -> ";
      print_node_code(n->R);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    preOrder(n->R);
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->R);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    
  }
}

void postOrder(node_t* n){

  
  if (n->L!=NULL) {
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":id:w -> ";
      print_node_code(n->L);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

    postOrder(n->L);

    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->L);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

  }

  
  
  if (n->R!=NULL){
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":val:s -> ";
      print_node_code(n->R);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    postOrder(n->R);
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->R);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    
  }

  if (details)
    printf("%d ",n->val);

  if (graph){
    node_print_graph(n);
  }

  
}


void EulerOrder(node_t* n){

    if (details)
    printf("%d ",n->val);

    output_visit << n->val << " ";
    
  if (graph){
    node_print_graph(n);
  }

  
  if (n->L!=NULL) {
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":id:w -> ";
      print_node_code(n->L);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

    EulerOrder(n->L);

    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->L);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":val:s [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }

  }

  if (details)
    printf("%d ",n->val);
    output_visit << n->val << " ";

  
  if (n->R!=NULL){
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n);
      output_graph  << ":val:s -> ";
      print_node_code(n->R);
      output_graph  <<":id:w [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    EulerOrder(n->R);
    if (graph){
      // scrivo arco con numero operazione di visita
      print_node_code(n->R);
      output_graph  << ":id:e -> ";
      print_node_code(n);
      output_graph  <<":id:e [ color=blue, fontcolor=blue, penwidth=3, label = \""<< ct_visit++<< "\"]\n";
    }
    
  }

  if (details)
    printf("%d ",n->val);
  output_visit << n->val << " ";
  
}


// 6) euler tour
// costruzione albero in base ai valori stampati dalla visita di eulero
// la funzione restituisce il puntatore alla radice dell'albero
// Es. input: 32 74 74 64 76 76 44 44 44 76 64 64 74 32 85 85 2 36 36 36 2 85 85 85 2 85 32


node_t* build_euler(int preordine){

  int n1,n2,n3;


  /// qui ("zona" preordine) leggiamo gia' dal file il prossimo valore
  /// per decidere se dobbiamo fare la chiamata ricorsiva
  /// in caso si passa il valore letto alla chiamata (equivalente alla lettura in preordine),
  /// altrimenti il valore letto e' il valore da usare in ordine, tra le due chiamate ricorsive

  /// completare con le creazioni dei nodi e corretti puntatori
  
  n1 = preordine;
  node_t* nodo;

  nodo = new node_t;
  nodo->L = NULL;
  nodo->R = NULL;

  ripetere_lettura_n2:
  
  input_visit >> n2;
  
  if (n1!=n2) {
   node_t* nodo_sinistro = build_euler(n2);

    nodo->L = nodo_sinistro;

    goto ripetere_lettura_n2;
  }

  ripetere_lettura_n3:

  input_visit >> n3;

  if (n2!=n3){
   node_t* nodo_destro = build_euler(n3);   

   nodo->R = nodo_destro; 

    goto ripetere_lettura_n3;
  }
  
  nodo->val = preordine;

  return nodo;
} 

node_t* build_euler_senza_goto(int n1){

  int n2,n3; // variabili su cui memorizzare gli elementi letti da file

  /// qui ("zona" preordine) leggiamo gia' dal file il prossimo valore
  /// per decidere se dobbiamo fare la chiamata ricorsiva
  /// in caso si passa il valore letto alla chiamata (equivalente alla lettura in preordine),
  /// altrimenti il valore letto e' il valore da usare in ordine, tra le due chiamate ricorsive

  /// completare con le creazioni dei nodi e corretti puntatori

  int cnt = 1; // contatore letture valore nodo corrente. Ogni valore deve essere letto esattamente 3 volte.

  node_t* nodo = node_new(n1); // inizializzazione nodo con n1


  input_visit >> n2; // lettura elemento successivo 
  if(n1 == n2) ++cnt; // se leggo ancora lo stesso valore incremento cnt

  if (n1 != n2) { 
   nodo->L = build_euler(n2); // creazione sottoalbero sinistro
  }

  input_visit >> n3; // lettura elemento successivo 

  if(n1 == n3)  ++cnt; // se leggo ancora lo stesso valore incremento cnt

  while(n1 == n3 && cnt != 3) { // leggo i valori successivi fino a che non ne trovo uno diverso rispetto al valore del nodo corrente oppure leggo 3 volte quest'ultimo
    input_visit >> n3; // lettura elemento successivo 
     if(n1 == n3) ++cnt; // se leggo ancora lo stesso valore incremento cnt
  }

  if (n2 != n3 && cnt != 3){ // controllo anche che il valore del nodo corrente non sia stato letto 3 volte e quindi non possa più avere un sottoalbero destro
    nodo->R = build_euler(n3); // creazione sottoalbero destro
  }

  if(cnt != 3) { // controllo che il valore del nodo corrente sia stato letto esattamente 3 volte
    input_visit >> n3; 
  }

  return nodo; // ritorno la radice
}


// 7) flip albero
/// crea una copia dell'albero in input, scambiando i sottoalberi L e R 
node_t* flip(node_t* root){
  node_t* n1 = node_new(root->val);

  if (root->L!=NULL){
    n1->R = flip(root->L);
  }
  if (root->R!=NULL){
    n1->L = flip(root->R);
  }
  return n1;
}

// 8) height - depth
int tree_height(node_t* radice) {
    if (radice){
        int altezza_sinistra = tree_height(radice->L);
        int altezza_destra = tree_height(radice->R);
        if (altezza_sinistra >= altezza_destra)
            return altezza_sinistra + 1;
        else
            return altezza_destra + 1;
    }
    else {
      return 0; // se si arriva ad una figlia si effettua un return a 0 e incomincia a contare quando è alto
                  // l'albero
        
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

  // 6) euler tour: stampa e ricostruzione albero da lista nodi

  /* scheletro per la costruzione dell'albero a partire dalla visita di eulero
   */
  input_visit.open("visita.txt"); // File di input per il punto 6)

  int preordine;
  input_visit >> preordine;
  node_t* root=build_euler(preordine);
  global_ptr_ref=root;
  input_visit.close();

  // Viene stampato l'albero binario che è stato creato tramite build_euler in un file chiamato stampa.txt
  // e se si utilizza il flag -graph viene creato un file graph.dot con il grafo dell'albero

  // per poterlo vedere ./a.out -graph; dot -Tpdf graph.dot -o graph.pdf dopo la compilazione

  output_visit.open("stampa.txt"); // file di stampa per il punto 6)
  EulerOrder(root);
  output_visit.close();  
  // Confrontando stampa.txt e visita.txt si vede che le due visite sono identiche
  



  // 7) flip albero

  // flip dell'albero, punto 7 della consegna
  node_t* root1=flip(root);  // nuovo albero flippato
  tree_print_graph(root1); 

  // tree_print_graph stampa l'albero flippato all'interno del file dot, per poterlo vedere
  // ./a.out -graph; dot -Tpdf graph.dot -o graph.pdf
  
  // 8) height - depth

  // Altezza dell'albero, punto 8 della consegna
  int altezza_albero = tree_height(root); 
  printf("Altezza dell'albero: %d\n",altezza_albero);

  

  if (graph){
    /// preparo footer e chiudo file
    output_graph << "}"<<endl; 
    output_graph.close();
    cout << " File graph.dot scritto" << endl<< "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf"<<endl;
  }



  return 0;
}
