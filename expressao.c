#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "expressao.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAX_STACK 100

typedef struct {
    float dados[MAX_STACK];
    int topo;
} PilhaFloat;

typedef struct {
    char *dados[MAX_STACK];
    int topo;
} PilhaStr;

static void limparPilhaStr(PilhaStr *p) {
    while (p->topo >= 0) {
        free(p->dados[p->topo--]);
    }
}

void empilhar(PilhaFloat *p, float v) {
    if (p->topo < MAX_STACK - 1) p->dados[++p->topo] = v;
}

float desempilhar(PilhaFloat *p) {
    if (p->topo >= 0) return p->dados[p->topo--];
    return 0;
}

void push(PilhaStr *p, const char *v) {
    if (p->topo < MAX_STACK - 1) p->dados[++p->topo] = strdup(v);
}

char *pop(PilhaStr *p) {
    if (p->topo >= 0) return p->dados[p->topo--];
    return NULL;
}

char *top(PilhaStr *p) {
    if (p->topo >= 0) return p->dados[p->topo];
    return NULL;
}

int ehOperador(const char *s) {
    return strcmp(s, "+") == 0 || strcmp(s, "-") == 0 || strcmp(s, "*") == 0 ||
           strcmp(s, "/") == 0 || strcmp(s, "%") == 0 || strcmp(s, "^") == 0;
}

int ehFuncao(const char *s) {
    return strcmp(s, "raiz") == 0 || strcmp(s, "sen") == 0 ||
           strcmp(s, "cos") == 0 || strcmp(s, "tg") == 0 || strcmp(s, "log") == 0;
}

int precedencia(const char *op) {
    if (ehFuncao(op)) return 4;
    if (strcmp(op, "^") == 0) return 3;
    if (strcmp(op, "*") == 0 || strcmp(op, "/" ) == 0 || strcmp(op, "%") == 0) return 2;
    if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0) return 1;
    return 0;
}

int associatividadeDireita(const char *op) {
    return strcmp(op, "^") == 0;
}

int ehNumero(const char *s) {
    char *endptr;
    strtod(s, &endptr);
    return endptr != s && *endptr == '\0';
}

float aplicarOperador(float a, float b, const char *op) {
    if (strcmp(op, "+") == 0) return a + b;
    if (strcmp(op, "-") == 0) return a - b;
    if (strcmp(op, "*") == 0) return a * b;
    if (strcmp(op, "/") == 0) {
        if (b == 0) {
            printf("Erro: Divisao por zero\n");
            return INFINITY;
        }
        return a / b;
    }
    if (strcmp(op, "%") == 0) {
        if ((int)b == 0) {
            printf("Erro: Modulo por zero\n");
            return 0;
        }
        return (int)a % (int)b;
    }
    if (strcmp(op, "^") == 0) return powf(a, b);
    return 0;
}

float aplicarFuncao(float x, const char *fun) {
    if (strcmp(fun, "raiz") == 0) return sqrtf(x);
    if (strcmp(fun, "sen") == 0) return sinf(x * M_PI / 180.0);
    if (strcmp(fun, "cos") == 0) return cosf(x * M_PI / 180.0);
    if (strcmp(fun, "tg") == 0)  return tanf(x * M_PI / 180.0);
    if (strcmp(fun, "log") == 0) return log10f(x);
    return 0;
}

char *tokenizarExpressao(const char *expr) {
    char *saida = malloc(2048);
    int j = 0;
    for (int i = 0; expr[i] != '\0'; i++) {
        if (isspace(expr[i])) continue;
        if (isalpha(expr[i])) {
            while (isalpha(expr[i])) saida[j++] = expr[i++];
            saida[j++] = ' ';
            i--;
        } else if (strchr("()+-*/%^", expr[i])) {
            saida[j++] = ' ';
            saida[j++] = expr[i];
            saida[j++] = ' ';
        } else {
            while (isdigit(expr[i]) || expr[i] == '.' || expr[i] == ',') {
                saida[j++] = (expr[i] == ',') ? '.' : expr[i];
                i++;
            }
            saida[j++] = ' ';
            i--;
        }
    }
    saida[j] = '\0';
    return saida;
}

