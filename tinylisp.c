#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Tiny LISP Interpreter - Turing Complete */

/* Object types */
typedef enum {
    T_INT,      /* Integer */
    T_SYMBOL,   /* Symbol */
    T_CONS,     /* Cons cell (pair) */
    T_FUNC,     /* Built-in function */
    T_LAMBDA    /* User-defined function */
} ObjType;

/* Forward declaration */
typedef struct Obj Obj;

/* Built-in function pointer type */
typedef Obj* (*BuiltinFunc)(Obj* args, Obj* env);

/* Object structure */
struct Obj {
    ObjType type;
    union {
        int num;                    /* For T_INT */
        char* sym;                  /* For T_SYMBOL */
        struct {                    /* For T_CONS */
            Obj* car;
            Obj* cdr;
        } cons;
        BuiltinFunc func;           /* For T_FUNC */
        struct {                    /* For T_LAMBDA */
            Obj* params;            /* Parameter list */
            Obj* body;              /* Function body */
            Obj* env;               /* Closure environment */
        } lambda;
    };
};

/* Global nil and t symbols */
Obj* nil_obj;
Obj* t_obj;

/* Memory management - simple mark-and-sweep GC */
#define MAX_OBJS 10000
Obj* all_objs[MAX_OBJS];
int obj_count = 0;

/* Allocate a new object */
Obj* alloc_obj(ObjType type) {
    if (obj_count >= MAX_OBJS) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }
    Obj* obj = (Obj*)malloc(sizeof(Obj));
    obj->type = type;
    all_objs[obj_count++] = obj;
    return obj;
}

/* Create integer */
Obj* make_int(int num) {
    Obj* obj = alloc_obj(T_INT);
    obj->num = num;
    return obj;
}

/* Create symbol */
Obj* make_symbol(const char* name) {
    Obj* obj = alloc_obj(T_SYMBOL);
    obj->sym = strdup(name);
    return obj;
}

/* Create cons cell */
Obj* cons(Obj* car, Obj* cdr) {
    Obj* obj = alloc_obj(T_CONS);
    obj->cons.car = car;
    obj->cons.cdr = cdr;
    return obj;
}

/* Create built-in function */
Obj* make_func(BuiltinFunc func) {
    Obj* obj = alloc_obj(T_FUNC);
    obj->func = func;
    return obj;
}

/* Create lambda */
Obj* make_lambda(Obj* params, Obj* body, Obj* env) {
    Obj* obj = alloc_obj(T_LAMBDA);
    obj->lambda.params = params;
    obj->lambda.body = body;
    obj->lambda.env = env;
    return obj;
}

/* Check if object is nil */
int is_nil(Obj* obj) {
    return obj == nil_obj;
}

/* List operations */
Obj* car(Obj* obj) {
    if (obj->type != T_CONS) {
        fprintf(stderr, "CAR: not a cons cell\n");
        return nil_obj;
    }
    return obj->cons.car;
}

Obj* cdr(Obj* obj) {
    if (obj->type != T_CONS) {
        fprintf(stderr, "CDR: not a cons cell\n");
        return nil_obj;
    }
    return obj->cons.cdr;
}

/* Print object */
void print_obj(Obj* obj) {
    if (!obj) {
        printf("NULL");
        return;
    }
    
    switch (obj->type) {
        case T_INT:
            printf("%d", obj->num);
            break;
        case T_SYMBOL:
            printf("%s", obj->sym);
            break;
        case T_CONS:
            printf("(");
            print_obj(car(obj));
            obj = cdr(obj);
            while (!is_nil(obj)) {
                if (obj->type == T_CONS) {
                    printf(" ");
                    print_obj(car(obj));
                    obj = cdr(obj);
                } else {
                    printf(" . ");
                    print_obj(obj);
                    break;
                }
            }
            printf(")");
            break;
        case T_FUNC:
            printf("<built-in function>");
            break;
        case T_LAMBDA:
            printf("<lambda>");
            break;
    }
}

/* Tokenizer */
typedef struct {
    char* input;
    int pos;
} Tokenizer;

void skip_whitespace(Tokenizer* t) {
    while (t->input[t->pos]) {
        if (isspace(t->input[t->pos])) {
            t->pos++;
        } else if (t->input[t->pos] == ';') {
            /* Skip comment until end of line */
            while (t->input[t->pos] && t->input[t->pos] != '\n') {
                t->pos++;
            }
        } else {
            break;
        }
    }
}

