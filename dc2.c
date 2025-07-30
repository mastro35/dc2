// SPDX-License-Identifier: GPL-2.0-or-later
/* dc2.c -- Dave's (RPN) Calculator
 *
 * A simple RPN calculator for your terminal
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
#define APP_VERSION_PATCH 5

// Helper macros to stringify values
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define APP_VERSION STR(APP_VERSION_MAJOR) "." STR(APP_VERSION_MINOR) "." STR(APP_VERSION_PATCH)

#define COPYRIGHT "2025 Davide Mastromatteo"

#define STACK_LENGTH 99
#define INPUT_BUFFER 100
#define MAX_VIEWABLE_STACK 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>

#include <termios.h>
#include <unistd.h>

double stack[STACK_LENGTH];
int sp = 0;

// dc2 starts in rad mode with scientific notation
char mode = 'r';
char numeric_format = 's';

#include "dc2_stack.c"
#include "dc2_math.c"
#include "dc2_ui.c"

operation_0o get_operation_0o(char *operation); 
operation_1o get_operation_1o(char *operation); 
operation_1o get_trigonometric_operation_1o(char *operation); 
operation_2o get_operation_2o(char *operation); 


/* ------------
   MAIN PROGRAM
   ------------ */

/* Set the input inserted by the user in memory */
void set_input_if_numeric(char* input, double* value, int* is_numeric ) {
  char* endptr;
  *value = strtod(input, &endptr);
  if (endptr[0] == '\0') *is_numeric = 1; 
}

/* Get a random number between 0 and 1 */
double get_random_number(void) {
    return drand48();
}

/* Set Mode:
   d = DEG mode
   r = RAD mode */
void set_mode(char input_mode) {
  if (input_mode == 'r' || input_mode == 'd') mode = input_mode;
}

/* Set Numeric Format:
   f = Fixed Decimal format
   s = Scientific format */
void set_numeric_format(char input_format) {
  if (input_format == 's' || input_format == 'f') numeric_format = input_format;
}

void set_rad_mode(void) {
  set_mode('r');
}

void set_deg_mode(void) {
  set_mode('d');
}

void set_sci_numeric_format(void) {
  set_numeric_format('s');
}

void set_fix_numeric_format(void) {
  set_numeric_format('f');
}

/* Compute the command received */
int compute(char* command, char* last_command) {
//  int prevent_last_command_mem = 0;
  double value = 0;
  int is_numeric = 0;
  operation_2o operation_2o = NULL;
  operation_1o operation_1o = NULL;
  operation_0o operation_0o = NULL;

  if ((strcmp(command, "quit") == 0) ||
      (strcmp(command, "q") == 0)) return 1;

//  if ((strcmp(command, "redo") == 0) || 
//      (strcmp(command, "r") == 0)) {
//    prevent_last_command_mem = 1;
//    strcpy(command, last_command);
// }

  if (strcmp(command, "") == 0) {
    if (sp == 0) return 0;
    push(pick(sp));
    return 0;
  }

  if (strcmp(command, "e") == 0) {
    push(M_E);
  }

  if (strcmp(command, "pi") == 0) {
    push(M_PI);
  }

  if ((strcmp(command, "random") == 0) ||
     (strcmp(command, "rnd") == 0)) {
    push(get_random_number());
  }

  set_input_if_numeric(command, &value, &is_numeric);

  if (is_numeric) {
    push(value);
  } else {
    //if (!prevent_last_command_mem) 
    strcpy(last_command, command);
  }
        
  if ((operation_2o = get_operation_2o(command))) {
    compute_operation_2o(operation_2o);
  }

  if ((operation_1o = get_operation_1o(command))) {
    compute_operation_1o(operation_1o);
  }

  if ((operation_1o = get_trigonometric_operation_1o(command))) {
    compute_trigonometric_operation_1o(operation_1o);
  }

  if ((operation_0o = get_operation_0o(command))) {
    compute_operation_0o(operation_0o);
  }

    
return 0;
}

