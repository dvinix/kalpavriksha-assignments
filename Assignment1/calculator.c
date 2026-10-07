#include <ctype.h>
#include <stdio.h>

#include <limits.h>

#define MAX 1000

char expr[MAX];
int pos = 0;
int error = 0;
int division_by_zero = 0;

void skip_spaces() {
  while (expr[pos] == ' ' || expr[pos] == '\t') {
    pos++;
  }
}

int get_number() {
  long long num = 0;
  int sign = 1;

  skip_spaces();

  if (expr[pos] == '+' || expr[pos] == '-') {
    if (expr[pos] == '-') {
      sign = -1;
    }
    pos++;
    skip_spaces();
  }

  if (!isdigit(expr[pos])) {
    error = 1;
    return 0;
  }

  while (isdigit(expr[pos])) {
    num = num * 10 + (expr[pos] - '0');
    if ((sign == 1 && num > INT_MAX) || (sign == -1 && -num < INT_MIN)) {
      error = 1;
      return 0;
    }
    pos++;
  }

  return (int)(sign * num);
}

int get_term() {
  int result = get_number();

  while (!error) {
    skip_spaces();

    if (expr[pos] == '*') {
      pos++;
      result *= get_number();
    } else if (expr[pos] == '/') {
      int number;

      pos++;
      number = get_number();

      if (error)
        return 0;

      if (number == 0) {
        division_by_zero = 1;
        return 0;
      }

      result /= number;
    } else {
      break;
    }
  }

  return result;
}

int expr_result() {
  int result = get_term();

  while (!error && !division_by_zero) {
    skip_spaces();

    if (expr[pos] == '+') {
      pos++;
      result += get_term();
    } else if (expr[pos] == '-') {
      pos++;
      result -= get_term();
    } else {
      break;
    }
  }

  return result;
}

int main() {
  int result;

  printf("Enter the Expression: ");
  if (fgets(expr, MAX, stdin) == NULL) {
    printf("Error: Invalid Expression.\n");
    return 0;
  }

  result = expr_result();

  skip_spaces();

  if (division_by_zero) {
    printf("Error: Division by zero.\n");
  } else if (error || (expr[pos] != '\0' && expr[pos] != '\n')) {
    printf("Error: Invalid Expression.\n");
  } else {
    printf("%d\n", result);
  }

  return 0;
}