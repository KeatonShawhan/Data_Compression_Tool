#include "heap_help.h"
#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// PriorityQueue struct to hold the max capacity and the current size
typedef struct PriorityQueue {
  uint32_t capacity;
  uint32_t size;
  Node **arr;
} PriorityQueue;

// function to create the actual PQ, specifying the desired capacity of it
PriorityQueue *pq_create(uint32_t capacity) {
  // allocate memory for the actual PQ object
  PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  // make sure that it allocated correctly
  if (q != NULL) {
    // allocate memory for the actual PQ array inside the stuct with enough
    // space for the desired capacity
    q->arr = (Node **)calloc(capacity, sizeof(Node *));
    // make sure that the array allocated properly
    if (q->arr != NULL) {
      q->size = 0;
      q->capacity = capacity;
    } else {
      free(q->arr);
      q->arr = NULL;
      free(q);
      q = NULL;
      return q;
    }
  } else {
    // free and set to NULL if there was an allocation error
    free(q);
    q = NULL;
  }
  // return the object
  return q;
}

// helper function to return the actual amount of elements in the PQ array
uint32_t pq_size(PriorityQueue *q) {
  if (q != NULL) {
    return q->size;
  }
  return 0;
}

// function to delete the pq object to stop memory leaks
void pq_delete(PriorityQueue **q) {
  // make sure that the object and the array inside of it actually exist so you
  // don't accidently clear something that doesn't need to be
  if (*q != NULL) {
    // make sure that the array inside the object isn't NULL
    if ((*q)->arr != NULL) {
      // free the memory for the array
      free((*q)->arr);
      (*q)->arr = NULL;
    }
    // free the memory for the priority queue
    free(*q);
    q = NULL;
  }
}

// helper function to determine if the pq is empty
bool pq_empty(PriorityQueue *q) {
  // make sure that the pq object isn't NULL
  if (q != NULL) {
    // return true if the size of the pq is 0, else, false
    if (q->size == 0) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// helper function to determine if the pq is full
bool pq_full(PriorityQueue *q) {
  // make sure that the pq object isn't NULL
  if (q != NULL) {
    // return true is the size of the pq is equal to the max capacity, else,
    // false
    if (q->size == q->capacity) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// function to actually enqueue a Node into the pq object
bool enqueue(PriorityQueue *q, Node *n) {
  // make sure that the pq isn't NULL
  if ((q != NULL) && (n != NULL)) {
    // check that the pq isn't full because you can't enqueue a Node if the
    // object hits its max capacity
    if (pq_full(q) == true) {
      return false;
    } else {
      // set the next index without a node to the one passed in
      q->arr[q->size] = n;
      // call up_heap() to fix the priority queue
      up_heap(q->arr, q->size);
      // increase the size of the pq
      q->size += 1;
      return true;
    }
  } else {
    return false;
  }
}

// function to take a node out of the pq object
bool dequeue(PriorityQueue *q, Node **n) {
  // make sure that the pq isn't NULL
  if (q != NULL) {
    // make sure that the qp isn't empty (can't take a node if there aren't any)
    if (pq_empty(q) == true) {
      return false;
    } else {
      // swap the first node and the last node in the pq
      swap(q->arr[0], q->arr[q->size - 1]);
      // set the output node to the original last one
      *n = q->arr[q->size - 1];
      // clear the node
      q->arr[q->size - 1] = 0;
      // decrease the size of the pq
      q->size -= 1;
      // call down_heap() to fix the priority queue again
      down_heap(q->arr, q->size);
      return true;
    }
  } else {
    return false;
  }
}

// debugging helper function to print the contents of the priority queue
void pq_print(PriorityQueue *q) {
  if (q != NULL) {
    for (uint32_t i = 0; i < pq_size(q); i++) {
      // printf("hi\n");
      node_print(q->arr[i]);
      printf("\n");
    }
  }
}

/*
int main(void){
        PriorityQueue *q = pq_create(10);
        Node *a = node_create('a', 4);
        Node *b = node_create('b', 19);
        Node *c = node_create('c', 2);
        Node *d = node_create('d', 25);
        Node *e = node_create('e', 12);

        printf("Enqueue two:\n");
        enqueue(q, a);
        enqueue(q, b);
        pq_print(q);
        printf("\n");
        printf("Enqueue three:\n");
        enqueue(q, c);
        enqueue(q, d);
        enqueue(q, e);
        pq_print(q);
        printf("\n");
        printf("Dequeue one:\n");
        dequeue(q, &a);
        pq_print(q);
        pq_delete(&q);
}
*/
