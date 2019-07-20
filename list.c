#include <stdio.h>
#include <stdlib.h>
#include "list.h"

/*
 * Allocate memory for a node of type struct nodeStruct and initialize
 * it with the value item. Return a pointer to the new node.
 */
struct nodeStruct* List_createNode(int item){
  struct nodeStruct* new_node = (struct nodeStruct*) malloc(sizeof(struct nodeStruct));
  new_node->item = item;
  new_node->next = NULL;
  return new_node;
}

/*
 * Insert node at the head of the list.
 */
void List_insertHead (struct nodeStruct **headRef, struct nodeStruct *node){
  node->next = *headRef;
  *headRef = node;
}

/*
 * Insert node after the tail of the list.
 */
void List_insertTail (struct nodeStruct **headRef, struct nodeStruct *node){
  if (*headRef == NULL){
    *headRef = node;
    return;
  }
  struct nodeStruct* nextnode;
  nextnode = *headRef;

  while (nextnode->next != NULL){
    nextnode = nextnode->next;
  }

  nextnode->next = node;
}

/*
 * Count number of nodes in the list.
 * Return 0 if the list is empty, i.e., head == NULL
 */
int List_countNodes (struct nodeStruct *head){
  if (head == NULL){
    return 0;
  }
  struct nodeStruct* node;
  node = head;
  int count = 0;

  while (node != NULL){
    count++;
    node = node->next;
  }

  return count;
}

/*
 * Return the first node holding the value item, return NULL if none found
 */
struct nodeStruct* List_findNode(struct nodeStruct *head, int item){
  struct nodeStruct* node;
  node = head;

  while (node != NULL){
    if (node->item == item){
      return node;
    }
    else{
      node = node->next;
    }
  }
  return NULL;
}

/*
 * Delete node from the list and free memory allocated to it.
 * This function assumes that node has been properly set to a valid node
 * in the list. For example, the client code may have found it by calling
 * List_findNode(). If the list contains only one node, the head of the list
 * should be set to NULL.
 */
void List_deleteNode (struct nodeStruct **headRef, struct nodeStruct *node){
  //if inserted node is null
  if (node == NULL){
    return;
  }
  //deleting first element of the list
  if (*headRef == node){
    *headRef = node->next;
    free(node);
    return;
  }
  //deleting last element of the list
  if (node->next == NULL){
    struct nodeStruct* tmp = *headRef;
    while(tmp->next->next != NULL){
      tmp = tmp->next;
    }
    tmp->next = NULL;
    free(node);
    return;
  }

  //every other case
  struct nodeStruct* temp;
  temp = node->next;
  node->item = temp->item;
  node->next = temp->next;
  free(temp);
}

void swap_values (struct nodeStruct *node1, struct nodeStruct *node2){
  int temp = node1->item;
  node1->item = node2->item;
  node2->item = temp;
}

void print_values (struct nodeStruct **headRef){
  struct nodeStruct* temp = *headRef;
  while (temp != NULL){
    printf("%d ", temp->item);
    temp = temp->next;
  }
  printf("\n");
}

/*
 * Sort the list in ascending order based on the item field.
 * Any sorting algorithm is fine.
 */
void List_sort (struct nodeStruct **headRef){
  struct nodeStruct* temp = *headRef;
  int flag = 1;

  while (flag == 1){
    flag = 0;
    while (temp->next != NULL){
      if (temp->item > temp->next->item){
        swap_values(temp,temp->next);
        flag = 1;
      }
      temp = temp->next;
    }
    temp = *headRef;
  }
}
