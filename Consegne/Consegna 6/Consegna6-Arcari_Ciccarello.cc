#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>

using namespace std;


// compilazione: g++ -xc++ g++ -xc++ Consegna6-Arcari_Ciccarello.cc; ./a.out -graph; dot graph.dot -Tpdf -o graph.pdf; open graph.pdf
//
// Obiettivo:
// 1) rappresentazione grafo con lista di adiacenza
// 2) visita ricorsiva (Scc)

int ct_swap = 0;
int ct_cmp = 0;
int ct_op = 0; /// operazioni per la ricerca

int max_dim = 0;
int ntests = 1;
int ndiv = 1;
int details = 0;
int graph = 0;

int idx = 0;

int n = 0; /// dimensione dell'array

/// file di output per grafo
ofstream output_graph;
int n_operazione = 0; /// contatore di operazioni per visualizzare i vari step

int ct_visit = 0; // contatore durante visita

typedef struct list my_stack;

my_stack *s;


//////////////////////////////////////////////////
/// Definizione della struttura dati lista
//////////////////////////////////////////////////

/// struct per il nodo della lista
typedef struct node
{
  int val;
  struct node *next;
} node_t;

/// struct per la lista
typedef struct list
{
  node *head;
  // node* tail; /// per lista doubly linked
} list_t;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati lista
//////////////////////////////////////////////////

//////////////////////////////////////////////////
/// Definizione della struttura dati grafo
//////////////////////////////////////////////////

int *V;          // elenco dei nodi del grafo
int *V_visitato; // nodo visitato?
int *V_Lowlink;
int *V_onStack;


// list_t* E;  /// array con le liste di adiacenza per ogni nodo
list_t **E; /// array di puntatori a le liste di adiacenza per ogni nodo
int n_nodi;

//////////////////////////////////////////////////
/// Fine Definizione della struttura dati grafo
//////////////////////////////////////////////////

/// Questo e' un modo per stampare l'indirizzo node relativamente ad un altro di riferimento.
/// Permette di ottenere offset di piccola dimensione per essere facilmente visualizzati
/// Nota: il metodo non e' robusto e potrebbe avere comportamenti indesiderati su architetture diverse
/// L'alternativa corretta' e' utilizzare %p di printf: es. printf("%p\n",(void*) node);
/// con lo svantaggio di avere interi a 64 bit poco leggibili

list_t *global_ptr_ref = NULL; /// usato per memorizzare il puntatore alla prima lista allocata

int get_address(void *node)
{
  return (int)((long)node - (long)global_ptr_ref);
}

void node_print(int n)
{

  output_graph << "node_" << n << "_" << n_operazione << endl;
  output_graph << "[ shape = oval; ";
  if (V_visitato[n])
    output_graph << "fillcolor = red; style=filled; ";
  output_graph << "label = "
               << "\"Idx: " << n << ", val: " << V[n] << " index " << V_visitato[n] << " lowlink " << V_Lowlink[n] <<"\" ];\n";

  node_t *elem = E[n]->head;
  while (elem != NULL)
  { /// disegno arco
    output_graph << "node_" << n << "_" << n_operazione << " -> ";
    output_graph << "node_" << elem->val << "_" << n_operazione << " [  color=gray ]\n";
    elem = elem->next;
  }
}

void graph_print()
{
  for (int i = 0; i < n_nodi; i++)
    node_print(i);
  n_operazione++;
}

void list_print(list_t *l)
{
  printf("Stampa lista\n");

  if (l->head == NULL)
  {
    printf("Lista vuota\n");
  }
  else
  {
    node_t *current = l->head;

    while (current != NULL)
    {
      if (!details)
        printf("%d, ", current->val);
      else
      { /// stampa completa
        if (current->next == NULL)
          printf("allocato in %d [Val: %d, Next: NULL]\n",
                 get_address(current),
                 current->val);
        else
          printf("allocato in %d [Val: %d, Next: %d]\n",
                 get_address(current),
                 current->val,
                 get_address(current->next));
      }
      current = current->next;
    }
    printf("\n");
  }
}

