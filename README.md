# dc2 – Dave's RPN Calculator

A simple, fast, and minimal Reverse Polish Notation (RPN) calculator for the terminal.  
Made with love in Italy 🇮🇹 by Davide Mastromatteo (mastro35@gmail.com)

## ✨ Features

- Reverse Polish Notation (RPN) input
- Basic arithmetic: +, -, *, /
- Advanced math: power, factorial, square root, reciprocal
- Trigonometric functions: sin, cos, tan
- Constants: pi, e
- Random number generation
- Stack manipulation: drop, swap, clear, roll
- Command repetition with redo
- Help and credits screen
- Clean, minimal terminal interface

## 🧑‍💻 Usage

Run the program in your terminal. Each line accepts either:
- A number to push onto the stack
- An operator or a command

Pressing Enter with no input duplicates the top of the stack.

## 📚 Commands Reference

### Arithmetic
+, -, *, / – Basic operations  
^, power, pow – Raise y to the power of x

### Trigonometric
sin, cos, tan

### Advanced Math
sqrt – Square root  
! – Factorial  
rec, reciprocal – Reciprocal (1/x)

### Constants
pi – Push π (3.14159…)  
e – Push Euler’s number (2.71828…)

### Random
rnd, random – Push a number in the range [0.0, 1.0)

### Stack Manipulation
drop, d – Remove top of stack  
swap, s – Swap top two elements  
clear, c – Clear the stack  
roll, cycle – Rotate stack (last becomes first)

### Other Commands
redo, r – Repeat last command  
help, h – Show help screen  
credits, ? – Show credits  
quit, q – Exit the program

## 🛠️ Building

Requires a C compiler such as gcc or clang. Compile the source file dc2.c and link with the math library.

## 🧾 License

This project is licensed under the GNU GPL v2.0 or later.

## ☕ Support

If you enjoy this project, consider donating:  
https://buymeacoffee.com/mastro35

## 🙏 Credits

Written by Davide "iceman" Mastromatteo  
Inspired by classic RPN tools like dc, HP calculators, and command-line minimalism.
