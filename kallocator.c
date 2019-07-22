#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "kallocator.h"

/* ------------- Linked list implementation ------------- */

struct nodeStruct {
    bool allocated;
    int size;
    size_t location;
    struct nodeStruct *next;
};

//initializes the first node of meta data list
struct nodeStruct* initialize_list(int size){
  struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
  if (new_node != NULL){
    new_node->size = size;
    new_node->allocated = false;
    new_node->location = 0;
  }
  return new_node;
}

size_t* allocate_memory(struct nodeStruct **headRef, int size, allocation_algorithm type){
  switch(type){
    case FIRST_FIT:
      struct nodeStruct* node = *headRef;
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
            if(node->next->allocated){
              struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
              new_node->size = remaining_memory;
              new_node->allocated = false;
              new_node->location = node->location + size;
              new_node->next = node->next;
              node->next = new_node;
              return &node->location;
            }
            //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
            else{
              node->next->size = node->next->size + remaining_memory;
              return &node->location;
            }
          }
          //case where it is a perfect fit just return
          return &node->location;
        }
        //case where free memory chunk is not big enough for the desired size loop again
        else{
          node = node->next;
        }
      }
      return NULL;

    case BEST_FIT:
      struct nodeStruct* node = *headRef;
      struct nodeStruct* best_fit_node = node;
      int smallest_size_difference = size;

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
        return NULL;
      }
      //after iterating through entire list, allocate memory in the smallest space
      best_fit_node->allocated = true;
      int remaining_memory = best_fit_node->size - size;
      if(remaining_memory > 0){
        //if the next node is a not a free chunk create a node
        if(best_fit_node->next->allocated){
          struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
          new_node->size = remaining_memory;
          new_node->allocated = false;
          new_node->location = best_fit_node->location + size;
          new_node->next = best_fit_node->next;
          best_fit_node->next = new_node;
          return &best_fit_node->location;
        }
        //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
        else{
          best_fit_node->next->size = node->next->size + remaining_memory;
          return &best_fit_node->location;
        }
      }
      else{
        return &best_fit_node->location;
      }

    case WORST_FIT:
      struct nodeStruct* node = *headRef;
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
        return NULL;
      }
      //after iterating through entire list, allocate memory in the smallest space
      worst_fit_node->allocated = true;
      int remaining_memory = worst_fit_node->size - size;
      if(remaining_memory > 0){
        //if the next node is a not a free chunk create a node
        if(worst_fit_node->next->allocated){
          struct nodeStruct *new_node = malloc(sizeof(struct nodeStruct));
          new_node->size = remaining_memory;
          new_node->allocated = false;
          new_node->location = worst_fit_node->location + size;
          new_node->next = worst_fit_node->next;
          worst_fit_node->next = new_node;
          return &worst_fit_node->location;
        }
        //if the next node is a free chunk combine values to prevent contiguous free chunks of memory
        else{
          worst_fit_node->next->size = node->next->size + remaining_memory;
          return &worst_fit_node->location;
        }
      }
      else{
        return &worst_fit_node->location;
      }
  } //switch
}//function

//frees up all the nodes of the linked list
void delete_list(struct nodeStruct **headRef){

}

void free_memory(void* ptr, struct nodeStruct **headRef){
  struct nodeStruct* node = *headRef;
  while(node != NULL){
    //if ptr is pointing to the node location
    if(&kallocator.memory[node->location] == ptr){
      //if next node is allocated or null just free current node
      if(node->next->allocated || node->next == NULL){
        node->allocated = false;
      }
      //if next node is a free node combine nodes to prevent contiguous free memory
      else{
        int tmp_size = node->next->size;
        node->size = node->size + tmp_size;
        node->next = node->next->next;
        free(node->next);
      }
    }
    node = node->next;
  }
}

/* ------------- Memory allocation implementaion ------------- */

struct KAllocator {
    enum allocation_algorithm aalgorithm;
    int size;
    void* memory;
    // Some other data members you want,
    // such as lists to record allocated/free memory
};

struct KAllocator kallocator;
struct nodeStruct klist_head;


void initialize_allocator(int _size, enum allocation_algorithm _aalgorithm) {
    assert(_size > 0);
    kallocator.aalgorithm = _aalgorithm;
    kallocator.size = _size;
    kallocator.memory = malloc((size_t)kallocator.size);

    // Add some other initialization
    klist_head = initialize_list(size);
}

void destroy_allocator() {
    free(kallocator.memory);

    // free other dynamic allocated memory to avoid memory leak
    delete_list(klist_head);
}

void* kalloc(int _size) {
    void* ptr = NULL;

    //check if there is enough space
    if()


    // Allocate memory from kallocator.memory
    size_t location = allocate_memory(klist_head, _size, kallocator.aalgorithm);
    ptr = &kallocator.memory[location];

    // ptr = address of allocated memory
    return ptr;
}

void kfree(void* _ptr) {
    assert(_ptr != NULL);


}

int compact_allocation(void** _before, void** _after) {
    int compacted_size = 0;

    // compact allocated memory
    // update _before, _after and compacted_size

    return compacted_size;
}

int available_memory() {
    int available_memory_size = 0;
    // Calculate available memory size
    struct nodeStruct node = klist_head;
    while(node != NULL){
      if (node->allocated == false){
        available_memory_size = available_memory_size + node->size;
      }
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
    struct nodeStruct node = klist_head;
    while(node != NULL){
      //free chunk
      if(node->allocated == false){
        free_size = free_size + node->size;
        free_chunks++;
        if(smallest_free_chunk_size > node->size){
          smallest_free_chunk_size = node->size;
        }
        if(largest_free_chunk_size < node->size){
          largest_free_chunk_size = node->size;
        }
      }
      else{
        allocated_size = allocated_size + node->size;
        allocated_chunks++;
      }
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