list_t *list_new(void)
{
  list_t *l = new list;
  if (details)
  {
    printf("Lista creata\n");
  }

  l->head = NULL; //// perche' non e' l.head ?
  if (details)
  {
    printf("Imposto a NULL head\n");
  }

  return l;
}

void list_delete(list_t *l)
{
  //// implementare rimozione dal fondo della lista
  //// deallocazione struct list
}

void list_insert_front(list_t *l, int elem)
{
  /// inserisce un elemento all'inizio della lista
  node_t *new_node = new node_t;
  new_node->next = NULL;

  new_node->val = elem;

  new_node->next = l->head;

  l->head = new_node;
}

void print_array(int *A, int dim)
{
  for (int j = 0; j < dim; j++)
  {
    printf("%d ", A[j]);
  }
  printf("\n");
}

void print_array_graph(int *A, int n, string c, int a, int l, int m, int r)
{
  /// prepara il disegno dell'array A ed il suo contenuto (n celle)
  /// a e' il codice del nodo e c la stringa
  /// l,m,r i tre indici della bisezione

  // return ;

  output_graph << c << a << " [label=<" << endl;

  /// tabella con contenuto array
  output_graph << "<TABLE BORDER=\"0\" CELLBORDER=\"0\" CELLSPACING=\"0\" > " << endl;
  /// indici
  output_graph << "<TR  >";
  for (int j = 0; j < n; j++)
  {
    output_graph << "<TD ";
    output_graph << ">" << j << "</TD>" << endl;
  }
  output_graph << "</TR>" << endl;
  output_graph << "<TR>";
  // contenuto
  for (int j = 0; j < n; j++)
  {
    output_graph << "<TD BORDER=\"1\"";
    if (j == m)
      output_graph << " bgcolor=\"#00a000\""; /// valore testato
    else if (j >= l && j <= r)
      output_graph << " bgcolor=\"#80ff80\""; /// range di competenza
    output_graph << ">" << A[j] << "</TD>" << endl;
  }
  output_graph << "</TR>" << endl;
  output_graph << "</TABLE> " << endl;

  output_graph << ">];" << endl;
}

////////// operazioni stack

void list_delete_front(list_t *l)
{
  /// elimina il primo elemento della lista
  node_t *node = l->head; // il nodo da eliminare

  if (node == NULL) // lista vuota
    return;

  l->head = node->next;

  // if (graph) print_status(l,node,"DEL FRONT: aggancio lista a nodo successivo");

  node->next = NULL;

  // if (graph) print_status(l,node,"DEL FRONT: sgancio puntatore da nodo da cancellare");

  delete node;

  // if (graph) print_status(l,NULL,"DEL FRONT: cancello nodo");
}



my_stack *stack_new()
{
  return list_new();
}

int stack_top(my_stack *s)
{
  if (s->head != NULL)
    return s->head->val;
  printf("ERRORE: stack vuoto!\n");
  return -1;
}

int stack_pop(my_stack *s)
{
  if (s->head != NULL)
  {
    int v = s->head->val;
    list_delete_front((list_t *)s);
    return v;
  }
  printf("ERRORE: stack vuoto!\n");
  return -1;
}




void stack_push(my_stack *s, int v)
{
  list_insert_front((list_t *)s, v);
}


void stack_print(my_stack *s)
{
  list_print((list_t *)s);
}



