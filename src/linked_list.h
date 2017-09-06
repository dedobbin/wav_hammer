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

//Most functions take double pointer param because they need to alter where pointers are pointing to, llist_copy and print functions do so for consistency

Linked_list * llist_create();
void llist_destroy(Linked_list ** list);
void llist_push(Linked_list ** list, int key, long data);
long llist_pop(Linked_list ** list);
//Looks for node with requested key value, inserts a new node after that
void llist_insert(Linked_list ** list, int key, int newKey, long newData);
void llist_prepend(Linked_list ** list, int key, long data);
long llist_get(Linked_list ** list, int key);
void llist_remove(Linked_list ** list, int key);
//Places elements of listTwo in listOne after element n of listOne
void llist_merge(Linked_list ** listOne, Linked_list ** listTwo, int n);
void llist_push_list(Linked_list ** listOne, Linked_list ** listTwo);
void llist_prepend_list(Linked_list ** listOne, Linked_list ** listTwo);
Linked_list * llist_copy(Linked_list ** src);
void llist_print(Linked_list ** list);
void llist_print_backwards(Linked_list ** list);

#endif
