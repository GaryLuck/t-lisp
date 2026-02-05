# Tiny LISP Interpreter

A complete, Turing-complete LISP interpreter implemented in C with approximately 500 lines of code.

## Features

### Core Functionality
- **REPL (Read-Eval-Print Loop)**: Interactive command-line interface with multi-line support
- **S-Expression Parsing**: Full support for LISP S-expressions
- **Comment Support**: Lines starting with `;` are treated as comments
- **Turing Complete**: Supports recursion, conditionals, and function definitions

### Implemented Operations

#### List Operations
- `CAR` - Returns the first element of a list
- `CDR` - Returns the rest of the list (all elements except the first)
- `CONS` - Constructs a new list by prepending an element
- `QUOTE` - Returns an expression without evaluating it (also supports `'` syntax)

#### Arithmetic Operations
- `+` - Addition (supports multiple arguments)
- `-` - Subtraction (supports multiple arguments)
- `*` - Multiplication (supports multiple arguments)
- `/` - Division (supports multiple arguments)

#### Control Flow
- `IF` - Conditional expression: `(if condition then-expr else-expr)`

#### Function Definition
- `LAMBDA` - Anonymous function: `(lambda (params) body)`
- `DEFUN` - Named function definition: `(defun name (params) body)`

#### Comparison
- `EQ` - Equality test
- `<` - Less than comparison

## Building

```bash
gcc -o tinylisp tinylisp.c -Wall
```

## Usage

### Interactive REPL

```bash
./tinylisp
```

The REPL supports multi-line expressions. Just keep typing and the interpreter will wait until parentheses are balanced:

```lisp
> (+ 1 2 3)
6
> (defun square (x) 
    (* x x))
square
> (square 7)
49
```

Comments starting with `;` are supported:

```lisp
> ; This is a comment
> (+ 1 2)  ; This computes 1 + 2
3
```

### Batch Mode

You can also pipe expressions into the interpreter:

```bash
cat examples.lisp | ./tinylisp
```

## Examples

### Basic Arithmetic

```lisp
(+ 1 2 3)           ; => 6
(- 10 3)            ; => 7
(* 4 5)             ; => 20
(/ 20 4)            ; => 5
(+ (* 2 3) (/ 8 2)) ; => 10
```

### List Operations

```lisp
(quote (1 2 3))     ; => (1 2 3)
'(a b c)            ; => (a b c)
(car '(1 2 3))      ; => 1
(cdr '(1 2 3))      ; => (2 3)
(cons 1 '(2 3))     ; => (1 2 3)
```

### Lambda Functions

```lisp
((lambda (x) (* x x)) 5)        ; => 25
((lambda (x y) (+ x y)) 10 20)  ; => 30
```

### Function Definitions

```lisp
; Define a square function
(defun square (x) (* x x))
(square 7)  ; => 49

; Recursive factorial
(defun factorial (n) 
  (if (eq n 0) 
      1 
      (* n (factorial (- n 1)))))
(factorial 5)  ; => 120

; Recursive Fibonacci
(defun fib (n) 
  (if (< n 2) 
      n 
      (+ (fib (- n 1)) (fib (- n 2)))))
(fib 10)  ; => 55

; List length
(defun length (lst) 
  (if (eq lst nil) 
      0 
      (+ 1 (length (cdr lst)))))
(length '(1 2 3 4 5))  ; => 5
```

## Turing Completeness

This interpreter is Turing complete because it supports:

1. **Recursion**: Functions can call themselves (demonstrated by factorial and fibonacci)
2. **Conditional Branching**: The `if` special form provides decision-making
3. **Arbitrary Data Structures**: Lists can represent any data structure
4. **Function Definitions**: User-defined functions with lexical scoping
5. **Higher-Order Functions**: Lambda functions can be passed as values

These features combined allow the expression of any computable function.

## Implementation Details

### Object Types
- `T_INT` - Integer values
- `T_SYMBOL` - Symbolic atoms
- `T_CONS` - Cons cells (pairs)
- `T_FUNC` - Built-in functions
- `T_LAMBDA` - User-defined functions

### Memory Management
Simple allocation-based memory management with a fixed pool of objects. The interpreter allocates objects as needed up to a maximum limit.

### Environment
The environment is implemented as an association list (alist) where each binding is a cons cell of (symbol . value). Function definitions are stored in a global environment that persists across REPL interactions.

### Parser
Recursive descent parser that:
- Tokenizes input into symbols, numbers, and parentheses
- Builds abstract syntax trees as nested cons cells
- Supports quote syntax sugar (`'expr` → `(quote expr)`)

### Evaluator
The evaluator implements:
- Special forms (quote, if, lambda, defun)
- Function application
- Variable lookup with lexical scoping
- Recursive evaluation

## Limitations

- Fixed maximum number of objects (10,000)
- No garbage collection (objects persist for program lifetime)
- Integer-only arithmetic (no floating point)
- No string type
- No macro system
- Limited error handling

## License

Public domain - feel free to use, modify, and distribute as you wish.

## Credits

Implemented as a demonstration of the core concepts of LISP interpretation in C.