/* Enable terminal raw mode to get arrows from keyboard 
   needed by the power_fgets */
void enable_raw_mode(struct termios *old_termios) {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, old_termios);
    new_termios = *old_termios;
    new_termios.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
}

/* Disable terminal raw mode 
   needed by the power_fgets */
void disable_raw_mode(struct termios *old_termios) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_termios);
}

/* Get an input from the keyboard 
   taking care of escape characters */
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
              buffer[i] = '\0';
            }
            continue;
          } 
          
          buffer[i++] = c;
          putchar(c);
        }
        
        buffer[i] = '\0';
    }
    
    disable_raw_mode(&old_termios);
}

void get_input(char* input) {
    printf("> ");
    power_fgets(input, INPUT_BUFFER - 1);

    // to lower case
    while (*input) {
        *input = tolower((unsigned char)*input);
        input++;
    }

    input[strcspn(input, "\n")] = '\0';
}

void handle_command_line_input(int argc, char* argv[]) {
  // process the input parameters
  static struct option long_options[] = {
    {"deg", no_argument, 0, 'd'},
    {"rad", no_argument, 0, 'r'},
    {"sci", no_argument, 0, 's'},
    {"fix", no_argument, 0, 'f'},
    {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'V'},
    {0, 0, 0, 0}
  };

  int opt = 0;
  int option_index = 0;

  while ((opt = getopt_long(argc, argv, "drsfV", long_options, &option_index))!=-1) {
    switch(opt) {
      case 'd': set_mode('d'); break;
      case 'r': set_mode('r'); break;
      case 's': set_numeric_format('s'); break;
      case 'f': set_numeric_format('f'); break;
      case 'h': show_command_line_help(); exit(0);
      case 'V': show_version(); exit(0);
      case '?': exit(1);
    }
  }
}

operation_0o get_operation_0o(char* operation) {
  if ((strcmp(operation, "credits") == 0) || 
      (strcmp(operation, "?") == 0)) {
    return show_credits;
  }

  if (strcmp(operation, "rad") == 0) {
    return set_rad_mode;
  }

  if (strcmp(operation, "deg") == 0) {
    return set_deg_mode;
  }

  if (strcmp(operation, "fix") == 0) {
    return set_fix_numeric_format;
  }

  if (strcmp(operation, "sci") == 0) {
    return set_sci_numeric_format;
  }
  
  if (strcmp(operation, "license") == 0) {
      return show_license_message;
  }

  if ((strcmp(operation, "help") == 0) ||
      (strcmp(operation, "h") == 0)) {
    return show_help;
  }

  if ((strcmp(operation, "clear") == 0) ||
     (strcmp(operation, "c") == 0)) {
    return clear;
  }

  if ((strcmp(operation, "drop") == 0) ||
     (strcmp(operation, "d") == 0)) {
    return drop;
  }

  if ((strcmp(operation, "swap") == 0) ||
     (strcmp(operation, "s") == 0)) {
    return swap;
  }

  if ((strcmp(operation, "roll") == 0) ||
      (strcmp(operation, "rroll") == 0) || 
     (strcmp(operation, "ARROW_RIGHT") == 0)) {
    return rroll;
  }

  if ((strcmp(operation, "unroll") == 0) ||
      (strcmp(operation, "lroll") == 0) || 
     (strcmp(operation, "ARROW_LEFT") == 0)) {
    return lroll;
  }

  return NULL;
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


/* Entry point */
int main(int argc, char* argv[]) {
  char input[(INPUT_BUFFER-1)] = "";
  char last_input[(INPUT_BUFFER-1)] = "";

  /* randomize the seed 
     of the random number generator*/
  srand48(time(NULL));

  handle_command_line_input(argc, argv);

  // REPL
  while (1) {
    view_status(); 
    get_input(input);        
    if (compute(input, last_input)) break;
  }

  return 0;
}
