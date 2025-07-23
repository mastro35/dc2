// SPDX-License-Identifier: GPL-2.0-or-later
/* dc2.c -- Dave's (RPN) Calculator
 * A simple RPN calculator for terminal
 * made with love in Italy.
 *
 * Copyright 2025 Davide Mastromatteo
 * All Rights Reserved.
 *
 * Written by Davide "iceman" Mastromatteo <mastro35@gmail.com>
 */

#define STACK_LENGTH 100
#define INPUT_BUFFER 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* ---------------
   STACK FUNCTIONS
   --------------- */

/* Pick a value from the stack without popping it */
double pick(double* ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) {
    return 0;
  }
  return *(ptr_stack + (*ptr_sp - 1));  
}

/* Pop a value from the stack returning it to the caller */
double pop(double* ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) {
    printf("No value left in the stack\n");
    return 0;
  }

  double result = pick(ptr_stack, ptr_sp);
  (*ptr_sp)--;
  return result;
}

/* Push a value to the stack */
void push(double* ptr_stack, int *ptr_sp, double val) {
    if (*ptr_sp >= STACK_LENGTH) {
      printf("Out Of Memory, prevented a stack overflow\n");
      return;
    }

    *(ptr_stack + *ptr_sp) = val;
    (*ptr_sp)++;
}

/* Clear the stack */
void clear(double *ptr_stack, int *ptr_sp) {
  *ptr_sp = 0;
  ptr_stack[*ptr_sp] = 0;
}

/* Swap the x and y register */
void swap(double *ptr_stack, int *ptr_sp) {
  if (*ptr_sp<2) return;
  double x = pop(ptr_stack, ptr_sp);
  double y = pop(ptr_stack, ptr_sp);
  push(ptr_stack, ptr_sp, x);
  push(ptr_stack, ptr_sp, y);
}

/* roll the stack */
void roll(double *ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) return;
  double last_value = pick(ptr_stack, ptr_sp);

  for (int i=((*ptr_sp) - 1); i>0; i--) ptr_stack[i] = ptr_stack[i-1];
  ptr_stack[0] = last_value;
}

/* --------------
   Math Functions
   -------------- */

/* Generic function pointers for the single operand operations
   and the two-operands operations */
typedef double (*operation_1o)(double);
typedef double (*operation_2o)(double, double);

/* Compute a single operand operation */
void compute_operation_1o(double *ptr_stack, int *ptr_sp, operation_1o f) {
  if (*ptr_sp < 1) return;
  double x = pop(ptr_stack, ptr_sp);
  double r = f(x);
  push(ptr_stack, ptr_sp, r);
}

/* Compute a two-operands operation */
void compute_operation_2o(double *ptr_stack, int *ptr_sp, operation_2o f) {
  if (*ptr_sp < 2) return;
  double y = pop(ptr_stack, ptr_sp);
  double x = pop(ptr_stack, ptr_sp);
  double r = f(y, x);
  push(ptr_stack, ptr_sp, r);
}

/* Compute the power x of y */
double to_power(double x, double y) {
  return pow(y, x);
}

/* Compute a sum between two numbers */
double sum(double x, double y) {
  return y + x;
}

/* Compute a subtraction between two numbers*/
double subtraction(double x, double y) {
  return y - x;
}

/* Compute a multiplication between two numbers */
double multiplication(double x, double y) {
  return x * y;
}

/* Compute a division between two numbers */
double division(double x, double y) {
  return y / x;
}

/* Compute the factorial of a number*/
double factorial(double x) {
  return tgamma(x+1);
}

/* Compute the reciprocal of a number*/
double reciprocal(double x) {
  return (1/x);
}


/* Get the single operand operation corresponding
   to the command received as input */
