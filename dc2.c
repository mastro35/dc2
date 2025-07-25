// SPDX-License-Identifier: GPL-2.0-or-later
/* dc2.c -- Dave's (RPN) Calculator
 *
 * A simple RPN calculator for terminal
 * made with love in Italy.
 *
 * Copyright 2025 Davide Mastromatteo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 2
#define APP_VERSION_PATCH 0

#define APP_VERSION "0.2.0"
#define COPYRIGHT "2025 Davide Mastromatteo"

#define STACK_LENGTH 100
#define INPUT_BUFFER 100
#define MAX_VIEWABLE_STACK 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <termios.h>
#include <unistd.h>

// dc2 starts in rad mode
char mode = 'r';

// and with numbers in sci mode
char numeric_format = 's';

/* ---------------
   STACK FUNCTIONS
   --------------- */

/* Pick a value from the stack without popping it */
double pick(double* ptr_stack, int sp) {
  if (sp == 0) {
    return 0;
  }
  return *(ptr_stack + (sp - 1));  
}

/* Pop a value from the stack returning it to the caller */
double pop(double* ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) {
    printf("No value left in the stack\n");
    return 0;
  }

  double result = pick(ptr_stack, *ptr_sp);
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

/* roll the entire stack to the left: the third item become the second, 
   the second become the first... and so on until the first
   become the last */
void lroll(double *ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) return;
  double first_value = pick(ptr_stack, 1); // *ptr_stack;

  for (int i=0; i<((*ptr_sp) - 1); i++) ptr_stack[i] = ptr_stack[i+1];
  ptr_stack[(*ptr_sp) - 1] = first_value;
}

/* roll the entire stack to the right: the first item become the second, 
   the second become the third... and so on until the last 
   become the first */
