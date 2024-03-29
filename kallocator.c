#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "kallocator.h"

/* ------------- Declaring structs ------------- */

struct nodeStruct {
    bool allocated;
    int size;
    int location;
    struct nodeStruct *next;
};

struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    struct nodeStruct* klist_head;
};

struct KAllocator kallocator;

/* ------------- Linked list implementation ------------- */

//initializes the first node of meta data list
struct nodeStruct* initialize_list(int size){
  struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
  if (new_node != NULL){
    new_node->size = size;
    new_node->allocated = false;
    new_node->location = 0;
    new_node->next = NULL;
  }
  return new_node;
}

int allocate_memory(struct nodeStruct **headRef, int size, enum allocation_algorithm type){
  struct nodeStruct* node = *headRef;

  switch(type){
    case FIRST_FIT:
    {
      while(node != NULL){
        //loop to find free space
        while(node->allocated && node != NULL){
          node = node->next;
        }
        //check if free node has enough space for allocation
        if(node->size >= size){
          node->allocated = true;
          int remaining_memory = node->size - size;
          //if remaining memory is greater than 0 create a new node
          if(remaining_memory > 0){
            //if the next node is a not a free chunk create a node
            if(node->next != NULL){
              if(node->next->allocated){
                struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
                new_node->size = remaining_memory;
                new_node->allocated = false;
                new_node->location = node->location + size;
                new_node->next = node->next;
                node->next = new_node;
                node->size = size;
                return node->location;
              }
              //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
              else{
                node->next->size = node->next->size + remaining_memory;
                node->size = size;
                return node->location;
              }
            }
            //case when next node is NULL
            else{
              struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
              new_node->size = remaining_memory;
              new_node->allocated = false;
              new_node->location = node->location + size;
              new_node->next = NULL;
              node->next = new_node;
              node->size = size;
              return node->location;
            }
          }
          //case where it is a perfect fit just return
          return node->location;
        }
        //case where free memory chunk is not big enough for the desired size loop again
        else{
          node = node->next;
        }
      }
      return -1;
    }//first fit case
    case BEST_FIT:
    {
      struct nodeStruct* best_fit_node = node;
      int smallest_size_difference = kallocator.size;

      while(node != NULL){
        //loop to find free space
        while(node->allocated && node != NULL){
          node = node->next;
        }
        //check if free node has enough space for allocation
        if(node->size >= size){
          //update smallest size difference if smallest value so far
          if((node->size - size) < smallest_size_difference){
            smallest_size_difference = node->size - size;
            best_fit_node = node;
          }
        }
        node = node->next;
      }
      //return NULL if there are no free spaces after iterating through list
      if(best_fit_node->size < size){
        return -1;
      }
      //after iterating through entire list, allocate memory in the smallest space
      best_fit_node->allocated = true;
      int remaining_memory = best_fit_node->size - size;
      if(remaining_memory > 0){
        //if the next node is a not a free chunk create a node
        if(best_fit_node->next != NULL){
          if(best_fit_node->next->allocated){
            struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
            new_node->size = remaining_memory;
            new_node->allocated = false;
            new_node->location = best_fit_node->location + size;
            new_node->next = best_fit_node->next;
            best_fit_node->next = new_node;
            best_fit_node->size = size;
            return best_fit_node->location;
          }
          //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
          else{
            best_fit_node->next->size = best_fit_node->next->size + remaining_memory;
            best_fit_node->next->location = best_fit_node->next->location - remaining_memory;
            best_fit_node->size = size;
            return best_fit_node->location;
          }
        }
        //case when next node is NULL
        else{
          struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
          new_node->size = remaining_memory;
          new_node->allocated = false;
          new_node->location = best_fit_node->location + size;
          new_node->next = NULL;
          best_fit_node->next = new_node;
          best_fit_node->size = size;
          return best_fit_node->location;
        }
      }
      else{
        return best_fit_node->location;
      }
    }//best fit case
    case WORST_FIT:
    {
      struct nodeStruct* worst_fit_node = node;
      int largest_size_difference = 0;

      while(node != NULL){
        //loop to find free space
        while(node->allocated && node != NULL){
          node = node->next;
        }
        //check if free node has enough space for allocation
        if(node->size >= size){
          //update smallest size difference if smallest value so far
          if((node->size - size) > largest_size_difference){
            largest_size_difference = node->size - size;
            worst_fit_node = node;
          }
        }
        node = node->next;
      }
      //return NULL if there are no free spaces after iterating through list
      if(worst_fit_node->size < size){
        return -1;
      }
      //after iterating through entire list, allocate memory in the smallest space
      worst_fit_node->allocated = true;
      int remaining_memory = worst_fit_node->size - size;
      if(remaining_memory > 0){
        //if the next node is a not a free chunk create a node
        if(worst_fit_node->next != NULL){
          if(worst_fit_node->next->allocated){
            struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
            new_node->size = remaining_memory;
            new_node->allocated = false;
            new_node->location = worst_fit_node->location + size;
            new_node->next = worst_fit_node->next;
            worst_fit_node->next = new_node;
            worst_fit_node->size = size;
            return worst_fit_node->location;
          }
          //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
          else{
            worst_fit_node->next->size = worst_fit_node->next->size + remaining_memory;
            worst_fit_node->next->location = worst_fit_node->next->location - remaining_memory;
            worst_fit_node->size = size;
            return worst_fit_node->location;
          }
        }
        //case when next node is NULL
        else{
          struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
          new_node->size = remaining_memory;
          new_node->allocated = false;
          new_node->location = worst_fit_node->location + size;
          new_node->next = NULL;
          worst_fit_node->next = new_node;
          worst_fit_node->size = size;
          return worst_fit_node->location;
        }
      }
      else{
        return worst_fit_node->location;
      }
    }//worst fit case
  } //switch
}//function

