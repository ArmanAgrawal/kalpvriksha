#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 500

int valueStack[MAX];
char operatorStack[MAX];

int topValue = -1;
int topOperator = -1;
int divisionError = 0;

int applyOp(int a, int b, char op, int *divisionError);
int applyTop();
int precedence(char op);
int isOperator(char ch);

void evaluate(char *expr) {
    topValue = -1;
    topOperator = -1;
    divisionError = 0;

    int i = 0;
    int len = strlen(expr);
    int needOp = 1;

    while (i < len) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }
        if (isdigit(expr[i])) {
            if (!needOp) {
                printf("Error: Invalid Expression\n");
                return;
            }
            int num = 0;
            while (i < len && isdigit(expr[i])) {
                num = num * 10 + (expr[i] - '0');
                i++;
            }
            valueStack[++topValue] = num;
            needOp = 0;
        }
        else if (isOperator(expr[i])) {
            if (needOp) {
                printf("Error: Invalid Expression\n");
                return;
            }

            while (topOperator >= 0 && precedence(operatorStack[topOperator]) >= precedence(expr[i])) {
                if (!applyTop()) {
                    if (divisionError) {
                        printf("Error: Division by zero\n");
                    } else {
                        printf("Error: Invalid Expression\n");
                    }
                    return;
                }
            }
            operatorStack[++topOperator] = expr[i];
            needOp = 1;
            i++;
        }
        else {
            printf("Error: Invalid Expression\n");
            return;
        }
    }

    if (needOp) {
        printf("Error: Invalid Expression\n");
        return;
    }

    while (topOperator >= 0) {
        if (!applyTop()) {
            if (divisionError) {
                printf("Error: Division by zero\n");
            } else {
                printf("Error: Invalid Expression\n");
            }
            return;
        }
    }

    if (topValue == 0) {
        printf("%d\n", valueStack[topValue]);
    } else {
        printf("Error: Invalid Expression\n");
    }
}

int main(void) {
    char input[MAX];

    if (fgets(input, sizeof(input), stdin)) {
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        evaluate(input);
    }

    return 0;
}

int applyTop() {
    if (topValue < 1)
        return 0;

    int b = valueStack[topValue--];
    int a = valueStack[topValue--];
    char op = operatorStack[topOperator--];

    int result = applyOp(a, b, op, &divisionError);

    if (divisionError)
        return 0;

    valueStack[++topValue] = result;

    return 1;
}

int precedence(char op) {
    if (op == '*' || op == '/')
        return 2;
    if (op == '+' || op == '-')
        return 1;
    return 0;
}
int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}
int applyOp(int a, int b, char op, int *divisionError) {
    if (op == '+') {
        return a + b;
    } else if (op == '-') {
        return a - b;
    } else if (op == '*') {
        return a * b;
    } else if (op == '/') {
        if (b == 0) {
            *divisionError = 1;
            return 0;
        }
        return a / b;
    }
    return 0;
}