void rroll(double *ptr_stack, int *ptr_sp) {
  if (*ptr_sp == 0) return;
  double last_value = pick(ptr_stack, *ptr_sp);

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

void compute_trigonometric_operation_1o(double *ptr_stack, int *ptr_sp, operation_1o f) {
  if (*ptr_sp < 1) return;
  double x = pop(ptr_stack, ptr_sp);
  if (mode == 'd') x = x * M_PI / 180;
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

  if (strcmp(operation, "log10") == 0) {
    return log10;}

  if ((strcmp(operation, "log") == 0) || 
      (strcmp(operation, "ln") == 0)) {
    return log;}

  if ((strcmp(operation, "reciprocal") == 0) ||
      (strcmp(operation, "\\") == 0) ||
      (strcmp(operation, "rec") == 0)) {
    return reciprocal;}

  return NULL;

}

/* Get the single operand operation corresponding
   to the command received as input */
operation_1o get_trigonometric_operation_1o(char *operation) {
  if (strcmp(operation, "sin") == 0) {
    return sin;}

  if (strcmp(operation, "cos") == 0) {
    return cos;}

  if (strcmp(operation, "tan") == 0) {
    return tan;}

  if (strcmp(operation, "asin") == 0) {
    return asin;}

  if (strcmp(operation, "acos") == 0) {
    return acos;}

  if (strcmp(operation, "atan") == 0) {
    return atan;}

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

void show_license_message(int stop) {
  if (stop) printf("\x1B[1;1H\x1B[2J");
  printf("dc2 %s Copyright (C) %s\n", APP_VERSION, COPYRIGHT);
  printf("dc2 comes with ABSOLUTELY NO WARRANTY. \n");  
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions.\n");
  printf("Check the license at https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
  printf("\n");
  if (stop) {
    printf("press ENTER to continue\n");
    getchar();
  }
}

/* Shows the status of the calculator  */
void view_status(double *ptr_stack, int *ptr_sp) {
  printf("\x1B[1;1H\x1B[2J");

  char mode_string[] = "err";
  if (mode == 'd') strcpy(mode_string, "deg");
  if (mode == 'r') strcpy(mode_string, "rad");

  char numeric_format_string[] = "err";
  if (numeric_format == 'f') strcpy(numeric_format_string, "fix");
  if (numeric_format == 's') strcpy(numeric_format_string, "sci");

  printf("┌─────┬─────┐ \n");	
  printf("│ %s │ %s │ \n", mode_string, numeric_format_string);
  printf("└─────┴─────┘ \n");	

  printf("┌────┬────────────────┐\n");

  char buffer[3];

  int start = 0;
  if (*ptr_sp > MAX_VIEWABLE_STACK) {
    start = *ptr_sp - (MAX_VIEWABLE_STACK - 1);
    get_register_name((*ptr_sp) , buffer);
    if (numeric_format == 'f') printf("│ %s │ %15.6lf│\n", buffer, ptr_stack[*ptr_sp-1]);
    if (numeric_format == 's') printf("│ %s │ %15.6lg│\n", buffer, ptr_stack[*ptr_sp-1]);
    printf("│....│................│\n");
  }

  for (int i=start; i<*ptr_sp; i++) {
    get_register_name((*ptr_sp) - i, buffer);
    if (numeric_format == 'f') printf("│ %s │ %15.6lf│\n", buffer, ptr_stack[i]);
    if (numeric_format == 's') printf("│ %s │ %15.6lg│\n", buffer, ptr_stack[i]);
  } 
  printf("└────┴────────────────┘\n");
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
  printf("\n");
  printf("Dave's (RPN) Calculator\n");
  printf("ˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆ\n");
  printf("made with love in Italy\n");
  printf("<mastro35@gmail.com> || https://hachyderm.io/@mastro35\n");
  printf("If you enjoy this program buy me a coffee at\n");
  printf("https://buymeacoffee.com/mastro35\n\n");
  printf("LICENSE INFORMATION\n");
  printf("ˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆˆ\n");
  show_license_message(0);  
  printf("\n\n");
  printf("press ENTER to continue\n");
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

  if (strcmp(command, "rad") == 0) {
      mode = 'r';
  }

  if (strcmp(command, "deg") == 0) {
      mode = 'd';
  }

  if (strcmp(command, "fix") == 0) {
      numeric_format = 'f';
  }

  if (strcmp(command, "sci") == 0) {
      numeric_format = 's';
  }
  
  if (strcmp(command, "license") == 0) {
      show_license_message(1);
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
      (strcmp(command, "rroll") == 0) || 
     (strcmp(command, "ARROW_RIGHT") == 0)) {
    rroll(ptr_stack, ptr_sp);
  }

  if ((strcmp(command, "unroll") == 0) ||
      (strcmp(command, "lroll") == 0) || 
     (strcmp(command, "ARROW_LEFT") == 0)) {
    lroll(ptr_stack, ptr_sp);
  }


  if (strcmp(command, "") == 0) {
    if (*ptr_sp == 0) return 0;
    push(ptr_stack, ptr_sp, pick(ptr_stack, *ptr_sp));
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

  if ((operation_1o = get_trigonometric_operation_1o(command))) {
    compute_trigonometric_operation_1o(ptr_stack, ptr_sp, operation_1o);
  }
    
return 0;
}


void enable_raw_mode(struct termios *old_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

void disable_raw_mode(struct termios *old_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_termios);
}

/* Get an input from the keyboard */
void power_fgets(char *buffer, int max_len) {
    struct termios old_termios;
    enable_raw_mode(&old_termios);

    int i = 0;
    while (i < max_len - 1) {
        char c = getchar();

        if (c == 27) { // if an escape char has been pressed...
          char seq1 = getchar();
          if (seq1 == '[') {
            char seq2 = getchar();
            switch (seq2) {
            case 'A': strcpy(buffer, "ARROW_UP\0"); break; // printf("\n[FRECCIA SU]\n"); break;
            case 'B': strcpy(buffer, "ARROW_DOWN\0"); break; // printf("\n[FRECCIA GIÙ]\n"); break;
            case 'C': strcpy(buffer, "ARROW_RIGHT\0"); break; // printf("\n[FRECCIA DESTRA]\n"); break;
            case 'D': strcpy(buffer, "ARROW_LEFT\0"); break; // printf("\n[FRECCIA SINISTRA]\n"); break;
            default: break;
            }
          }            
          break;
        } else {
          if (c == '\n') { 
            putchar('\n');
            break;
          }
          
          if (c == 127 || c == 8) { // this is the backspace
            if (i > 0) {
              i--;
              printf("\b \b");
            }
          } 
          
          buffer[i++] = c;
          putchar(c);
        }
        
        buffer[i] = '\0';
    }

    disable_raw_mode(&old_termios);
}

/* Get an input from the user */
void get_input(char* input) {
    printf("> ");
    power_fgets(input, INPUT_BUFFER - 1);
//    fgets(input, (INPUT_BUFFER-1), stdin);
    input[strcspn(input, "\n")] = '\0';
}

/* Entry point */
int main(void) {
  double stack[STACK_LENGTH];
  int sp = 0;

  // randomize the seed
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