char* read_token(Tokenizer* t) {
    skip_whitespace(t);
    
    if (!t->input[t->pos]) return NULL;
    
    if (t->input[t->pos] == '(' || t->input[t->pos] == ')') {
        char* token = (char*)malloc(2);
        token[0] = t->input[t->pos++];
        token[1] = '\0';
        return token;
    }
    
    int start = t->pos;
    while (t->input[t->pos] && !isspace(t->input[t->pos]) && 
           t->input[t->pos] != '(' && t->input[t->pos] != ')') {
        t->pos++;
    }
    
    int len = t->pos - start;
    char* token = (char*)malloc(len + 1);
    strncpy(token, &t->input[start], len);
    token[len] = '\0';
    return token;
}

/* Parser */
Obj* parse_expr(Tokenizer* t);

Obj* parse_list(Tokenizer* t) {
    skip_whitespace(t);
    
    /* Check for closing paren */
    if (t->input[t->pos] == ')') {
        return nil_obj;
    }
    
    if (!t->input[t->pos]) {
        fprintf(stderr, "Unexpected EOF in list\n");
        return nil_obj;
    }
    
    Obj* car_obj = parse_expr(t);
    Obj* cdr_obj = parse_list(t);
    return cons(car_obj, cdr_obj);
}

Obj* parse_expr(Tokenizer* t) {
    char* token = read_token(t);
    
    if (!token) return NULL;
    
    if (strcmp(token, "(") == 0) {
        free(token);
        Obj* list = parse_list(t);
        token = read_token(t);
        if (!token || strcmp(token, ")") != 0) {
            fprintf(stderr, "Expected ')'\n");
        }
        if (token) free(token);
        return list;
    }
    
    if (strcmp(token, "'") == 0) {
        free(token);
        Obj* quoted = parse_expr(t);
        return cons(make_symbol("quote"), cons(quoted, nil_obj));
    }
    
    /* Check if it's a number */
    int is_num = 1;
    int i = (token[0] == '-') ? 1 : 0;
    if (!token[i]) is_num = 0;
    for (; token[i]; i++) {
        if (!isdigit(token[i])) {
            is_num = 0;
            break;
        }
    }
    
    if (is_num) {
        int num = atoi(token);
        free(token);
        return make_int(num);
    }
    
    /* It's a symbol */
    Obj* sym = make_symbol(token);
    free(token);
    return sym;
}

/* Environment operations */
Obj* env_lookup(Obj* env, const char* name) {
    while (!is_nil(env)) {
        Obj* pair = car(env);
        if (strcmp(car(pair)->sym, name) == 0) {
            return cdr(pair);
        }
        env = cdr(env);
    }
    return NULL;
}

Obj* env_define(Obj* env, const char* name, Obj* value) {
    return cons(cons(make_symbol(name), value), env);
}

/* Evaluator */
Obj* eval(Obj* expr, Obj* env);

/* Global environment pointer for defun */
Obj** global_env_ptr = NULL;

Obj* eval_list(Obj* list, Obj* env) {
    if (is_nil(list)) return nil_obj;
    return cons(eval(car(list), env), eval_list(cdr(list), env));
}

