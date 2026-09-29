#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX 500

int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

int is_operator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

int apply_op(int a, int b, char op, int *divisionError) {
    switch (op) {
        case '+': return a+ b;
        case '-': return a -b;
        case '*': return a* b;
        case '/':
            if (b == 0) {
                *divisionError =1;
                return 0;
            }
            return a / b;
    }
    return 0;
}

void evaluate(char *expr) {
    int valueStack[MAX];
    char operatorStack[MAX];
    int topValue = -1;
    int topOperator = -1;

    int i = 0;
    int len = strlen(expr);
    int needOperand = 1;
    int divisionError = 0;

    while (i < len) {
        if (isspace(expr[i])) {
            i++;
            continue;
        }

        if (isdigit(expr[i])) {
            if (!needOperand) {
                printf("Error: Invalid Expression\n");
                return;
            }

            int num = 0;
            while (i < len && isdigit(expr[i])) {
                num = num*10 + (expr[i] - '0');
                i++;
            }
            valueStack[++topValue] = num;
            needOperand = 0;
        }
        else if (is_operator(expr[i])) {
            if (needOperand){
                printf("Error: Invalid Expression\n");
                return;
            }
        while (topOperator >= 0 && precedence(operatorStack[topOperator]) >= precedence(expr[i])) {
                if (topValue < 1) {
                    printf("Error: Invalid Expression\n");
                    return;
                }
                int b = valueStack[topValue--];
                int a = valueStack[topValue--];
                char op = operatorStack[topOperator--];

                int res = apply_op(a, b, op, &divisionError);
                if (divisionError) {
                    printf("Error: Division by zero\n");
                    return;
                }
                valueStack[++topValue] = res;
            }

            operatorStack[++topOperator] = expr[i];
            needOperand = 1;
            i++;
        }
        else {
            printf("Error: Invalid Expression\n");
            return;
        }
    }

    if (needOperand){
        printf("Error: Invalid Expression\n");
        return;
    }

    while (topOperator>= 0) {
        if (topValue <1) {
            printf("Error: Invalid Expression\n");
            return;
        }
        int b = valueStack[topValue--];
        int a = valueStack[topValue--];
        char op = operatorStack[topOperator--];

        int res = apply_op(a, b, op, &divisionError);
        if (divisionError) {
            printf("Error: Division by zero\n");
            return;
        }
        valueStack[++topValue]=res;
    }

    if (topValue == 0) {
        printf("%d\n",valueStack[topValue]);
    } else {
        printf("Error: Invalid Expression\n");
    }
}

int main(void) {
    char input[MAX];

    if (fgets(input,sizeof(input),stdin)) {
        size_t len=strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len-1] = '\0';
        }
        evaluate(input);
    }

    return 0;
}