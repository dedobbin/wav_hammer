#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linked_list.h"
#include "datatypes.h"

//private functions

Node * create_node(long data)
{
  Node * new = NULL;
  if ( (new = malloc(sizeof(Node))) == NULL){
    fprintf(stderr, "Not enough memory to create node\n");
    exit(1);
  }
  new->next = NULL;
  new->prev = NULL;
  new->data = data;
  return new;
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

void remove_node(Node * node)
{
  if (!node)
    return;
  if (node->next)
    node->next->prev = node->prev;
  if (node->prev)
    node->prev->next = node->next;
  free(node);
  node = NULL;
}

void connect_node(Node * target, Node * new, bool after)
{
  if ( !target || !new)
    return;
  
  //insert after target
  if (after){
    new->next = target->next;
    new->prev = target;
    if (new->next)
      new->next->prev = new;
    if (new->prev)
      new->prev->next = new;
  
  //insert before target
  } else {
    new->next = target;
    new->prev = target->prev;
    if (new->next)
      new->next->prev = new;
    if (new->prev)
      new->prev->next = new;
  }
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

void llist_push(Linked_list ** list, long data)
{
  if ( !list || !(*list) )
    return;
  Node * new = create_node(data);
  connect_node((*list)->head, new, true);
  
  (*list)->head = new;
  //Check if was pushed into empy list, if also point tail
  if (!(*list)->tail)
    (*list)->tail = new;
  
  (*list)->size ++;
}

long llist_pop(Linked_list ** list)
{
  if (!list || !*list || !(*list)->head)
    return 0;
  Node * newHead = (*list)->head->prev;
  long result = (*list)->head->data;
  remove_node((*list)->head);
  
  (*list)->head = newHead;
  if (!(*list)->head){
    //list is empty, point tail to NULL
    (*list)->tail = NULL;
    (*list)->head = NULL;
  }  
  --(*list)->size;
  return result;
}

void llist_insert(Linked_list ** list, int n, long data)
{
  if (!list || !(*list) || n > (*list)->size || n < 0)
    return;

  Node * newNode = create_node(data);
  
  //Use this path if list needs to be appended
  if (n == (*list)->size){
    connect_node((*list)->head, newNode, true);
    (*list)->head = newNode;
    ++(*list)->size;
    return;
  }
  
  //Move to target node, to insert new node before it
  Node * target = (*list)->tail;
  int i;
  for (i = 0; i < n;++i) {
    target = target->next;
  }
  connect_node(target, newNode, false);
  ++(*list)->size; 

  //Set new tail if new node was inserted at index 0 
  if (n == 0)
    (*list)->tail = newNode;
  
}

long llist_get(Linked_list ** list, int n)
{
  int result = 0;
  if (! (! list || !(*list) || !(*list)->head || n < 0 || n >= (*list)->size) ) 
    result = (get_node(list, n))->data;
  return result;
}

void llist_remove(Linked_list ** list, int n)
{
  if (n < 0 || n > (*list)->size || !list || !*list || !(*list)->head)
   return;
 
  Node * target = get_node(list, n);
  if (target == (*list)->head)
    (*list)->head = target->prev;
  if (target == (*list)->tail)
    (*list)->tail = target->next;
     
  if (target){
   remove_node(target);
   --(*list)->size;
  }

}

void llist_merge(Linked_list ** listOne, Linked_list ** listTwo, int n)
{
  //If n is larger than listOne, pad with extra elements
  //This is the only operation using such logic, perhaps should be removed?
  if (n > (*listOne)->size){
    int extraElements = n - (*listOne)->size;
    int i;
    for (i = 0; i < extraElements; ++i){
      llist_push(listOne, 0);
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
  //But first shallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);

  if ( ! (*listOne)->head){   
    (*listOne)->head = (*listTwo)->head;
    (*listOne)->tail = (*listTwo)->tail;
    (*listOne)->size = (*listTwo)->size;
  } else { 
    Node * connectionNode = get_node(listOne, n-1);
    Node * connectionEnd = get_node(listOne, n);
  
    connectionNode->next = (*listTwo)->tail;
    connectionNode->next->prev = connectionNode;

    connectionEnd->prev = (*listTwo)->head;
    connectionEnd->prev->next = connectionEnd;

    (*listOne)->size = (*listOne)->size + (*listTwo)->size;
  }
 
  *listTwo = cpy;
}

void llist_push_list(Linked_list ** listOne, Linked_list ** listTwo)
{
  if ( !listOne || !(*listOne) || !listTwo || !(*listTwo) || !(*listTwo)->head )
    return;
  
  //Shallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);  

  if ( ! (*listOne)->head){   
    (*listOne)->head = (*listTwo)->head;
    (*listOne)->tail = (*listTwo)->tail;
    (*listOne)->size = (*listTwo)->size;
  } else {
    (*listOne)->head->next = (*listTwo)->tail;
    (*listOne)->head->next->prev = (*listOne)->head;
    (*listOne)->head = (*listTwo)->head;
    (*listOne)->size = (*listOne)->size + (*listTwo)->size;
  }
  
  *listTwo = cpy;
}

void llist_prepend_list(Linked_list ** listOne, Linked_list ** listTwo)
{
  if ( !listOne || !(*listOne) || !listTwo || !(*listTwo) || !(*listTwo)->head )
    return;
  
  //Shallow copy listTwo, point listTwo here when everything is done
  //otherwise it shares memory with listOne
  Linked_list * cpy = llist_copy(listTwo);
  
  if ( ! (*listOne)->head){   
    (*listOne)->head = (*listTwo)->head;
    (*listOne)->tail = (*listTwo)->tail;
    (*listOne)->size = (*listTwo)->size;
  } else { 
    (*listOne)->tail->prev = (*listTwo)->head;
    (*listOne)->tail->prev->next = (*listOne)->tail;
    (*listOne)->tail = (*listTwo)->tail;
    (*listOne)->size = (*listOne)->size + (*listTwo)->size;
  }
  *listTwo = cpy;
}

//Can also be implemented with normal ptr argument instead of double, but this is more consistent
Linked_list * llist_copy(Linked_list ** src)
{
  if (!src || !(*src) || !(*src)->head)
    return;

  Linked_list * dst = llist_create();
  Node * node = (*src)->tail;
  while(node){  
    llist_push(&dst, node->data);
    node = node->next;
  }
  return dst;
}

void llist_print(Linked_list ** list)
{
  if (!list || !(*list)){
    printf("List is not initialized\n");
  } else if (!(*list)->tail){
    printf("size: 0\n");
  } else {
    printf("size: %d\n", (*list)->size);
    Node * node = (*list)->tail;
    while (node){
      printf("\[%ld\] ", node->data);
      node = node->next;
    }
    printf("\n");
  }
}

//Can also be implemented with normal ptr argument instead of double, but this is more consistent
void llist_print_backwards(Linked_list ** list)
{
  if (!list || !(*list)){
    printf("List is not initialized\n");
  } else if (!(*list)->head){
    printf("size: 0\n");
  } else {
    printf("size: %d\n", (*list)->size);
    Node * node = (*list)->head;
    while(node){ 
      printf("\[%ld\] ", node->data);
      node = node->prev;
    }
  printf("\n");
  }
}