void Scc(int v)
{

  graph_print();


  V_visitato[v] = idx;
  V_Lowlink[v] = idx;
  idx++;

  stack_push(s, v);
  V_onStack[v] = 1;

  /// esploro la lista di adiacenza
  node_t *elem = E[v]->head;
  while (elem != NULL)
  { /// elenco tutti i nodi nella lista

    graph_print();

    
    /// espando arco  n --> elem->val
    /// quindi Scc(elem->val)
    output_graph << "Scc_"<< n << " -> Scc_"<< elem->val;
    if (V_visitato[elem->val])
      output_graph << "[color=gray, label = \""<< ct_visit++<< "\"]";
    else
      output_graph << "[color=red, label = \""<< ct_visit++<< "\"]";
    output_graph  <<endl;
    


    int w = elem->val;

    if (V_visitato[w] == -1){
      Scc(w);
      if (V_Lowlink[v] > V_Lowlink[w])
        V_Lowlink[v] = V_Lowlink[w];
    }    
      else
      {
        if (V_onStack[w] == 1)
          if (V_Lowlink[v] > V_visitato[w])
            V_Lowlink[v] = V_visitato[w];
      }

      elem = elem->next;
    }

    if (V_Lowlink[v] == V_visitato[v])
    {
      printf("Nuova compoente conessa: ");

      int w = -1;

      do
      {
        w = stack_pop(s);
        V_onStack[w] = 0;
        printf("%d ", w);

      } while (v != w);
      printf("\n");
    }
  }


void swap(int &a, int &b)
{
  int tmp = a;
  a = b;
  b = tmp;
  /// aggiorno contatore globale di swap
  ct_swap++;
}

int parse_cmd(int argc, char **argv)
{
  /// controllo argomenti
  int ok_parse = 0;
  for (int i = 1; i < argc; i++)
  {
    if (argv[i][1] == 'v')
    {
      details = 1;
      ok_parse = 1;
    }
    if (argv[i][1] == 'g')
    {
      graph = 1;
      ok_parse = 1;
    }
  }

  if (argc > 1 && !ok_parse)
  {
    printf("Usage: %s [Options]\n", argv[0]);
    printf("Options:\n");
    printf("  -verbose: Abilita stampe durante l'esecuzione dell'algoritmo\n");
    printf("  -graph: creazione file di dot con il grafo dell'esecuzione (forza d=1 t=1)\n");
    return 1;
  }

  return 0;
}

int main(int argc, char **argv)
{
  int i, test;

  if (parse_cmd(argc, argv))
    return 1;

  if (graph)
  {
    output_graph.open("graph.dot");
    /// preparo header
    output_graph << "digraph g" << endl;
    output_graph << "{ " << endl;
    output_graph << "node [shape=none]" << endl;
    output_graph << "rankdir=\"LR\"" << endl;
    
    //    output_graph << "edge[tailclip=false,arrowtail=dot];"<<endl;
  }

  // int* V; // elenco dei nodi del grafo
  // list_t* E;  /// array con le liste di adiacenza per ogni nodo
  
  s = stack_new();

  n_nodi = 5;
  V = new int[n_nodi];          //(int*)malloc(n_nodi*sizeof(int));
  V_visitato = new int[n_nodi]; //(int*)malloc(n_nodi*sizeof(int));
  V_Lowlink = new int[n_nodi];
  V_onStack = new int[n_nodi];


  E = new list_t*[n_nodi]; //(list_t**)malloc(n_nodi*sizeof(list_t*));

  // inizializzazione
  for (int i = 0; i < n_nodi; i++)
  {
    V[i] = 2 * i;
    V_visitato[i] = 0; // flag = non visitato

    E[i] = list_new();

    if (i == 0)
      global_ptr_ref = E[i];

    for (int j = 0; j < n_nodi; j++)
    {
      // if (rand()%2==0)
      if (i < j)
        list_insert_front(E[i], j);
    }
  }

  graph_print();

  for (int i = 0; i < n_nodi; i++)
  {

    printf("Sono il nodo di indice %d nell'array\n", i);
    printf("Il valore del nodo e' %d\n", V[i]);
    printf("La lista di adiacenza e'\n");
    list_print(E[i]);
  }

 // Scc(0);

  idx = 0;

  for (int i = 0; i < n_nodi; i++)
    V_visitato[i] = -1;

  for (int i = 0; i < n_nodi; i++)
    if (V_visitato[i] == -1)
      Scc(i);

  // init random
  srand((unsigned)time(NULL));

  if (graph)
  {
    /// preparo footer e chiudo file
    output_graph << "}" << endl;
    output_graph.close();
    cout << " File graph.dot scritto" << endl
         << "Creare il grafo con: dot graph.dot -Tpdf -o graph.pdf" << endl;
  }

  return 0;
}
