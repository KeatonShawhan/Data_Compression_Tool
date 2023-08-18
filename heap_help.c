#include "heap_help.h"
#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// typedef struct Node Node;

void swap(Node *n1, Node *n2) {
  Node temp;
  temp = *n1;
  *n1 = *n2;
  *n2 = temp;
}

// create the left child helper function
uint32_t l_child(uint32_t n) { return ((2 * n) + 1); }

// create the right child helper function
uint32_t r_child(uint32_t n) { return ((2 * n) + 2); }

// create the parent helper function
uint32_t parent(uint32_t n) { return ((n - 1) / 2); }

// create the up_heap helper function, passing in the current element index (n)
// and the array
void up_heap(Node **arr, uint32_t n) {
  // loop while the element index is > 0 and the array's value of the index n is
  // less than the array's value of the parent function's return value
  while ((n > 0) && (node_cmp(arr[n], arr[parent(n)]) == false)) {
    // node_print(arr[n]);
    // node_print(arr[parent(n)]);
    //  swap the address of the array's value of the index n and the address of
    //  the array's value of the parent function's return value
    swap(arr[n], arr[parent(n)]);
    // update the element index n with the parent value
    n = parent(n);
    // loop again
  }
}

// create the down_heap helper function, assing in the current element index n
// and the array
void down_heap(Node **arr, uint32_t heap_size) {
  // initialize the element index n and the smaller value of the childs
  uint32_t n = 0;
  uint32_t smaller = 0;
  // loop while the left child function's return value of the element index is
  // less than the current heap size
  while (l_child(n) < heap_size) {
    // check if the right child function's return value is equal to the current
    // heap size
    if (r_child(n) == heap_size) {
      // if so, update the smaller variable to the left child function's return
      // value
      smaller = l_child(n);
      // if not:
    } else {
      // check if the array's value of the left child function's return value of
      // the element index n is less than the array's value of the right child
      // function's return value of the element index n
      if (node_cmp(arr[l_child(n)], arr[r_child(n)]) == false) {
        // if so, update the smaller variable to the left child function's
        // return value of the element index n
        smaller = l_child(n);
        // if not:
      } else {
        // update the smaller variable to the right child function's return
        // value of the element index n
        smaller = r_child(n);
      }
    }
    // check if the array's value of element index n is less than the array's
    // value of the smaller variable
    if (node_cmp(arr[n], arr[smaller]) == false) {
      // break out of the initial loop
      break;
    }
    // swap the addresses of the array's value of the element index n with the
    // array's value of the smaller variable
    swap(arr[smaller], arr[n]);
    // update the element index n with the smaller variable
    n = smaller;
  }
}

/*
int main(void){
        Node* l;
        Node* r;
        Node* m;
        Node* f;
        Node* z;
        l = node_create('l', 122);
        r = node_create('r', 5);
        m = node_create('m', 1);
        f = node_create('f', 78);
        z = node_create('z', 220);
        //Node **arr[3];
        Node **arr = (Node **)calloc(5, sizeof(Node*));
        arr[0] = l;
        up_heap(arr, 0);
        arr[1] = r;
        up_heap(arr, 1);
        arr[2] = m;
        up_heap(arr, 2);
        arr[3] = f;
        up_heap(arr, 3);
        arr[4] = z;
        up_heap(arr, 4);
        node_print(arr[0]);
        swap(arr[0], arr[4]);
        arr[4] = 0;
        down_heap(arr, 4);
        node_print(arr[0]);
        swap(arr[0], arr[3]);
        arr[3] = 0;
        down_heap(arr, 3);
        node_print(arr[0]);
        swap(arr[0], arr[2]);
        arr[2] = 0;
        down_heap(arr, 2);
        node_print(arr[0]);
        swap(arr[0], arr[1]);
        arr[1] = 0;
        down_heap(arr, 1);
        node_print(arr[0]);

}
*/