//frees up all the nodes of the linked list
void delete_list(struct nodeStruct **headRef){
  struct nodeStruct* node = *headRef;
  struct nodeStruct* tmp = *headRef;

  while(node != NULL){
    node = node->next;
    free(tmp);
    tmp = node;
  }
}

void free_memory(void* ptr, struct nodeStruct **headRef){
  struct nodeStruct* node = *headRef;
  struct nodeStruct* prev = *headRef;

  while(node != NULL){
    //if ptr is pointing to the node location
    if(&kallocator.memory[node->location] == ptr){
      if(node->next != NULL){
        //if next node is allocated just free current node
        if(node->next->allocated){
          node->allocated = false;
        }
        //if next node is a free node combine nodes to prevent contiguous free memory
        else if (!node->next->allocated){
          int tmp_size = node->next->size;
          node->size = node->size + tmp_size;
          struct nodeStruct* tmp_node = node->next;
          node->next = node->next->next;
          free(tmp_node);
          node->allocated = false;
        }
        //if prev node is free node combine nodes to prevent contiguous free memory
        if(prev->allocated == false && node != prev){
          int tmp_size = node->size;
          prev->size = prev->size + tmp_size;
          prev->next = node->next;
          free(node);
          node = prev;
        }
        return;
      }
      //case where next node is null
      node->allocated = false;
      //if prev node is free node combine nodes to prevent contiguous free memory
      if(prev->allocated == false && node != prev){
        int tmp_size = node->size;
        prev->size = prev->size + tmp_size;
        prev->next = NULL;
        free(node);
        node = prev;
      }
      return;
    }
    prev = node;
    node = node->next;
  }
}

/* ------------- Memory allocation implementaion ------------- */

void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);

    // initialize linked list
    kallocator.klist_head = initialize_list(_size);
}

void destroy_allocator() {
    free(kallocator.memory);

    // free linked list memory to avoid memory leak
    delete_list(&kallocator.klist_head);
}

void* kalloc(int _size) {
    void* ptr = NULL;

    // Allocate memory from kallocator.memory
    int location = allocate_memory(&kallocator.klist_head, _size, kallocator.aalgorithm);
    //checking for successsful allocation
    if (location != -1){
      ptr = kallocator.memory + location;
    }

    // ptr = address of allocated memory
    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);
    free_memory(_ptr, &kallocator.klist_head);
}

int compact_allocation(void** _before, void** _after) {

    int compacted_size = 0;
    int total_freed_memory = 0;

    struct nodeStruct* node = kallocator.klist_head;
    struct nodeStruct* tmp = node;
    struct nodeStruct* prev = node;

    while(node != NULL){
      //if node is allocated add it to before/after arrays
      if(node->allocated){
        _before[compacted_size] = kallocator.memory + node->location;
        //move allocated node by amount of spaces freed so far
        node->location -= total_freed_memory;
        _after[compacted_size] = kallocator.memory + node->location;
        compacted_size++;
        prev = node;
        node = node->next;
        continue;
      }
      //case where first node is a free node
      if(node == prev && !node->allocated){
        total_freed_memory += node->size;
        //case where head pointer is empty, if so update new head pointer
        if(node->next != NULL){
          kallocator.klist_head = node->next;
        }
        tmp = node->next;
        prev->next = node->next;
        free(node);
        node = tmp;
        continue;
      }
      //if node is a free node, add the size to total free size at end of linked list
      else{
        total_freed_memory += node->size;
        tmp = node->next;
        prev->next = node->next;
        free(node);
        node = tmp;
      }
    }
    struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
    new_node->size = total_freed_memory;
    new_node->location = prev->location + prev->size;
    new_node->allocated = false;
    new_node->next = NULL;
    prev->next = new_node;

    return compacted_size;
}


int available_memory() {
    int available_memory_size = 0;
    // Calculate available memory size
    struct nodeStruct* node = kallocator.klist_head;
    while(node != NULL){
      if (node->allocated == false){
        available_memory_size += node->size;
      }
      node = node->next;
    }
    return available_memory_size;
}

void print_statistics() {
    int allocated_size = 0;
    int allocated_chunks = 0;
    int free_size = 0;
    int free_chunks = 0;
    int smallest_free_chunk_size = kallocator.size;
    int largest_free_chunk_size = 0;

    // Calculate the statistics
    struct nodeStruct* node = kallocator.klist_head;

    while(node != NULL){
      //free chunk
      if(!node->allocated){
        free_size = free_size + node->size;
        free_chunks++;
        if(smallest_free_chunk_size > node->size){
          smallest_free_chunk_size = node->size;
        }
        if(largest_free_chunk_size < node->size){
          largest_free_chunk_size = node->size;
        }
      }
      //allocated chunks
      else{
        allocated_size += node->size;
        allocated_chunks++;
      }
      node = node->next;
    }
    //case when there are no free chunks
    if(free_chunks == 0){
      smallest_free_chunk_size = 0;
    }

    printf("Allocated size = %d\n", allocated_size);
    printf("Allocated chunks = %d\n", allocated_chunks);
    printf("Free size = %d\n", free_size);
    printf("Free chunks = %d\n", free_chunks);
    printf("Largest free chunk size = %d\n", largest_free_chunk_size);
    printf("Smallest free chunk size = %d\n", smallest_free_chunk_size);
}
