How to compile: To compile this program, simply use the "make" command. This will create an executable file for the encode.c and decode.c test harnesses called "encode" and "decode" respectively. Now the executables can be ran with default values like so: "./encode" or "./decode". Included in the executable are specific command-line options pertaining to the usage of the harness.



For the encode executable, the command line options are:
  

  -h             Program usage and help.
  -v             Print compression statistics.
  -i infile      Input file to compress.
  -o outfile     Output of compressed data.



For the decode executable, the command line options are:


  -h             Program usage and help.
  -v             Print compression statistics.
  -i infile      Input file to decompress.
  -o outfile     Output of decompressed data.



File Descriptions:


"README.md" is this file.


"encode.c": Contains the implementation of the Huffman encoder.


"decode.c": Contains the implementation of the Huffman decoder.


"defines.h": This file will contain the macro definitions used throughout the program.


"header.h": This will will contain the struct definition for a file header.


"node.h": This file will contain the node ADT interface.


"node.c": This file will contain the implementation of the node ADT.


"pq.h": This file will comntain the priority queue ADT interface.


"pq.c": This file will contain the implementation of the priority queue ADT.


"code.h": This file will contain the code ADT interface.


"code.c": This file will contain the implementation of the code ADT.


"io.h": This file will contain the I/O module interface. 


"io.c": This file will contain the implementation of the I/O module.


"stack.h": This file will contain the stack ADT interface.


"stack.c": This file will contain the implementation of the stack ADT.


"huffman.h": This file will contain the Huffman coding module interface.


"huffman.c": This file will contain the implementation of the Huffman coding module interface.


"Makefile": This file will allow the user to compile the program into executables.


“DESIGN.pdf” is a file that contains pseudocode for each function.



