; Factorial
(defun factorial (n) 
  (if (eq n 0) 
      1 
      (* n (factorial (- n 1)))))

(factorial 5)
(factorial 10)

; Fibonacci
(defun fib (n) 
  (if (< n 2) 
      n 
      (+ (fib (- n 1)) (fib (- n 2)))))

(fib 10)
(fib 12)

; List length
(defun length (lst) 
  (if (eq lst nil) 
      0 
      (+ 1 (length (cdr lst)))))

(length '(1 2 3 4 5))
(length '(a b c d e f g h))

; Sum of list
(defun sum (lst) 
  (if (eq lst nil) 
      0 
      (+ (car lst) (sum (cdr lst)))))

(sum '(1 2 3 4 5))
(sum '(10 20 30 40))

; Power function
(defun power (base exp)
  (if (eq exp 0)
      1
      (* base (power base (- exp 1)))))

(power 2 8)
(power 3 4)

; Map function (higher-order)
(defun map (f lst)
  (if (eq lst nil)
      nil
      (cons (f (car lst)) (map f (cdr lst)))))

(defun square (x) (* x x))
(map square '(1 2 3 4 5))

; List membership
(defun member (x lst)
  (if (eq lst nil)
      nil
      (if (eq x (car lst))
          t
          (member x (cdr lst)))))

(member 3 '(1 2 3 4 5))
(member 7 '(1 2 3 4 5))
