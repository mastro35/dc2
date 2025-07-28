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

/* Returns the register name to be displayed for a stack position */
void get_register_name(int i, char* buffer) {
  sprintf(buffer, "%2d", i);

  if (i==1) strcpy(buffer," x");
  if (i==2) strcpy(buffer," y");
}

/* Display the command line help when called with the --help parameter */
void show_command_line_help(void) {
    printf("Usage: dc2 [OPTION]...\n");
    printf("Dave's (RPN) Calculator - a simple terminal-based RPN calculator\n\n");

    printf("Options:\n");
    printf("  -d, --deg          Set angle mode to degrees\n");
    printf("  -r, --rad          Set angle mode to radians (default)\n");
    printf("  -s, --sci          Use scientific notation for numbers (default)\n");
    printf("  -f, --fix          Use fixed-point notation for numbers\n");
    printf("  -V, --version      Show version information and exit\n");
    printf("  -h, --help         Display this help message and exit\n\n");

    printf("Examples:\n");
    printf("  dc2 --deg --fix     Start in degrees mode with fixed-point display\n");
    printf("  dc2 -s              Start with scientific display mode\n\n");

    printf("This is free software released under the GNU GPL v2.\n");
    printf("Made with love in Italy by Davide Mastromatteo\n");
}

/* Display the version of the program */
void show_version(void) {
  printf("dc2 %s Copyright (C) %s\n", APP_VERSION, COPYRIGHT);
}

/* Display a message showing the license of the program */
void show_license_message(void) {
  printf("\x1B[1;1H\x1B[2J");
  show_version();
  printf("dc2 comes with ABSOLUTELY NO WARRANTY. \n");  
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions.\n");
  printf("Check the license at https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
  printf("\n");
  printf("press ENTER to continue\n");
  getchar();
}

/* Shows the status of the calculator  */
void view_status(void) {
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

  printf("                 STACK\n");
  printf("┌────┬──────────────────────────┐\n");

  char buffer[3];

  int start = 0;
  if (sp > MAX_VIEWABLE_STACK) {
    start = sp - (MAX_VIEWABLE_STACK - 1);
    get_register_name((sp) , buffer);
    if (numeric_format == 'f') printf("│ %s │ %25.6lf│\n", buffer, stack[sp - 1]);
    if (numeric_format == 's') printf("│ %s │ %25.12lg│\n", buffer, stack[sp - 1]);
    printf("│....│..........................│\n");
  }

  for (int i=start; i<sp; i++) {
    get_register_name((sp) - i, buffer);
    if (numeric_format == 'f') printf("│ %s │ %25.6lf│\n", buffer, stack[i]);
    if (numeric_format == 's') printf("│ %s │ %25.12lg│\n", buffer, stack[i]);
  } 
  printf("└────┴──────────────────────────┘\n");
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
  show_version();
  printf("dc2 comes with ABSOLUTELY NO WARRANTY. \n");  
  printf("This is free software, and you are welcome to redistribute it\n");
  printf("under certain conditions.\n");
  printf("Check the license at https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
  printf("\n\n");
  printf("press ENTER to continue\n");
  getchar();
}

/* Shows the help */
void show_help(void) {
    printf("\x1B[1;1H\x1B[2J"); // Clear screen
    printf("HELP - Dave's (RPN) Calculator\n");
    printf("------------------------------\n");
    printf("Numbers & Ops:    <num>  +  -  *  /  ^\n");
    printf("Modes:            fix  sci   deg  rad\n");
    printf("Stack Ops:        d(drop)  s(swap)  c(clear)\n");
    printf("                  roll  unroll  [arrows]\n");
    printf("Consts & Rand:    pi   e   rnd/random\n");
    printf("Functions:        sqrt  log  ln  log10  !  rec(/)\n");
    printf("Trig:             sin cos tan  asin acos atan\n");
    printf("Other Cmds:       ENTER = repeat\n");
    printf("                  h/help  ?/credits  q/quit\n");
    printf("------------------------------\n");
    printf("Made with ❤ in Italy   (v%s)\n", APP_VERSION);
    printf("Press ENTER to return...");
    getchar();
}

void show_extended_help(void) {
    printf("\x1B[1;1H\x1B[2J"); // Clear screen
    printf("HELP - Dave's (RPN) Calculator\n");
    printf("---------------------------------\n");
    printf("Basics:\n");
    printf("  <number>                     Push number to stack\n");
    printf("  + - * /                      Arithmetic operations\n");
    printf("  ^                            Power (y^x)\n");
    printf("  fix / sci                    Set numeric display format\n");
    printf("  deg / rad                    Set angle mode\n");
    printf("\n");
    printf("Stack Operations:\n");
    printf("  d / drop                     Remove top of stack (x) \n");
    printf("  s / swap                     Swap top two values\n");
    printf("  c / clear                    Clear stack\n");
    printf("  roll / unroll / arrows keys  Rotate stack (right / left)\n");
    printf("\n");
    printf("Constants and Random:\n");
    printf("  pi                           Push π\n");
    printf("  e                            Push Euler's number\n");
    printf("  rnd / random                 Push random number [0,1)\n");
    printf("\n");
    printf("Press ENTER for more...");
    getchar();

    printf("\x1B[1;1H\x1B[2J"); // Clear screen
    printf("HELP (page 2) - Dave's (RPN) Calculator\n");
    printf("---------------------------------------\n");
    printf("Functions:\n");
    printf("  sqrt                         Square root\n");
    printf("  log / ln                     Natural logarithm\n");
    printf("  log10                        Log base 10\n");
    printf("  !                            Factorial (n!)\n");
    printf("  rec or \\                     Reciprocal (1/x)\n");
    printf("\n");
    printf("Trigonometry:\n");
    printf("  sin  cos  tan                Basic trig functions\n");
    printf("  asin acos atan               Inverse trig functions\n");
    printf("\n");
    printf("Other Commands:\n");
    printf("  ENTER                        Repeat last input\n");
    printf("  h / help                     Show this help\n");
    printf("  ? / credits                  About the author\n");
    printf("  q / quit                     Exit calculator\n");
    printf("\n");
    printf("Press ENTER to return...");
    getchar();
}
