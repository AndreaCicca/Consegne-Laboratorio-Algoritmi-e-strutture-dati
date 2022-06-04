#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;

// compilazione: g++ -xc++ lezione8-xxx.c 
//
// Obiettivo:

// 1) implementazione chiamata *ricorsiva* per la valutazione: (./a.out 1 2 3 4 + + +)
//    input: stack con tutti i nodi caricati
//    output: valore di valutazione dell'espressione

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
int ct_rec=0; /// contatore delle chiamate ricorsive

char* arithmetic; /// stringa con l'espressione



//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node {
  int val;
  int op;  //// 0: val e' un intero, 1: +, 2: -, 3: *, 4: /
  struct node * next;
} node_t;


/// struct per la lista
typedef struct list {
     node *head;
  // node* tail; /// per lista doubly linked
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////


typedef struct list my_stack;


/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t* global_ptr_ref=NULL;  /// usato per memorizzare il puntatore alla prima lista allocata

int get_address( void* node){
  return (int)((long)node-(long)global_ptr_ref);
}

void node_print_graph(node_t *n){

  output_graph << "node_" << get_address(n) << "_" << n_operazione <<  endl;
  output_graph << "[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
  output_graph << get_address(n) << "</TD> </TR><TR>\n<TD PORT=\"val\">";
  if (n->op==0)
    output_graph << n->val << "</TD>\n <TD PORT=\"next\" ";
  if (n->op==1)
    output_graph << "+" << "</TD>\n <TD PORT=\"next\" ";
  if (n->op==2)
    output_graph << "-" << "</TD>\n <TD PORT=\"next\" ";
  if (n->op==3)
    output_graph << "*" << "</TD>\n <TD PORT=\"next\" ";
  if (n->op==4)
    output_graph << "/" << "</TD>\n <TD PORT=\"next\" ";
  if (n->next==NULL)
    output_graph << "bgcolor=\"#808080\"> NULL";
  else
    output_graph << "> " << get_address(n->next);
  output_graph <<  "</TD>\n</TR></TABLE>>];\n";

  if (n->next!=NULL){ /// disegno arco
    output_graph << "node_" << get_address(n) << "_" << n_operazione << ":next:c -> ";
    output_graph << "node_" << get_address(n->next) << "_" << n_operazione <<":id ;\n";
  }
  
}

void rec_node_print_graph(node_t *n){
  /// stampa per la rappresentazione ricorsiva delle chiamate di eval

  output_graph << "rec_node_" << get_address(n) <<  endl;
  output_graph << "[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR>\n<TD PORT=\"val\">";
  if (n->op==0)
    output_graph << n->val << "</TD>\n";
  if (n->op==1)
    output_graph << "+" << "</TD>\n";
  if (n->op==2)
    output_graph << "-" << "</TD>\n";
  if (n->op==3)
    output_graph << "*" << "</TD>\n";
  if (n->op==4)
    output_graph << "/" << "</TD>\n";
  output_graph <<  "</TR></TABLE>>];\n";  
}


void list_print_graph(list_t *n){

  ///// stampa struttura list_t
  
  output_graph << "list_" << get_address(n) << "_" << n_operazione <<  endl;
  output_graph << "[label=<\n<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\" >\n<TR> <TD CELLPADDING=\"3\" BORDER=\"0\"  ALIGN=\"LEFT\" bgcolor=\"#f0f0f0\" PORT=\"id\">";
  output_graph << "list" << "</TD> </TR><TR>\n<TD PORT=\"next\" ";
  if (n->head==NULL)
    output_graph << "bgcolor=\"#808080\"> head: NULL";
  else
    output_graph << "> head: " << get_address(n->head) ;
  output_graph <<  "</TD>\n</TR></TABLE>>];\n";

  if (n->head!=NULL){ /// disegno arco
    output_graph << "list_" << get_address(n) << "_" << n_operazione << ":next:e -> ";
    output_graph << "node_" << get_address(n->head) << "_" << n_operazione <<":id ;\n";
  }

  /// stampa gli eventuali nodi della lista

    node_t* current = n->head;
    while (current != NULL) {
      node_print_graph(current);
      current = current->next;
    }
}


void list_node_print_graph(list_t* l,node_t* elem){
  node_print_graph(elem);
  list_print_graph(l);
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
      printf("(%d, %d), ", current->val, current->op);
      else{ /// stampa completa 
	if (current->next==NULL)
	printf("allocato in %d [Val: %d, Op: %d, Next: NULL]\n",
	       get_address(current),
	       current->val,
	       current->op);
	  else
	printf("allocato in %d [Val: %d, Op: %d, Next: %d]\n",
	       get_address(current),
	       current->val,
	       current->op,
	       get_address(current->next)
	       );
      }
      current = current->next;
    }
    printf("\n");
  }
  
}



