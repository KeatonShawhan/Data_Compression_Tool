#include "stack.h"
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

// create the Stack type structure
typedef struct Stack {
  uint32_t top;
  uint32_t capacity;
  Node **items;
} Stack;

// function to create the stack itself
Stack *stack_create(uint32_t capacity) {
  // allocate memory for the stack object itself
  Stack *stack = (Stack *)malloc(sizeof(Stack));
  // make sure the memory properly allocated
  if (stack != NULL) {
    // allocate memory for the stack of items inside the stack struct
    stack->items = (Node **)calloc(capacity, sizeof(Node *));
    // check that the list of Nodes stack allocated properly
    if (stack->items != NULL) {
      // initialzie the top and capacity variables for the stack object
      stack->top = 0;
      stack->capacity = capacity;
    } else {
      // if the memory didn't allocate properly, delete the stack
      free(stack->items);
      stack->items = NULL;
      free(stack);
      stack = NULL;
    }
  } else {
    // if the stack object didn't allocate memory properly, delete it
    free(stack);
    stack = NULL;
  }
  // return the stack
  return stack;
}

// helper function to return the stack size
uint32_t stack_size(Stack *s) { return s->top; }

// function to delete the stack itself
void stack_delete(Stack **s) {
  // make sure the stack passed in isn't NULL
  if (*s != NULL) {
    // make sure the items stack inside the object isn't NULL
    if ((*s)->items != NULL) {
      // delete the stack and items
      (*s)->top = 0;
      free((*s)->items);
      (*s)->items = NULL;
      free(*s);
      *s = NULL;
    }
    // just delete the stack
    free(*s);
    *s = NULL;
  }
}

// helper function to check if the stack is empty
bool stack_empty(Stack *s) {
  // make sure the stack object isn't NULL
  if (s != NULL) {
    // check if the top of the stack is 0, meaning empty
    if (s->top == 0) {
      // if so return true
      return true;
    } else {
      // if not, return false
      return false;
    }
  } else {
    return false;
  }
}

// helper function to check if the stack is full
bool stack_full(Stack *s) {
  // make sure the stack object isn't NULL
  if (s != NULL) {
    // check if the top of the stack is at the max capacity, and return based on
    // the check
    if (s->top == s->capacity) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// function to push a node onto the list object inside the stack
bool stack_push(Stack *s, Node *n) {
  // make sure that the stack object isn't NULL
  if ((s != NULL) && (n != NULL)) {
    // check if the stack is full before pushing a bit
    if (stack_full(s) == false) {
      // add the node passed in to the top of the list of Nodes
      s->items[s->top] = n;
      // increase the size of the stack
      s->top += 1;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// function to remove the last node inside the stack objeect
bool stack_pop(Stack *s, Node **n) {
  // make sure that the stack object isn't NULL
  if (s != NULL) {
    // make sure that the stack isn't empty because you can't take a bit if
    // there aren't any
    if (stack_empty(s) == false) {
      // set the node pointer passed in to the value of the last node in the
      // stack
      *n = s->items[s->top - 1];
      // set the node to 0
      s->items[s->top - 1] = 0;
      // decrease the size of the stack
      s->top -= 1;
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

// helper function to just print the stack for debugging
void stack_print(Stack *s) {
  if (s != NULL) {
    for (uint32_t i = 0; i < stack_size(s); i++) {
      if (s->items[i] != NULL) {
        node_print(s->items[i]);
      }
    }
  }
}

/*
int main(void){
        Stack* s;
        s = stack_create(10);
        Node* a = node_create('a', 20);
        Node* b = node_create('b', 5);
        Node* c = node_create('c', 12);
        Node* d = node_create('d', 50);
        printf("One push:\n");
        stack_push(s, b);
        stack_print(s);
        printf("\n\nTwo push:\n");
        stack_push(s, a);
        stack_push(s, d);
        stack_print(s);
        printf("\n\nOne pop:\n");
        stack_pop(s, &c);
        stack_print(s);
        printf("\n\nPopped node:\n");
        node_print(c);
        stack_delete(&s);
        stack_print(s);
}
*/
