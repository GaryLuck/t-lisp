# Tiny LISP - Quick Start Guide

## Compilation

```bash
gcc -o tinylisp tinylisp.c -Wall
```

## Running

### Interactive Mode
```bash
./tinylisp
```

### Run a File
```bash
./tinylisp < examples_simple.lisp
```

or

```bash
cat examples_simple.lisp | ./tinylisp
```

## Basic Examples

### Simple Arithmetic
```lisp
> (+ 1 2 3)
6
> (* 5 6)
30
> (/ 20 4)
5
```

### Lists
```lisp
> (cons 1 '(2 3))
(1 2 3)
> (car '(1 2 3))
1
> (cdr '(1 2 3))
(2 3)
```

### Define a Function
```lisp
> (defun square (x) 
    (* x x))
square
> (square 9)
81
```

### Recursive Functions
```lisp
> (defun factorial (n) 
    (if (eq n 0) 
        1 
        (* n (factorial (- n 1)))))
factorial
> (factorial 5)
120
```

### Lambda (Anonymous Functions)
```lisp
> ((lambda (x y) (+ x y)) 10 20)
30
```

### Higher-Order Functions
```lisp
> (defun map (f lst)
    (if (eq lst nil)
        nil
        (cons (f (car lst)) (map f (cdr lst)))))
map
> (defun square (x) (* x x))
square  
> (map square '(1 2 3 4))
(1 4 9 16)
```

## Language Reference

### Special Forms
- `(quote expr)` or `'expr` - Return expression without evaluation
- `(if cond then else)` - Conditional
- `(lambda (params) body)` - Anonymous function
- `(defun name (params) body)` - Define named function

### Built-in Functions
- `(car list)` - First element
- `(cdr list)` - Rest of list
- `(cons elem list)` - Add element to front
- `(+ ...)` - Addition
- `(- ...)` - Subtraction
- `(* ...)` - Multiplication
- `(/ ...)` - Division
- `(eq a b)` - Equality test
- `(< a b)` - Less than

### Special Values
- `nil` - Empty list / false
- `t` - True

## Tips

1. **Multi-line input**: The REPL automatically continues reading if parentheses aren't balanced
2. **Comments**: Start a line with `;` for comments
3. **Exit**: Press Ctrl+D to exit the REPL
4. **Recursion**: All functions can be recursive - this makes the language Turing complete!

## Example Programs

See `examples_simple.lisp` for working examples including:
- Factorial (recursive)
- Fibonacci (multiple recursion)
- List operations (length, sum, map)
- Higher-order functions

Try running: `./tinylisp < examples_simple.lisp`