operation_1o get_operation_1o(char *operation) {
  if (strcmp(operation, "!") == 0) {
    return factorial;}

  if (strcmp(operation, "sqrt") == 0) {
    return sqrt;}

  if (strcmp(operation, "sin") == 0) {
    return sin;}

  if (strcmp(operation, "cos") == 0) {
    return cos;}

  if (strcmp(operation, "tan") == 0) {
    return tan;}

  if ((strcmp(operation, "reciprocal") == 0) ||
      (strcmp(operation, "rec") == 0)) {
    return reciprocal;}

  return NULL;

}

/* Get the two-operands operation corresponding
   to the command received as input */
operation_2o get_operation_2o(char *operation) {
  if (strcmp(operation, "+") == 0) {
    return sum;}

  if (strcmp(operation, "-") == 0) {
    return subtraction;}

  if (strcmp(operation, "*") == 0) {
    return multiplication;}

  if (strcmp(operation, "/") == 0) {
    return division;}

  if ((strcmp(operation, "power") == 0) ||
      (strcmp(operation, "pow") == 0) ||
      (strcmp(operation, "^") == 0)) {
    return to_power;}

  return NULL;
}

/* ------------
   MAIN PROGRAM
   ------------ */

/* Returns the register name to be displayed for a stack position */
void get_register_name(int i, char* buffer) {
  sprintf(buffer, "%2d", i);

  if (i==1) strcpy(buffer," x");
  if (i==2) strcpy(buffer," y");
}

/* Shows the status of the calculator  */
void view_status(double *ptr_stack, int *ptr_sp) {
  printf("\x1B[1;1H\x1B[2J");

  printf("___BEGIN OF STACK___\n");

  char buffer[3];

  for (int i=0; i<*ptr_sp; i++) {
    get_register_name((*ptr_sp) - i, buffer);
    printf("%s -> %12.6lf\n", buffer, ptr_stack[i]);
  } 
  printf("____END OF STACK____\n");
}

/* Set the input inserted by the user in memory */
void set_input_if_numeric(char* input, double* value, int* is_numeric ) {
  char* endptr;
  *value = strtod(input, &endptr);
  if (endptr[0] == '\0') *is_numeric = 1; 
}

/* Shows the credits */
void show_credits(void) {
  printf("\x1B[1;1H\x1B[2J");
  printf("__________________________________\n");
  printf("Dave's (RPN) Calc\n");
  printf("made with love in Italy\n");
  printf("by Davide Mastromatteo\n");
  printf("<mastro35@gmail.com>\n\n");
  printf("If you enjoy this program\n");
  printf("consider donating at \n");
  printf("https://buymeacoffee.com/mastro35\n\n");
  printf("press ENTER to continue\n");
  printf("__________________________________\n");
  getchar();
}

/* Shows the help */
void show_help(void) {
  printf("\x1B[1;1H\x1B[2J"); // Clear screen
  printf("HELP - Dave's (RPN) Calculator\n");
  printf("---------------------------------\n");
  printf("This calculator uses Reverse Polish Notation (RPN).\n");
  printf("Enter numbers first, then operations.\n\n");

  printf("Basic Usage:\n");
  printf("  <number>             Push number to stack\n");
  printf("  ENTER (empty input)  Duplicate top of stack (x -> x x)\n\n");

  printf("Arithmetic Operators:\n");
  printf("  + - * /            Basic Operations\n");
  printf("  ^                  Power (y^x)\n");
  printf("  (pow)er            Power (alias)\n");

  printf("Trigonometric Functions:\n");
  printf("  sin                Sine\n");
  printf("  cos                Cosine\n");
  printf("  tan                Tangent\n");

  printf("Other Functions:\n");
  printf("  !                  Factorial\n");
  printf("  sqrt               Square root\n");
  printf("  (rec)iprocal       Reciprocal (1/x)\n");

  printf("Constants:\n");
  printf("  pi                 Push π (3.14159...)\n");
  printf("  e                  Push e (2.71828...)\n");
  printf("  rnd                Push random number [0.0, 1.0)\n");
  printf("  random             Push random number (alias)\n");

  printf("Stack Commands:\n");
  printf("  (d)rop             Drop top of stack\n");
  printf("  (s)wap             Swap top two values\n");
  printf("  (c)lear            Clear stack\n");
  printf("  roll | cycle       Roll the stack\n");

  printf("Other Commands:\n");
  printf("  (r)redo            Repeat last operation\n");
  printf("  (h)elp             Show the help screen\n");
  printf("  (?)credits         Show credits\n");
  printf("  (q)uit             Quit program \n");

  printf("Press ENTER to continue...\n");
  getchar();
}

