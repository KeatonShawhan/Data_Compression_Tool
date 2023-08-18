#include "code.h"
#include "defines.h"
#include "header.h"
#include "heap_help.h"
#include "huffman.h"
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
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// debug function
void print_tree(Node *n) {
  if (n != NULL) {
    print_tree(n->left);
    print_tree(n->right);
    node_print(n);
  }
}

// parse through each input character
int main(int argc, char **argv) {
  // set default values
  bool verbose = false;
  // create a header struct to hold all of the input file's header
  Header Header;
  // set default values for the stdin and stdout
  int input = STDIN_FILENO;
  int output = STDOUT_FILENO;
  int opt = 0;
  // loop, checking which option was used and if it was in the list of options
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    // allows a variable to be tested for equality against a list of values
    switch (opt) {
    case 'i':
      // set the input file with whatever name the user chooses
      input = open(optarg, O_RDONLY);
      break;
    case 'o':
      // set the output file with whatever name the user chooses (make sure to
      // create the file if it doesn't exist)
      output = open(optarg, O_WRONLY | O_CREAT);
      break;
    case 'v':
      // turn the verbose flag to true for printing stats later
      verbose = true;
      break;
    case 'h':
      // help messsage
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 0;
    default:
      // if the user tries to call ./decode with an argument that doesn't exist,
      // print a help message and return a non-zero number
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman decoder.\n");
      fprintf(stderr,
              "  Decompresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./decode [-h] [-i infile] [-o outfile]\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to decompress.\n");
      fprintf(stderr, "  -o outfile     Output of decompressed data.\n");
      return 1;
    }
  }
  // make a buffer array to hold the header information
  uint8_t *buf = (uint8_t *)calloc(BLOCK, sizeof(uint8_t));
  // read magic number (4 bytes from the compressed file)
  read_bytes(input, buf, 4);
  // copy the memory from the 4 bytes in the buffer into the new Header's magic
  // number
  memcpy((uint32_t *)&Header.magic, buf, 4);
  // clear the 4 bytes used
  for (uint8_t i = 0; i < 4; i++) {
    buf[i] = 0;
  }
  // verify that the magic number is correct
  if (Header.magic != MAGIC) {
    fprintf(stderr, "Invalid magic number.\n");
    return 1;
  }
  // read the permissions from the infile (2 bytes)
  read_bytes(input, buf, 2);
  // copy the memory from the 2 bytes in the buffer into the new Header's
  // permissions value
  memcpy((uint16_t *)&Header.permissions, buf, 2);
  // clear the 2 bytes used
  buf[0] = 0;
  buf[1] = 1;
  // read the size of the tree from the infile (2 bytes)
  read_bytes(input, buf, 2);
  // copy the memory from the 2 bytes in the buffer into the new Header's
  // tree_size value
  memcpy((uint16_t *)&Header.tree_size, buf, 2);
  // clear the 2 bytes used
  buf[0] = 0;
  buf[1] = 1;
  // read the size of the file from the infile header (8 bytes)
  read_bytes(input, buf, 8);
  // copy the memory from the 8 bytes in the buffer into the new Header's file
  // size value
  memcpy((uint64_t *)&Header.file_size, buf, 8);
  // clear the 8 bytes used
  for (uint8_t i = 0; i < 8; i++) {
    buf[i] = 0;
  }
  // update the permissions of the outfile with the infile's permissions that
  // were read
  fchmod(output, Header.permissions);
  // read the tree_size from the header, allocating memory for the
  // rebuild_tree() call
  uint8_t *ts = (uint8_t *)calloc(Header.tree_size, sizeof(uint8_t));
  // zero out the allocated memory (don't know if necessary)
  for (uint64_t i = 0; i < Header.tree_size; i++) {
    ts[i] = 0;
  }
  // read bytes into the tree size buffer
  read_bytes(input, ts, Header.tree_size);
  // get the root node from the tree
  Node *root;
  root = rebuild_tree(Header.tree_size, ts);
  // print_tree(root);
  //  make a copy of the root node to use for walking down the tree
  Node *cur = root;
  // loop while the bytes written is still less than the size of the input file
  while (bytes_written < Header.file_size) {
    uint8_t bit;
    // read each bit from the tree
    read_bit(input, &bit);
    // if the bit is 0, walk left, if 1, walk right
    if (bit == 0) {
      cur = cur->left;
    } else {
      cur = cur->right;
    }
    // if the node is a leaf
    if ((cur->left == NULL) && (cur->right == NULL)) {
      // write the symbol of the current node to the outfile
      write_bytes(output, &cur->symbol, 1);
      // reset the copy node to the root
      cur = root;
    }
  }
  // if the -v option is used by the user, print out the statistics of the file
  // compression
  if (verbose == true) {
    fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
    fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);
    fprintf(stderr, "Space saving: %.2f%%\n",
            (100 * (1 - ((float)bytes_read / (float)bytes_written))));
  }
  // close files, free arrays, and delete the root to stop memory leaks
  delete_tree(&root);
  free(buf);
  free(ts);
  close(input);
  close(output);
  return 0;
}
