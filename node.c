#include "node.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct Node Node;

// function to create the node itself
Node *node_create(uint8_t symbol, uint64_t frequency) {
  // allocate memory for the node
  Node *node = (Node *)malloc(sizeof(Node));
  // check to see that the memory has been properly allocate for the node being
  // created
  if (node) {
    node->symbol = symbol;
    node->frequency = frequency;
    // set the left and right nodes of the main node to NULL
    node->left = NULL;
    node->right = NULL;
    // if the node wasn't initially allocated correctly, free the node and
    // return the NULL node
  } else {
    free(node);
    node = NULL;
  }
  // return the node
  return node;
}

// used to delete the entire node
void node_delete(Node **n) {
  // make sure that the node being passed in isn't NULL
  if (*n != NULL) {
    (*n)->left = NULL;
    (*n)->right = NULL;
    free(*n);
    *n = NULL;
  }
}

// function to create a parent node between 2 children passed in
Node *node_join(Node *left, Node *right) {
  // make sure the children aren't NULL
  if (left != NULL && right != NULL) {
    // create the parent node
    Node *parent;
    uint8_t symbol = '$';
    parent = node_create(symbol, (left->frequency) + (right->frequency));
    // make sure that the parent was created properly
    if (parent == NULL) {
      node_delete(&parent);
      return parent;
    } else {
      // link the two children to the paren
      parent->left = left;
      parent->right = right;
      return parent;
    }
  }
  return NULL;
}

// helper function to debug
void node_print(Node *n) {
  // check which type of symbol to print with each node
  if (n != NULL) {
    if (iscntrl(n->symbol) == 1) {
      printf("0x%02X ", n->symbol);
    } else {
      printf("%c ", n->symbol);
    }
    // print the node's frequency
    printf("%lu\n", n->frequency);
  }
}

// function to compare node's frequency
bool node_cmp(Node *n1, Node *n2) {
  // return true if the left node's frequency is greater than the right's
  if (n1 != NULL && n2 != NULL) {
    if (n1->frequency > n2->frequency) {
      return true;
    }
    return false;
  }
  return false;
}

// debugging function to only print the node's symbol
void node_print_sym(Node *n) {
  if (n != NULL) {
    if (iscntrl(n->symbol) == 1) {
      printf("0x%02X\n", n->symbol);
    } else {
      printf("%c\n", n->symbol);
    }
  }
}

/*
int main(void){
        Node* n;
        n = node_create('d', 5);
        //node_print(n);
        Node* test;
        test = node_create('e', 8);
        Node* parent;
        parent = node_join(n, test);
        node_print(parent);
        Node* join;
        join = node_create('s', 10);
        join = node_join(parent, join);
        node_print(join);
        //node_delete(&n);
        //node_print(n);
        node_print(join->left);
        printf("%i\n", node_cmp(join, parent));
        node_print_sym(join);
}
*/
