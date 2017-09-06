#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"

//TODO: sort

//private functions

Node * create_node(int key, long data)
{
  Node * new = NULL;
  if ( (new = malloc(sizeof(Node))) == NULL){
    fprintf(stderr, "Not enough memory to create node\n");
    exit(1);
  }
  new->next = NULL;
  new->prev = NULL;
  new->key = key;
  new->data = data;
  return new;
}

Node * get_node_by_key(Linked_list ** list, int key)
{
  if ( !list || !(*list) || !(*list)->head )
    return;

  Node * ptr = (*list)->tail;
  while(ptr != NULL){
    if (ptr->key == key){
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
}


//Returns nth node
Node * get_node(Linked_list ** list, int n)
{
   if ( !list || !(*list) || !(*list)->head )
    return NULL;
 
  Node * node = (*list)->tail;
  int i;
  for (i = 0; i < n; ++i){
    node = node->next;
  }
  return node;
}

//public functions

Linked_list * llist_create()
{
  Linked_list * list = NULL;
  if ( (list  = malloc(sizeof(Linked_list))) == NULL){
    fprintf(stderr, "Not enough memory to create linked list\n");
    exit(1);
  }
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void llist_destroy(Linked_list ** list)
{
  if ((*list) == NULL)
    return;
  Node * node = (*list)->tail; 
  while (node){
      free(node);
      node = node->next;
    }
   free(*list);
   *list = NULL;
}

void llist_push(Linked_list ** list, int key, long data)
{
 if ( !list || !(*list) )
   return;
  
  Node * new = create_node(key, data);
  if ((*list)->head == NULL)  {
    (*list)->head = new;
    (*list)->tail = new;
  }
  else{
    (*list)->head->next = new;
    new->next = NULL;
    new->prev = (*list)->head;
    (*list)->head = (*list)->head->next;
  }
  ++(*list)->size;
}

//Gives compiler warnings because returns NULL instead of integer when nothing is found
long llist_pop(Linked_list ** list)
{
 if ( !list || !(*list) || !(*list)->head )
   return NULL;

  long result;
  if ((*list)->head == NULL)
    return NULL;
  else if ((*list)->head == (*list)->tail){
    result = (*list)->head->data;
    free((*list)->head);
    (*list)->head =  NULL;
    (*list)->tail = NULL;
  }
  else {
    long result = (*list)->head->data;
    free((*list)->head);
    (*list)->head = (*list)->head->prev;
    (*list)->head->next = NULL;
  }
  --(*list)->size;
  return result;
}

void llist_insert(Linked_list ** list, int key, int newKey, long newData)
{
  Node * target = get_node_by_key(list, key);
  if (target == NULL)
    return;
  Node * new = create_node(newKey, newData);
  if (target->next != NULL){
    target->next->prev = new;
    Node * tmp = target->next;
    new->next = tmp;
    new->prev = target;
    target->next = new;
  }
  else{ 
    llist_push(list, newKey, newData);
  }
  ++(*list)->size;
}

void llist_prepend(Linked_list ** list, int key, long data)
{
  if (! list || !(*list) )
    return;
   
  if ((*list)->tail == NULL){
    llist_push(list, key, data);
  }
  else{
    Node * new = create_node(key, data);
    new->next = (*list)->tail;
    (*list)->tail->prev = new;
    (*list)->tail = new;
    ++(*list)->size;
  } 
}

//Gives compiler warnings because returns NULL instead of integer when nothing is found
long llist_get(Linked_list ** list, int key)
{
  if (!list || !(*list))
    return;

  Node * node = get_node_by_key(list, key);
  if (node == NULL)
    return NULL;
  return node->data;
} 

//TODO: rewrite (with comments)
void llist_remove(Linked_list ** list, int key)
{
  if ((!list || !(*list) || !(*list)->head))
    return;
  
  else if ((*list)->head->prev == NULL){
    if ((*list)->head->key != key)
      return; 
    free((*list)->head);
    (*list)->head = NULL;
    (*list)->tail = NULL;
  }
  else{
    Node * target = get_node_by_key(list, key);
    if (target == NULL)
      return;
    Node * tmp = target;
    if (target == (*list)->tail)
      (*list)->tail = (*list)->tail->next;
    else
      target = tmp->prev;
    target->next = tmp->next;
    free(tmp); 
  }
  ++(*list)->size;
}

void llist_merge(Linked_list ** listOne, Linked_list ** listTwo, int n)
{
  //If n is larger than listOne, pad with extra elements
  //This is the only operation using such logic, perhaps should be removed?
  if (n > (*listOne)->size){
    int extraElements = n - (*listOne)->size;
    int i;
    for (i = 0; i < extraElements; ++i){
      llist_push(listOne, 0, 0);
    }
    llist_push_list(listOne, listTwo);
    return; 
  }
  //Check if listOne is anything
  //TODO: fix 
  else if ( !(*listOne)->head || !(*listTwo)->head )
    return;
  
  //Check if prepend or push can be used
  if (n == 0){
    llist_prepend_list(listOne, listTwo);
    return;
  }else if (n == (*listOne)->size){
    llist_push_list(listOne, listTwo);
    return;
  }
  
  //Can't use prepend  or push, insert inbetween nodes 
  //But first sShallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);
 
  Node * connectionNode = get_node(listOne, n-1);
  Node * connectionEnd = get_node(listOne, n);
  
  connectionNode->next = (*listTwo)->tail;
  connectionNode->next->prev = connectionNode;

  connectionEnd->prev = (*listTwo)->head;
  connectionEnd->prev->next = connectionEnd;

  (*listOne)->size = (*listOne)->size + (*listTwo)->size;

  *listTwo = cpy;
}

void llist_push_list(Linked_list ** listOne, Linked_list ** listTwo)
{
  if ( (*listOne)->head || !(*listTwo)->head )
    return;
  
  //Shallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);
  
  (*listOne)->head->next = (*listTwo)->tail;
  (*listOne)->head->next->prev = (*listOne)->head;
  (*listOne)->head = (*listTwo)->head;
  (*listOne)->size = (*listOne)->size + (*listTwo)->size;
  
  *listTwo = cpy;
}

void llist_prepend_list(Linked_list ** listOne, Linked_list ** listTwo)
{
  if ( !(*listOne)->head || !(*listTwo)->head )
    return;
  
  //Shallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);
  
  (*listOne)->tail->prev = (*listTwo)->head;
  (*listOne)->tail->prev->next = (*listOne)->tail;
  (*listOne)->tail = (*listTwo)->tail;
  (*listOne)->size = (*listOne)->size + (*listTwo)->size;

  *listTwo = cpy;
}

//Can also be implemented with normal ptr argument instead of double, but this is more consistent
Linked_list * llist_copy(Linked_list ** src)
{
  //Check if src is something
  if (!src || !(*src) || !(*src)->head)
    return;

  Linked_list * dst = llist_create();
  Node * node = (*src)->tail;
  while(node){  
    llist_push(&dst, node->key, node->data);
    node = node->next;
  }
  return dst;
}
void llist_print(Linked_list ** list)
{
  printf("size: %d\n", (*list)->size);
  Node * node = (*list)->tail;
  while(node){
    printf("\[%d:%ld\] ", node->key, node->data);
    node = node->next;
  }
  printf("\n");
}

//Can also be implemented with normal ptr argument instead of double, but this is more consistent
void llist_print_backwards(Linked_list ** list)
{
  printf("size %d\n", (*list)->size);
  Node * node = (*list)->head;
  while(node){ 
    printf("\[%d:%ld\] ", node->key, node->data);
    node = node->prev;
  }
  printf("\n");
}