Obj* eval(Obj* expr, Obj* env) {
    if (!expr) return nil_obj;
    
    /* Self-evaluating types */
    if (expr->type == T_INT) return expr;
    
    /* Symbol lookup */
    if (expr->type == T_SYMBOL) {
        if (strcmp(expr->sym, "nil") == 0) return nil_obj;
        if (strcmp(expr->sym, "t") == 0) return t_obj;
        
        /* Look up in provided environment first, then global */
        Obj* val = env_lookup(env, expr->sym);
        if (!val && global_env_ptr) {
            val = env_lookup(*global_env_ptr, expr->sym);
        }
        if (!val) {
            fprintf(stderr, "Undefined symbol: %s\n", expr->sym);
            return nil_obj;
        }
        return val;
    }
    
    /* List evaluation */
    if (expr->type == T_CONS) {
        Obj* op = car(expr);
        Obj* args = cdr(expr);
        
        /* Special forms */
        if (op->type == T_SYMBOL) {
            if (strcmp(op->sym, "quote") == 0) {
                return car(args);
            }
            
            if (strcmp(op->sym, "if") == 0) {
                Obj* cond = eval(car(args), env);
                if (!is_nil(cond)) {
                    return eval(car(cdr(args)), env);
                } else if (!is_nil(cdr(cdr(args)))) {
                    return eval(car(cdr(cdr(args))), env);
                }
                return nil_obj;
            }
            
            if (strcmp(op->sym, "lambda") == 0) {
                Obj* params = car(args);
                Obj* body = car(cdr(args));
                return make_lambda(params, body, env);
            }
            
            if (strcmp(op->sym, "defun") == 0) {
                Obj* name = car(args);
                Obj* params = car(cdr(args));
                Obj* body = car(cdr(cdr(args)));
                Obj* lambda = make_lambda(params, body, global_env_ptr ? *global_env_ptr : env);
                /* Store in global environment */
                if (global_env_ptr) {
                    *global_env_ptr = env_define(*global_env_ptr, name->sym, lambda);
                }
                return name;
            }
        }
        
        /* Function application */
        Obj* func = eval(op, env);
        
        if (func->type == T_FUNC) {
            /* Built-in function */
            Obj* evaled_args = eval_list(args, env);
            return func->func(evaled_args, env);
        }
        
        if (func->type == T_LAMBDA) {
            /* User-defined function */
            Obj* evaled_args = eval_list(args, env);
            
            /* Bind parameters */
            Obj* new_env = func->lambda.env;
            Obj* params = func->lambda.params;
            Obj* vals = evaled_args;
            
            while (!is_nil(params) && !is_nil(vals)) {
                new_env = env_define(new_env, car(params)->sym, car(vals));
                params = cdr(params);
                vals = cdr(vals);
            }
            
            return eval(func->lambda.body, new_env);
        }
        
        fprintf(stderr, "Not a function\n");
        return nil_obj;
    }
    
    return expr;
}

/* Built-in functions */
Obj* builtin_car(Obj* args, Obj* env) {
    if (is_nil(args)) return nil_obj;
    return car(car(args));
}

Obj* builtin_cdr(Obj* args, Obj* env) {
    if (is_nil(args)) return nil_obj;
    return cdr(car(args));
}

Obj* builtin_cons(Obj* args, Obj* env) {
    if (is_nil(args) || is_nil(cdr(args))) return nil_obj;
    return cons(car(args), car(cdr(args)));
}

Obj* builtin_add(Obj* args, Obj* env) {
    int sum = 0;
    while (!is_nil(args)) {
        Obj* arg = car(args);
        if (arg->type != T_INT) {
            fprintf(stderr, "+: expected integer\n");
            return make_int(0);
        }
        sum += arg->num;
        args = cdr(args);
    }
    return make_int(sum);
}

Obj* builtin_sub(Obj* args, Obj* env) {
    if (is_nil(args)) return make_int(0);
    Obj* first = car(args);
    if (first->type != T_INT) return make_int(0);
    
    if (is_nil(cdr(args))) return make_int(-first->num);
    
    int result = first->num;
    args = cdr(args);
    while (!is_nil(args)) {
        Obj* arg = car(args);
        if (arg->type != T_INT) return make_int(0);
        result -= arg->num;
        args = cdr(args);
    }
    return make_int(result);
}

Obj* builtin_mul(Obj* args, Obj* env) {
    int product = 1;
    while (!is_nil(args)) {
        Obj* arg = car(args);
        if (arg->type != T_INT) return make_int(1);
        product *= arg->num;
        args = cdr(args);
    }
    return make_int(product);
}

Obj* builtin_div(Obj* args, Obj* env) {
    if (is_nil(args)) return make_int(1);
    Obj* first = car(args);
    if (first->type != T_INT) return make_int(1);
    
    int result = first->num;
    args = cdr(args);
    while (!is_nil(args)) {
        Obj* arg = car(args);
        if (arg->type != T_INT || arg->num == 0) {
            fprintf(stderr, "/: division by zero or bad argument\n");
            return make_int(0);
        }
        result /= arg->num;
        args = cdr(args);
    }
    return make_int(result);
}

Obj* builtin_eq(Obj* args, Obj* env) {
    if (is_nil(args) || is_nil(cdr(args))) return nil_obj;
    Obj* a = car(args);
    Obj* b = car(cdr(args));
    
    if (a->type != b->type) return nil_obj;
    if (a->type == T_INT) {
        return (a->num == b->num) ? t_obj : nil_obj;
    }
    return (a == b) ? t_obj : nil_obj;
}

