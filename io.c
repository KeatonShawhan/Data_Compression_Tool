#include "io.h"
#include "code.h"
#include "defines.h"
#include "heap_help.h"
#include "node.h"
#include "pq.h"
#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// initialize all the static variables
uint64_t bytes_read = 0;
uint64_t bytes_written = 0;

static uint32_t pos = 0;
static uint8_t buf_block[4096];
static uint8_t code_block[BLOCK];
static uint32_t amount = 0;

// function to read a desired amount of bytes from an infile, filling up a
// buffer array
int read_bytes(int infile, uint8_t *buf, int nbytes) {
  if (true) {
    int total = 0;
    // loop
    while (true) {
      // if the total amount of bytes read is greater than or equal to the
      // desired amount of bytes (nbytes), break
      if (total >= nbytes) {
        break;
      }
      // call read() repeatedly until the desired amount of nbytes has been
      // completed
      int count = read(infile, buf + total, nbytes - total);
      // if read() returns -1, return -1
      if (count == -1) {
        return -1;
      }
      // add the return value of read() to the bytes_read extern variable for
      // later usage
      bytes_read += count;
      // add the return value of read() to the total variable to keep track of
      // how many bytes still need to be read
      total += count;
      // if read() returns 0, break;
      if (count == 0) {
        break;
      }
    }
    // return the total amount of bytes read
    return total;
  }
}

// function to write bytes from a buffer of uint8_ts to an outfile
int write_bytes(int outfile, uint8_t *buf, int nbytes) {
  if (true) {
    int total = 0;
    // loop infinitely
    while (true) {
      // same thing as read_bytes()
      if (total >= nbytes) {
        break;
      }
      // same thing as read_bytes(), except with write() instead of read()
      int count = write(outfile, buf, nbytes - total);
      // same as read_bytes()
      if (count == -1) {
        return -1;
      }
      // keep track of the bytes written for later usage
      bytes_written += count;
      // same as read_bytes()
      total += count;
      if (count == 0) {
        break;
      }
    }
    // return the total amount of bytes written
    return total;
  }
}

// function to read each individual bit from an infile, returning the bit
// through the parameters
bool read_bit(int infile, uint8_t *bit) {
  // check to see if the buffer block needs to be reset if 4096 bytes have been
  // read (*8 for bits)
  if ((pos % (4096 * 8)) == 0) {
    // zero out the buffer block
    for (uint64_t i = 0; i < 4096; i++) {
      buf_block[i] = 0;
    }
    // fill the block again with read_bytes()
    uint32_t out = read_bytes(infile, buf_block, 4096);
    pos = 0;
    if (out == 0) {
      return false;
    }
    // reset the bit position static variable
  }
  // find the element containing the bit and the index of the bit itself
  uint32_t ele = (pos / 8);
  uint32_t i = (pos % 8);
  uint8_t copy = buf_block[ele];
  copy |= 1UL << i;
  // use bit shifting to see if the bit at that index is 1 or 0
  // update the bit
  if (buf_block[ele] != copy) {
    *bit = 0;
  } else {
    *bit = 1;
  }
  // if the element that the bit is in is 0, then the block is empty at that
  // index
  // if (buf_block[ele] == 0){
  //	return false;
  //}
  pos += 1;
  return true;
}

// function to write the bits to a buffer array of bytes representing codes
void write_code(int outfile, Code *c) {
  uint8_t bit;
  pos = 0;
  // loop while the static bit position variable is less than the code passed in
  while (pos < code_size(c)) {
    // check if the bit is 1 or 0
    if (code_get_bit(c, pos) == true) {
      // if its 1, set the bit at the index of the total bits read (amount) to 1
      uint32_t ele = (amount / 8);
      uint32_t i = (amount % 8);
      code_block[ele] |= 1UL << i;
    } else {
      // if its 0, set the bit at the index of the total bits read (amount) to 0
      uint32_t ele = (amount / 8);
      uint32_t i = (amount % 8);
      code_block[ele] &= ~(1UL << i);
    }
    // increase the local counter for the bits read (pos) and the total counter
    // (amount)
    pos += 1;
    amount += 1;
    // if the total counter of bits read is a full block, call write_bytes() to
    // write each altered byte to the outfile
    if (amount == (BLOCK * 8)) {
      write_bytes(outfile, code_block, BLOCK);
      // reset the total bytes counter
      amount = 0;
      // zero out every element in the buffer array
      for (uint64_t i = 0; i < 4096; i++) {
        code_block[i] = 0;
      }
    }
  }
}

// function to flush out any other codes that didn't fill a full block
void flush_codes(int outfile) {
  // check if the total amount of bits is divisable by 8, if it is, just call
  // write_bytes() normally, but if not, add 1 to the amount of bytes to be read
  // because it wasn't a clean byte
  (amount % 8 == 0) ? write_bytes(outfile, code_block, amount / 8)
                    : write_bytes(outfile, code_block, (amount / 8) + 1);
}

/*
int main(void){
        uint8_t *buf = (uint8_t *)calloc(4096, sizeof(uint8_t));
        int nbytes = 4096;
        int f = open("iotest.txt", S_IRUSR|S_IWUSR);
        int n = read_bytes(f, buf, nbytes);
        //printf("bytes read: %i\n", n);
        for (uint64_t i = 0; i < n; i++){
                printf("%u ", buf[i]);
        }
        int o = open("out.txt", O_WRONLY);
        write_bytes(o, buf, n);
        //printf("\n\nbytes written: %i\n", x);
        int rb = open("read_bit.txt", S_IRUSR|S_IWUSR);
        uint8_t bit;
        uint64_t i = 0;
        bytes_read = 0;
        while (read_bit(rb, &bit)){
                printf("%u", bit);
                if (((i+1) % 8 == 0) && (i != 0)){
                        printf(" ");
                }
                i += 1;
        }
        printf("\n");
        int wc = open("write_code.txt", O_WRONLY);
        Code c = code_init();
        int i = 0;
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        //
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        //
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        //
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        //
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        //
        while (i < 6144){
                write_code(wc, &c);
                i += 1;
        }
        //code_print(&c);
        flush_codes(wc);
}
*/