void list_print_reversed(list_t *l);


void print_status(list_t* l, node_t* n, string c){
  /// stampa lista sul grafo con eventuale nodo aggiuntivo (durante le operazioni di modifica)
      output_graph <<  "subgraph cluster_"<< n_operazione << " {"<<endl;    
      output_graph <<  "label=\" " << c << "\";"<<endl;
      if (n==NULL){ /// di solito se chiedo solo la lista e' l'ultima operazione della sequenza -> coloro di verde lo sfondo
	output_graph <<  "style=filled;\ncolor=\"#e0ffe0\";\n";
	list_print_graph(l);
      }
      else
	list_node_print_graph(l,n);
      output_graph <<  "}"<<endl;
      n_operazione++;
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


void list_delete(list_t *l){
  //// implementare rimozione dal fondo della lista
  //// deallocazione struct list
}

void list_insert_front(list_t *l, int elem, int op){
  /// inserisce un elemento all'inizio della lista
  node_t* new_node = new node_t;
  new_node->next=NULL;
  
  //if (graph) print_status(l,new_node,"INS FRONT: nuovo nodo");
  
  new_node->val=elem;
  new_node->op=op;

  //if (graph) print_status(l,new_node,"INS FRONT: assegno valore");
  
  new_node->next=l->head;

  //if (graph) print_status(l,new_node,"INS FRONT: next agganciato a lista");

  l->head = new_node;

  if (graph) print_status(l,NULL,"INS FRONT: lista punta a nuovo nodo");

}

  
void list_delete_front(list_t *l){
  /// elimina il primo elemento della lista
  node_t* node = l->head; // il nodo da eliminare

  if (node==NULL)  // lista vuota
    return;

  l->head=node->next;
    
  //if (graph) print_status(l,node,"DEL FRONT: aggancio lista a nodo successivo");

  node->next=NULL;

  //if (graph) print_status(l,node,"DEL FRONT: sgancio puntatore da nodo da cancellare");

  delete node;

  if (graph) print_status(l,NULL,"DEL FRONT: cancello nodo");

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



////////// operazioni stack

my_stack* stack_new(){
  return list_new();
}

int stack_empty(my_stack* s){
  return s->head==NULL;
}

void stack_top(my_stack* s, int& val, int& op){
  if (s->head!=NULL){
    val=s->head->val;
    op=s->head->op;
  }
  else
  printf("ERRORE TOP: stack vuoto!\n");
}

void stack_pop(my_stack* s, int& val, int& op){
  if (s->head!=NULL){
    val=s->head->val;
    op=s->head->op;
    list_delete_front((list_t*)s);
  }
  else
  printf("ERRORE POP: stack vuoto!\n");
}

void stack_push(my_stack* s, int v, int op){
  list_insert_front((list_t*)s,v,op);
}

void stack_print(my_stack* s){
  list_print((list_t*)s);
}

/// la valutazione dello stack viene eseguita quando lo stack e' completo (top-down)
/// i risultati parziali sono restituiti dalla funzione ricorsiva. stack svuotato, ma niente inserimenti intermedi

int eval(my_stack* s){ 
  int val1,val2,op;
  
  stack_print(s);

  if (stack_empty(s)){
    printf("-----------> errore: stack vuoto\n");
    exit(1);
  }

  /// prepara stampa albero della ricorsione
  int addr_head=get_address(s->head);
  rec_node_print_graph(s->head);
  
  stack_pop(s,val1,op);
  stack_print(s);
  printf("Estratto elemento val=%d op=%d\n",val1,op);
  
  if (op==0){ /// valore -> restituisce
    return val1;
  }
  
  if (op==1){ /// somma
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val1=eval(s);
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val2=eval(s);
    return val1+val2;
  }
  if (op==2){ /// -
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val1=eval(s);
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val2=eval(s);
    return val2-val1;
  }
  if (op==3){ /// *
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val1=eval(s);
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val2=eval(s);
    return val1*val2;
  }
  if (op==4){ /// /
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val1=eval(s);
    output_graph << "rec_node_" << addr_head << ":e -> rec_node_" << get_address(s->head) << " [label="<< ct_rec++ <<"];\n";    
    val2=eval(s);
    return val2/val1;
  }
  return -1;
  
}


int main(int argc, char **argv) {
  int i,test;

  if (argc==1) {
    printf("Errore: immettere una espressione in notazione polacca inversa\n");
    return 1;
  }
  
  graph=1;
  details=1;
    
  if (graph){
    output_graph.open("graph.dot");
    /// preparo header
    output_graph << "digraph g"<<endl; 
    output_graph << "{ "<<endl;
    output_graph << "node [shape=none]"<<endl;
    output_graph << "rankdir=\"LR\""<<endl;;
    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;    
  }
  
  my_stack* s = stack_new();
  global_ptr_ref=s;

      
  /// parsing
  
  char c;
  int idx=0;
  int arg_ct=1; // scorre tutti gli argomenti
  
  while (arg_ct<argc){ /// stringhe non terminate
    int unknown=1;
    
    if (argv[arg_ct][idx]>='0' &&   // parsing numero
	argv[arg_ct][idx]<='9'){
      int temp=0;
      while (argv[arg_ct][idx]>='0' &&   /// cifre del numero
	     argv[arg_ct][idx]<='9') {
	temp=10*temp+argv[arg_ct][idx]-'0';
	unknown=0;
	idx++;
      }
      /// il numero e' terminato -> metto in stack
      printf("parse %d\n",temp);
      stack_push(s,temp,0);
    }
    else
    if (argv[arg_ct][idx]=='+'){   // parsing somma
      stack_push(s,0,1);
      idx++;
      unknown=0;
    }
    else
    if (argv[arg_ct][idx]=='-'){   // parsing differenza
      stack_push(s,0,2);
      idx++;
      unknown=0;
    }
    else
    if (argv[arg_ct][idx]=='*'){   // parsing moltiplicazione
      stack_push(s,0,3);
      idx++;
      unknown=0;
    }
    else
    if (argv[arg_ct][idx]=='/'){   // parsing divisinoe
      stack_push(s,0,4);
      idx++;
      unknown=0;
    }
    else
    if (argv[arg_ct][idx]==' '){   // parsing spazio
      idx++;
      unknown=0;
    }
    
    if (unknown==1){   // carattere sconosciuto
      printf("parsing error %c\n",argv[arg_ct][idx]);
      return 1;
    }
    
    /// se ho terminato la stringa, vado alla successiva
    if (argv[arg_ct][idx]==0){
      arg_ct++;
      idx=0;
    }
    
  }
  printf("dopo parsing\n");
  stack_print(s);

  int val,op;
  stack_top(s,val,op);  
  printf("Risultato %d\n",eval(s));
      

  if (graph){
    /// preparo footer e chiudo file
    output_graph << "}"<<endl; 
    output_graph.close();
    cout << " File graph.dot scritto" << endl<< "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf"<<endl;
  }



  return 0;
}