char *getFormaPosFixa(char *Str) {
    char *tokenizada = tokenizarExpressao(Str);
    PilhaStr operadores = {.topo = -1};
    char *saida = malloc(2048);
    saida[0] = '\0';

    char *token = strtok(tokenizada, " ");
    while (token) {
        if (ehNumero(token)) {
            strcat(saida, token);
            strcat(saida, " ");
        } else if (ehFuncao(token) || strcmp(token, "(") == 0) {
            push(&operadores, token);
        } else if (strcmp(token, ")") == 0) {
            while (operadores.topo >= 0 && strcmp(top(&operadores), "(") != 0) {
                strcat(saida, pop(&operadores));
                strcat(saida, " ");
            }
            if (operadores.topo >= 0 && strcmp(top(&operadores), "(") == 0) free(pop(&operadores));
            if (operadores.topo >= 0 && ehFuncao(top(&operadores))) {
                strcat(saida, pop(&operadores));
                strcat(saida, " ");
            }
        } else if (ehOperador(token)) {
            while (operadores.topo >= 0 && ehOperador(top(&operadores)) &&
                   (precedencia(top(&operadores)) > precedencia(token) ||
                    (precedencia(top(&operadores)) == precedencia(token) && !associatividadeDireita(token)))) {
                strcat(saida, pop(&operadores));
                strcat(saida, " ");
            }
            push(&operadores, token);
        } else {
            printf("Erro: token invalido \"%s\"\n", token);
            limparPilhaStr(&operadores);
            free(tokenizada);
            free(saida);
            return NULL;
        }
        token = strtok(NULL, " ");
    }
    while (operadores.topo >= 0) {
        strcat(saida, pop(&operadores));
        strcat(saida, " ");
    }
    limparPilhaStr(&operadores);
    free(tokenizada);
    return saida;
}

char *getFormaInFixa(char *Str) {
    PilhaStr pilha = {.topo = -1};
    char *copy = strdup(Str);
    char *token = strtok(copy, " ");
    while (token) {
        if (ehOperador(token)) {
            char *b = pop(&pilha);
            char *a = pop(&pilha);
            char *expr = malloc(512);
            sprintf(expr, "(%s %s %s)", a, token, b);
            free(a); free(b);
            push(&pilha, expr);
        } else if (ehFuncao(token)) {
            char *x = pop(&pilha);
            char *expr = malloc(512);
            sprintf(expr, "%s(%s)", token, x);
            free(x);
            push(&pilha, expr);
        } else if (ehNumero(token)) {
            push(&pilha, token);
        } else {
            printf("Erro: token invalido \"%s\"\n", token);
            limparPilhaStr(&pilha);
            free(copy);
            return NULL;
        }
        token = strtok(NULL, " ");
    }
    char *res = pop(&pilha);
    char *resultado = res ? strdup(res) : NULL;
    free(res);
    limparPilhaStr(&pilha);
    free(copy);
    return resultado;
}

float getValorPosFixa(char *StrPosFixa) {
    PilhaFloat p = {.topo = -1};
    char *copia = strdup(StrPosFixa);
    char *token = strtok(copia, " ");
    while (token) {
        if (ehOperador(token)) {
            float b = desempilhar(&p);
            float a = desempilhar(&p);
            empilhar(&p, aplicarOperador(a, b, token));
        } else if (ehFuncao(token)) {
            float x = desempilhar(&p);
            empilhar(&p, aplicarFuncao(x, token));
        } else {
            empilhar(&p, atof(token));
        }
        token = strtok(NULL, " ");
    }
    float resultado = desempilhar(&p);
    free(copia);
    return resultado;
}

float getValorInFixa(char *StrInFixa) {
    char *pos = getFormaPosFixa(StrInFixa);
    if (!pos) return INFINITY;
    float val = getValorPosFixa(pos);
    free(pos);
    return val;
}