Obj* builtin_lt(Obj* args, Obj* env) {
    if (is_nil(args) || is_nil(cdr(args))) return nil_obj;
    Obj* a = car(args);
    Obj* b = car(cdr(args));
    if (a->type != T_INT || b->type != T_INT) return nil_obj;
    return (a->num < b->num) ? t_obj : nil_obj;
}

Obj* builtin_print(Obj* args, Obj* env) {
    while (!is_nil(args)) {
        print_obj(car(args));
        printf("\n");
        args = cdr(args);
    }
    return nil_obj;
}

/* Initialize environment */
Obj* init_env() {
    Obj* env = nil_obj;
    
    env = env_define(env, "car", make_func(builtin_car));
    env = env_define(env, "cdr", make_func(builtin_cdr));
    env = env_define(env, "cons", make_func(builtin_cons));
    env = env_define(env, "+", make_func(builtin_add));
    env = env_define(env, "-", make_func(builtin_sub));
    env = env_define(env, "*", make_func(builtin_mul));
    env = env_define(env, "/", make_func(builtin_div));
    env = env_define(env, "eq", make_func(builtin_eq));
    env = env_define(env, "<", make_func(builtin_lt));
    env = env_define(env, "print", make_func(builtin_print));
    
    return env;
}

/* REPL */
void repl() {
    nil_obj = alloc_obj(T_SYMBOL);
    nil_obj->sym = strdup("nil");
    
    t_obj = alloc_obj(T_SYMBOL);
    t_obj->sym = strdup("t");
    
    Obj* global_env = init_env();
    global_env_ptr = &global_env;  /* Set global environment pointer */
    
    printf("Tiny LISP Interpreter\n");
    printf("Type expressions to evaluate. Press Ctrl+D to exit.\n");
    printf("Multi-line expressions are supported.\n\n");
    
    char input[4096];
    char line[1024];
    
    while (1) {
        printf("> ");
        fflush(stdout);
        
        input[0] = '\0';
        int paren_depth = 0;
        int first_line = 1;
        
        /* Read lines until we have balanced parentheses or hit EOF */
        while (1) {
            if (!fgets(line, sizeof(line), stdin)) {
                if (strlen(input) == 0) {
                    printf("\n");
                    return;
                }
                break;
            }
            
            /* Check if line is just a comment or whitespace */
            int is_empty = 1;
            int in_comment = 0;
            for (int i = 0; line[i]; i++) {
                if (line[i] == ';') in_comment = 1;
                if (!in_comment && !isspace(line[i])) {
                    is_empty = 0;
                    break;
                }
            }
            
            if (is_empty && first_line) {
                break;  /* Empty line at start, skip it */
            }
            
            /* Append to input buffer */
            if (strlen(input) + strlen(line) < sizeof(input) - 1) {
                strcat(input, line);
            }
            
            /* Count parentheses */
            in_comment = 0;
            for (int i = 0; line[i]; i++) {
                if (line[i] == ';') in_comment = 1;
                if (line[i] == '\n') in_comment = 0;
                if (!in_comment) {
                    if (line[i] == '(') paren_depth++;
                    if (line[i] == ')') paren_depth--;
                }
            }
            
            first_line = 0;
            
            /* If we have balanced parens and at least one expr, we're done */
            if (paren_depth == 0 && strlen(input) > 0) {
                /* Check if we have any non-whitespace/comment content */
                int has_content = 0;
                in_comment = 0;
                for (int i = 0; input[i]; i++) {
                    if (input[i] == ';') in_comment = 1;
                    if (input[i] == '\n') in_comment = 0;
                    if (!in_comment && !isspace(input[i])) {
                        has_content = 1;
                        break;
                    }
                }
                if (has_content) break;
            }
            
            if (paren_depth > 0) {
                printf("  ");  /* Continuation prompt */
                fflush(stdout);
            }
        }
        
        if (strlen(input) == 0) continue;
        
        Tokenizer t = {input, 0};
        Obj* expr = parse_expr(&t);
        
        if (expr) {
            Obj* result = eval(expr, global_env);
            print_obj(result);
            printf("\n");
        }
    }
}

int main() {
    repl();
    return 0;
}
