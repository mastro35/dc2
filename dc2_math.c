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

/* --------------
   Math Functions
   -------------- */

/* Log */
void log_operation_2o(double y, double x, char *name, double r) {
  sprintf(operation_log[n_operation_log], "%lg %s %lg = %lg", y, name, x, r);
  n_operation_log ++;
}

void log_operation_1o(double x, char *name, double r) {
  sprintf(operation_log[n_operation_log], "%lg %s = %lg", x, name, r);
  n_operation_log ++;
}

/* Generic function pointers for the single operand operations
   and the two-operands operations */
typedef void (*operation_0o)(void);
typedef double (*operation_1o)(double);
typedef double (*operation_2o)(double, double);

/* Compute a single operand operation */
void compute_operation_0o(operation_0o f) {
  f();
}

/* Compute a single operand operation */
void compute_operation_1o(operation_1o f, char *name) {
  if (sp < 1) return;
  double x = pop();
  double r = f(x);
  push(r);
  log_operation_1o(x, name, r);
}

void compute_trigonometric_operation_1o(operation_1o f, char *name) {
  if (sp < 1) return;
  double x = pop();
  if (mode == 'd') x = x * M_PI / 180;
  double r = f(x);
  push(r);
  log_operation_1o(x, name, r);
}

/* Compute a two-operands operation */
void compute_operation_2o(operation_2o f, char *name) {
  if (sp < 2) return;
  double y = pop();
  double x = pop();
  double r = f(y, x);
  push(r);
  log_operation_2o(y, x, name, r);
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


