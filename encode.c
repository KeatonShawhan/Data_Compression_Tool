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
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// parse through each input character
int main(int argc, char **argv) {
  // initialize the default values
  // set default values
  bool verbose = false;
  // make a Header struct 'Header'
  Header Header;
  // make a stat struct b to hold the stats from the infile
  struct stat b;
  // default the input and output sources to stdin and stdout
  int input = STDIN_FILENO;
  int output = STDOUT_FILENO;
  // make a temporary file for the stdin
  FILE *t = tmpfile();
  int tt = fileno(t);
  // bool print_bool = false;
  int opt = 0;
  // loop, checking which option was used and if it was in the list of options
  while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
    // allows a variable to be tested for equality against a list of values
    switch (opt) {
    case 'i':
      // set the file to encrypt to the option passed in through the command
      // line
      input = open(optarg, O_RDONLY);
      break;
    case 'o':
      // set the private file to the string passed in through the command line
      output = open(optarg, O_WRONLY | O_CREAT);
      break;
    case 'v':
      // turn the verbose flag to true for printing later
      verbose = true;
      break;
    case 'h':
      // help message
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 0;
    default:
      // if an option is used than isn't stated above, return a non-zero error
      // print out helper option (given)
      fprintf(stderr, "SYNOPSIS\n");
      fprintf(stderr, "  A Huffman encoder.\n");
      fprintf(stderr,
              "  Compresses a file using the Huffman coding algorithm.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "USAGE\n");
      fprintf(stderr, "  ./encode [-h] [-i infile] [-o outfile]\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "OPTIONS\n");
      fprintf(stderr, "  -h             Program usage and help.\n");
      fprintf(stderr, "  -v             Print compression statistics.\n");
      fprintf(stderr, "  -i infile      Input file to compress.\n");
      fprintf(stderr, "  -o outfile     Output of compressed data.\n");
      return 1;
    }
  }
  // make a histogram array of size 256
  uint64_t *hist = (uint64_t *)calloc(ALPHABET, sizeof(uint64_t));
  // zero out the array (don't know if neccessary)
  for (uint64_t i = 0; i < ALPHABET; i++) {
    hist[i] = 0;
  }
  // make a buffer array of size 1 for each byte read from the infile)
  uint8_t *buf = (uint8_t *)calloc(1, sizeof(uint8_t));
  // loop while read_bytes can still read a byte
  while (read_bytes(input, buf, 1) != 0) {
    // update the histogram with the value read by 1
    hist[buf[0]] += 1;
    // if the input file is still the default (stdin), write the bytes to the
    // temp file tt
    if (input == STDIN_FILENO) {
      write_bytes(tt, buf, 1);
    }
    // clear the buf (dont know if needed)
    buf[0] = 0;
  }
  // reset the bytes_written extern for stats later
  bytes_written = 0;
  // if the input is still stdin, set the input to the tempfile tt
  if (input == STDIN_FILENO) {
    input = tt;
  }
  // (check given in assignment doc)
  if ((hist[0] == 0) && (hist[1] == 0)) {
    hist[0] = 1;
    hist[1] = 1;
  }
  // make a counter for the unique characters in the histogram
  uint64_t unique = 0;
  // loop through the histogram and add 1 to the unique counter if the frequency
  // isn't 0
  for (uint64_t i = 0; i < ALPHABET; i++) {
    if (hist[i] != 0) {
      unique += 1;
    }
  }
  // make a root node that is returned from build_tree()
  Node *root;
  root = build_tree(hist);
  // make a Code table to hold the tree paths for each char
  Code table[ALPHABET];
  // make each Code inside the table
  for (uint64_t i = 0; i < ALPHABET; i++) {
    table[i] = code_init();
  }
  // build each of the codes from the root node, filling the table
  build_codes(root, table);
  // get the stats from the input file
  fstat(input, &b);

  // set the header magic number, permissions, tree size, and the file size
  Header.magic = MAGIC;
  Header.permissions = b.st_mode;
  // set the output file's permissions to the input file's with fchmod()
  fchmod(output, Header.permissions);
  Header.tree_size = (3 * unique) - 1;
  Header.file_size = b.st_size;
  // write the headers to the outfile, casting them as a uint8_t because
  // write_bytes() takes a buffer of type uint8_t
  write_bytes(output, (uint8_t *)&Header.magic, 4);
  write_bytes(output, (uint8_t *)&Header.permissions, 2);
  write_bytes(output, (uint8_t *)&Header.tree_size, 2);
  write_bytes(output, (uint8_t *)&Header.file_size, 8);
  // dump the tree
  dump_tree(output, root);
  // reset the file pointer
  lseek(input, 0, SEEK_SET);
  // read every byte from the input file and write the code to the output file
  bytes_read = 0;
  while (read_bytes(input, buf, 1) != 0) {
    write_code(output, &table[buf[0]]);
  }
  // flush the semi-full buffer if there is one
  flush_codes(output);
  // if the user enables the -v option, print out the stats
  if (verbose == true) {
    fprintf(stderr, "Uncompressed file size: %ld bytes\n", bytes_read);
    fprintf(stderr, "Compressed file size: %ld bytes\n", bytes_written);
    fprintf(stderr, "Space saving: %-.2f%%\n",
            (100 * (1 - ((float)bytes_written / (float)bytes_read))));
  }
  // delete all of the objects, arrays, and close the files to stop memory leaks
  delete_tree(&root);
  free(hist);
  free(buf);
  close(input);
  close(tt);
  close(output);
  return 0;
}
