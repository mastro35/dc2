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

/* ---------------
   STACK FUNCTIONS
   --------------- */

/* Pick a value from the stack without popping it */
double pick(int sp) {
  if (sp == 0) {
    return 0;
  }
  return stack[sp - 1];  
}

/* Pop a value from the stack returning it to the caller */
double pop(void) {
  if (sp == 0) {
    printf("No value left in the stack\n");
    return 0;
  }

  double result = pick(sp);
  sp--;
  return result;
}

/* Drop a value from the stack */
void drop(void) {
  pop();
}

/* Push a value to the stack */
void push(double val) {
    if (sp >= STACK_LENGTH) {
      printf("Out Of Memory, prevented a stack overflow\n");
      return;
    }

    stack[sp] = val;
    sp++;
}

/* Clear the stack */
void clear(void) {
  sp = 0;
  stack[sp] = 0;
}

/* Swap the x and y register */
void swap(void) {
  if (sp<2) return;
  double x = pop();
  double y = pop();
  push(x);
  push(y);
}

/* roll the entire stack to the left: the third item become the second, 
   the second become the first... and so on until the first
   become the last */
void lroll(void) {
  if (sp == 0) return;
  double first_value = pick(1); 

  for (int i=0; i<((sp) - 1); i++) stack[i] = stack[i+1];
  stack[sp - 1] = first_value;
}

/* roll the entire stack to the right: the first item become the second, 
   the second become the third... and so on until the last 
   become the first */
void rroll(void) {
  if (sp == 0) return;
  double last_value = pick(sp);

  for (int i=(sp - 1); i>0; i--) stack[i] = stack[i-1];
  stack[0] = last_value;
}
