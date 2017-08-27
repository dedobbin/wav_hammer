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

Node * get_node(Linked_list ** list, int key)
{
  Node * ptr = (*list)->tail;
  while(ptr != NULL){
    if (ptr->key == key){
      return ptr;
    }
    ptr = ptr->next;
  }
  return NULL;
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

void llist_push(Linked_list ** list, int key, long data)
{
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
  Node * target = get_node(list, key);
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
  Node * node = get_node(list, key);
  if (node == NULL)
    return NULL;
  return node->data;
} 

void llist_remove(Linked_list ** list, int key)
{
  //TODO: rewrite (with comments)
  if ((*list)->head == NULL)
    return;
  else if ((*list)->head->prev == NULL){
    if ((*list)->head->key != key)
      return; 
    free((*list)->head);
    (*list)->head = NULL;
    (*list)->tail = NULL;
  }
  else{
    Node * target = get_node(list, key);
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

void llist_print(Linked_list ** list)
{
  printf("size: %d\nsamples:\n", (*list)->size);
  Node * node = (*list)->tail;
  while(node){
    printf("%d : %ld \n", node->key, node->data);
    node = node->next;
  }
}
