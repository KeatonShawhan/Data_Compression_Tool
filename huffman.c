#include "huffman.h"
#include "code.h"
#include "defines.h"
#include "heap_help.h"
#include "io.h"
#include "node.h"
#include "pq.h"
#include "stack.h"
#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// make static variables to make recursive build_codes keep value
static uint8_t buff[MAX_CODE_SIZE];
static uint8_t total;

// function to build the actual tree
Node *build_tree(uint64_t hist[static ALPHABET]) {
  // allocate memory for a priority queue
  PriorityQueue *q = pq_create(ALPHABET);
  // loop through the histogram that was passed in
  for (uint64_t i = 0; i < ALPHABET; i++) {
    // if the frequency isn't 0, then we know a character is there
    if (hist[i] != 0) {
      // create a node for the character with the value at the index as the
      // frequency
      Node *n = node_create(i, hist[i]);
      // enqueue the ndoe into the priority queue
      enqueue(q, n);
    }
  }
  // loop through the size of the priority queue until there is only 1 node left
  while (pq_size(q) > 1) {
    // dequeue a left and right node from the pq
    Node *left;
    dequeue(q, &left);
    Node *right;
    dequeue(q, &right);
    // make a parent node between the two children
    Node *parent;
    parent = node_join(left, right);
    // enqueue the parent node back into the priority queue
    enqueue(q, parent);
  }
  // return the last node left in the pq (the root)
  Node *root;
  dequeue(q, &root);
  // delete the pq object
  pq_delete(&q);
  return root;
}

// function to build the code table with the path of the tree in bits
void build_codes(Node *root, Code table[static ALPHABET]) {
  // initialize an empty Code
  Code c = code_init();
  uint32_t i = 0;
  // loop through the total amount of bits in the static buffer array to hold
  // the previous path before hitting a leaf node
  while (i < total) {
    // fill the new code with whatever is in the buffer array
    code_push_bit(&c, buff[i]);
    i += 1;
  }
  // make sure the root isn't NULL
  if (root != NULL) {
    // check if the node is a leaf
    if ((root->left == NULL) && (root->right == NULL)) {
      // if the root is a leaf, add the code to the table passed in
      table[root->symbol] = c;
      // if the node isn't a leaf
    } else {
      // try to walk down the left side of the tree as far as possible
      uint8_t bit;
      code_push_bit(&c, 0);
      // update the buffer array and the size of the buffer array
      buff[total] = 0;
      total += 1;
      // call build_codes recursively again, trying to walk down the left child
      // this time
      build_codes(root->left, table);
      // once we've walked left as far as possible, we'll have an extra bit in
      // the code because we have to go back
      code_pop_bit(&c, &bit);
      // decrease the size of the static buffer array
      total -= 1;

      // same thing as above but walk down the right side as much as possible
      code_push_bit(&c, 1);
      // update the static buffer array with a 1
      buff[total] = 1;
      // update the size of the buffer array
      total += 1;
      // recursively call build_codes() again but pass in the right child this
      // time
      build_codes(root->right, table);
      // pop the last bit in the code once we can't go any more right because we
      // need to go back to the parent
      code_pop_bit(&c, &bit);
      // update the static buffer array size
      total -= 1;
    }
  }
}

// function to dump the tree when passed in the node and print to the outfile
void dump_tree(int outfile, Node *root) {
  // make sure that the node isn't NULL
  if (root != NULL) {
    // recursively call dump_tree(), trying to walk the left side as much as
    // possible
    dump_tree(outfile, root->left);
    // recursively call dump_tree(), trying to walk the right side as much as
    // possible
    dump_tree(outfile, root->right);
    // check if the node passed in has no children (leaf)
    if ((root->left == NULL) && (root->right == NULL)) {
      // make a buffer of size 2 to hold the leaf indicator 'L' and the symbol
      uint8_t buf[2];
      buf[0] = 'L';
      buf[1] = root->symbol;
      // write the L and the symbol to the outfile
      write_bytes(outfile, buf, 2);

    } else {
      // if the node isn't a leaf, make a buffer of size 1 and write the
      // internal node indicator 'I' to the outfile
      uint8_t buf[1];
      buf[0] = 'I';
      write_bytes(outfile, buf, 1);
    }
  }
}

// function to rebuild the tree once dumped
Node *rebuild_tree(uint16_t nbytes, uint8_t tree_dump[static nbytes]) {
  // create a stack of nodes
  Stack *s = stack_create(nbytes);
  // make a uint8_t casted L variable
  uint8_t L = 'L';
  uint8_t I = 'I';
  // loop through the desired amount of bytes passed in (nbytes)
  for (uint64_t i = 0; i < nbytes; i++) {
    // check if the byte at each index is an L (leaf indicator)
    if (tree_dump[i] == L) {
      // if so, create a node with the symbol of the leaf and any frequency
      // (doesn't really matter)
      Node *n = node_create(tree_dump[i + 1], 1);
      // push the node onto the stack and jump over the index of the symbol
      stack_push(s, n);
      i += 1;
    } else if (tree_dump[i] == I) {
      // if the char isn't an L, then it should be an internal node 'I'
      // create 3 nodes for the left, right, and parent
      Node *left;
      Node *right;
      Node *parent;
      // pop the right and left node from the stack
      stack_pop(s, &right);
      stack_pop(s, &left);
      // join the two nodes and update the parent node
      parent = node_join(left, right);
      // push the parent node onto the stack
      stack_push(s, parent);
    }
  }
  // get the last root left in the stack and return it
  Node *root;
  stack_pop(s, &root);
  // delete the stack object
  stack_delete(&s);
  return root;
}

// function to delete the tree
void delete_tree(Node **root) {
  // make sure the root isn't NULL
  if (*root != NULL) {
    // recursively call delete_tree with post-order traversal, deleting each
    // node
    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);
    node_delete(root);
  }
}

/*
int main(void){
        uint64_t hist[ALPHABET];
        uint64_t i = 0;
        while (i<ALPHABET){
                hist[i] = 0;
                i+=1;
        }
        hist[97] += 10;
        hist[99] += 8;
        hist[98] += 4;
        hist[101] += 7;
        Node *root;
        root = build_tree(hist);
        //node_print(root);
        Code table[ALPHABET];
        build_codes(root, table);
        printf("\n\n");
        int wc = open("write_code.txt", O_WRONLY);
        dump_tree(wc, root);
        int rbt = open("write_code.txt", S_IRUSR|S_IWUSR);
        uint16_t nbytes = 11;
        uint8_t tree_dump[nbytes];
        read_bytes(rbt, tree_dump, nbytes);
        Node *final;
        final = rebuild_tree(nbytes, tree_dump);
        //node_print(final);
        delete_tree(&final);
        //printTree(root, 10);
}
*/
