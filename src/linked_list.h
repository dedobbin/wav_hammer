#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

typedef struct Node {
  long data;
  int key;
  struct Node * next;
  struct Node * prev;
}Node;

typedef struct Linked_list{
  Node * head;
  Node * tail;
  int size;
}Linked_list;

Linked_list * llist_create();
void llist_push(Linked_list ** list, int key, long data);
long llist_pop(Linked_list ** list);
//Looks for node with requested key value, inserts a new node after that
void llist_insert(Linked_list ** list, int key, int newKey, long newData);
void llist_prepend(Linked_list ** list, int key, long data);
long llist_get(Linked_list ** list, int key);
void llist_remove(Linked_list ** list, int key);
void llist_destroy(Linked_list ** list);
void llist_print(Linked_list ** list);

#endif
