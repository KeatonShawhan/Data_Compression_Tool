#include "code.h"
#include "defines.h"
#include "heap_help.h"
#include "node.h"
#include "pq.h"
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// typedef struct Code Code;

// function to create the Code itself
Code code_init(void) {
  // initialize variables
  Code code;
  code.top = 0;
  return code;
}

// function to just return the amount of elements inside the Code struct
uint32_t code_size(Code *c) { return c->top; }

// function to check if the Code is empty (top == 0)
bool code_empty(Code *c) {
  if (code_size(c) == 0) {
    return true;
  } else {
    return false;
  }
}

// function to check if the Code isn't empty (top != the max size of a code)
bool code_full(Code *c) {
  if (code_size(c) == MAX_CODE_SIZE * 8) {
    return true;
  } else {
    return false;
  }
}

// function to set the bit at index i to 1
bool code_set_bit(Code *c, uint32_t i) {
  // make sure the index is in a valid range
  if (i > code_size(c)) {
    return false;
  } else {
    // find the index of the bit itself
    uint32_t ele = i / 8;
    uint32_t bit = i % 8;
    // set the bit to 1 and return true
    c->bits[ele] |= 1UL << bit;
    return true;
  }
}

// function to set the bit at index i to 0
bool code_clr_bit(Code *c, uint32_t i) {
  // make sure the index is in a valid range
  if (i > code_size(c)) {
    return false;
  } else {
    // find the index of the bit itself
    uint32_t ele = i / 8;
    uint32_t bit = i % 8;
    // set the bit to 0 and return true
    c->bits[ele] &= ~(1UL << bit);
    return true;
  }
}

// function to get the bit's value at index i
bool code_get_bit(Code *c, uint32_t i) {
  // make sure the index is in a valid range
  if (i > code_size(c)) {
    return false;
  } else {
    // find the actual index of the bit
    uint32_t ele = i / 8;
    uint32_t bit = i % 8;
    uint8_t copy = c->bits[ele];
    // make a copy of the element holding the bit
    copy |= 1UL << bit;
    // printf("%u\n", copy);
    //  check if the element's value has changed by setting it, if it has we
    //  know the bit is 0, if not, we know the bit is 1
    if (c->bits[ele] != copy) {
      return false;
    } else {
      return true;
    }
  }
}

// function to push a bit onto the Code stack
bool code_push_bit(Code *c, uint8_t bit) {
  // make sure the stack isn't already full before pushing another bit
  if (code_full(c) == true) {
    // printf("full\n");
    return false;
  } else {
    // check whether to push a 0 or a 1
    if (bit == 1) {
      // push a bit (1) after the last pushed bit and increase the size/top of
      // the stack
      code_set_bit(c, c->top);
      c->top += 1;
      return true;
    } else {
      // push a bit (0) after the last pushed bit and increase the size/top of
      // the stack

      code_clr_bit(c, c->top);
      c->top += 1;
      return true;
    }
  }
}

// function to take out the last bit of the Code stack
bool code_pop_bit(Code *c, uint8_t *bit) {
  uint8_t one = 1;
  // printf("going to pop\n");
  //  make sure that the code stack isn't already empty (nothing to pop)
  if (code_empty(c) == true) {
    // printf("empty\n");
    return false;
  } else {
    // get the bit at the end of the Code stack to pass back out through the
    // parameter *bit
    if (code_get_bit(c, c->top - 1) == false) {
      *bit = 0;
    } else {
      *bit = one;
    }
    // clear the last pushed bit and decrease the size/top of the stack
    code_clr_bit(c, c->top - 1);
    c->top -= 1;
    return true;
  }
}

// debugging function to print the Code stack "bits"
void code_print(Code *c) {
  // loop through and get each bit, printing each index
  // printf("%u\n", code_size(c));
  for (uint32_t i = 0; i < code_size(c); i++) {
    // printf("%u\n", i);
    if (code_get_bit(c, i) == true) {
      printf("1 ");
    } else {
      printf("0 ");
    }
  }
  printf("\n");
}

/*
int main(void){
        Code c = code_init();
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 1);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_push_bit(&c, 0);
        code_print(&c);
        printf("\n\n");
        uint8_t bit;
        code_pop_bit(&c, &bit);
        //printf("%s\n", bit);
        code_print(&c);
        printf("\n\n");
        code_pop_bit(&c, &bit);
        code_pop_bit(&c, &bit);
        code_pop_bit(&c, &bit);
        code_print(&c);
        printf("\n\n");
        code_push_bit(&c, 1);
        code_print(&c);
        printf("\n\n");

}
*/