double get_random_number(void) {
    return drand48();
}

/* Compute the command received */
int compute(double *ptr_stack, int *ptr_sp, char* command, char* last_command) {
  int prevent_last_command_mem = 0;
  double value = 0;
  int is_numeric = 0;
  operation_2o operation_2o = NULL;
  operation_1o operation_1o = NULL;

  if ((strcmp(command, "quit") == 0) ||
      (strcmp(command, "q") == 0)) return 1;

  if ((strcmp(command, "redo") == 0) || 
      (strcmp(command, "r") == 0)) {
    prevent_last_command_mem = 1;
    strcpy(command, last_command);
  }

  if ((strcmp(command, "credits") == 0) || 
      (strcmp(command, "?") == 0)) {
    show_credits();
  }

  if ((strcmp(command, "help") == 0) ||
      (strcmp(command, "h") == 0)) {
    show_help();
  }

  if ((strcmp(command, "clear") == 0) ||
     (strcmp(command, "c") == 0)) {
    clear(ptr_stack, ptr_sp);
  }

  if ((strcmp(command, "drop") == 0) ||
     (strcmp(command, "d") == 0)) {
    pop(ptr_stack, ptr_sp);
  }

  if ((strcmp(command, "swap") == 0) ||
     (strcmp(command, "s") == 0)) {
    swap(ptr_stack, ptr_sp);
  }

  if ((strcmp(command, "roll") == 0) ||
     (strcmp(command, "cycle") == 0)) {
    roll(ptr_stack, ptr_sp);
  }

  if (strcmp(command, "") == 0) {
    if (*ptr_sp == 0) return 0;
    push(ptr_stack, ptr_sp, pick(ptr_stack, ptr_sp));
    return 0;
  }

  if (strcmp(command, "e") == 0) {
    push(ptr_stack, ptr_sp, M_E);
  }

  if (strcmp(command, "pi") == 0) {
    push(ptr_stack, ptr_sp, M_PI);
  }

  if ((strcmp(command, "random") == 0) ||
     (strcmp(command, "rnd") == 0)) {
    push(ptr_stack, ptr_sp, get_random_number());
  }

  set_input_if_numeric(command, &value, &is_numeric);

  if (is_numeric) {
    push(ptr_stack, ptr_sp, value);
  } else {
    if (!prevent_last_command_mem) strcpy(last_command, command);
  }
        
  if ((operation_2o = get_operation_2o(command))) {
    compute_operation_2o(ptr_stack, ptr_sp, operation_2o);
  }

  if ((operation_1o = get_operation_1o(command))) {
    compute_operation_1o(ptr_stack, ptr_sp, operation_1o);
  }
    
return 0;
}

/* Get an input from the user */
void get_input(char* input) {
    printf("> ");
    fgets(input, (INPUT_BUFFER-1), stdin);
    input[strcspn(input, "\n")] = '\0';
}

/* Entry point */
int main(void) {
  double stack[STACK_LENGTH];
  int sp = 0;

  srand48(time(NULL));

  char input[(INPUT_BUFFER-1)] = "";
  char last_input[(INPUT_BUFFER-1)] = "";

  while (1) {
    view_status(stack, &sp);
    get_input(input);
    
    if (compute(stack, &sp, input, last_input)) break;
  }

  return 0;
}
