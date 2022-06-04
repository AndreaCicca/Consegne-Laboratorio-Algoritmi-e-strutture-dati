# Coda

La coda è una struttura dati astratta e dinamica (la memoria usata non è necessariamente fisicamente contigua) che denota una collezione omogenea o container di dati con una politica di inserimento e di rimozione *FIFO* (First in First out).

Operazioni:
* Enqueue
* Dequeue

Costo temporale:
- Aggiungere in testa O(1)
- Estraggo dalla coda e Aggiungi in coda : O(n) se non c'è il puntatore alla coda della lista e O(1) con il suddetto puntatore


La coda si presta bene se si utilizzano 2 puntatori all'inizio e alla fine per non usare operazioni O(n).

## Implementazioni alternative

Una coda può essere strutturata anche tramite un **Array**.

I limite di questa implementazione è il fatto che l'array debba essere espanso quando si raggiunge il suo limite massimo.

Per non dover espandere l'array si potrebbe utilizzare una logica circolare per poter sfruttare le celle dell'array che hanno subito subito un Dequeue. Quando l'array è pieno bisogna espanderlo per forza di cose (se non sposto nulla O(1))

## Stringhe palindrome

Posso inserire la medesima stringa in una coda e in uno stack, se il risultato della pop è il medesimo allora la stringa è palindroma.

In questo cosa si sfrutta la caratteristica FIFO e FILO delle 2 strutture dati.
