#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define NUM_SUBJECTS 3
#define MAX_NAME_LEN 50
#define MAX_MARKS 100
#define MIN_MARKS 0
#define LINE_BUF_SIZE 256

#define GRADE_A_MIN 85.0
#define GRADE_B_MIN 70.0
#define GRADE_C_MIN 50.0
#define GRADE_D_MIN 35.0
#define MIN_PASSING_AVG GRADE_D_MIN

#define STARS_A 5
#define STARS_B 4
#define STARS_C 3
#define STARS_D 2

typedef struct {
  int roll_number;
  char name[MAX_NAME_LEN];
  int marks[NUM_SUBJECTS];
  int total_marks;
  double average_marks;
  char grade;
} Student;

static int g_students_evaluated = 0;

static int calculate_total(const int marks[], int num_subjects);
static double calculate_average(int total_marks, int num_subjects);
static char assign_grade(double average);
static int stars_for_grade(char grade);
static void print_performance_pattern(char grade);
static void print_roll_numbers_recursive(const Student students[], int index,
                                         int total_students);
static bool validate_marks(const int marks[], int num_subjects);
static bool is_duplicate_roll(const Student students[], int count, int roll);
static void evaluate_student(Student *student);
static void print_student(const Student *student);
static void display_results(const Student students[], int total_students);
static bool read_line(char *buf, size_t size, bool skip_blank);
static bool parse_student(const char *line, Student *student);

static int calculate_total(const int marks[], int num_subjects) {
  int total = 0;

  for (int subject_index = 0; subject_index < num_subjects; subject_index++) {
    total += marks[subject_index];
  }

  return total;
}

static double calculate_average(int total_marks, int num_subjects) {
  if (num_subjects <= 0) {
    return 0.0;
  }

  return (double)total_marks / num_subjects;
}

static char assign_grade(double average) {
  if (average >= GRADE_A_MIN) {
    return 'A';
  } else if (average >= GRADE_B_MIN) {
    return 'B';
  } else if (average >= GRADE_C_MIN) {
    return 'C';
  } else if (average >= GRADE_D_MIN) {
    return 'D';
  }

  return 'F';
}

static int stars_for_grade(char grade) {
  switch (grade) {
  case 'A':
    return STARS_A;
  case 'B':
    return STARS_B;
  case 'C':
    return STARS_C;
  case 'D':
    return STARS_D;
  case 'F':
    return 0;
  default:
    return 0;
  }
}

static void print_performance_pattern(char grade) {
  int star_count = stars_for_grade(grade);

  for (int star_index = 0; star_index < star_count; star_index++) {
    putchar('*');
  }

  putchar('\n');
}

static void print_roll_numbers_recursive(const Student students[], int index,
                                         int total_students) {
  if (index >= total_students) {
    return;
  }

  if (index > 0) {
    putchar(' ');
  }

  printf("%d", students[index].roll_number);

  print_roll_numbers_recursive(students, index + 1, total_students);
}

static bool validate_marks(const int marks[], int num_subjects) {
  for (int subject_index = 0; subject_index < num_subjects; subject_index++) {
    if (marks[subject_index] < MIN_MARKS || marks[subject_index] > MAX_MARKS) {
      return false;
    }
  }

  return true;
}

static bool is_duplicate_roll(const Student students[], int count, int roll) {
  for (int student_index = 0; student_index < count; student_index++) {
    if (students[student_index].roll_number == roll) {
      return true;
    }
  }

  return false;
}

static void evaluate_student(Student *student) {
  student->total_marks = calculate_total(student->marks, NUM_SUBJECTS);
  student->average_marks =
      calculate_average(student->total_marks, NUM_SUBJECTS);
  student->grade = assign_grade(student->average_marks);

  g_students_evaluated++;
}

static void print_student(const Student *student) {
  printf("Roll: %d\n", student->roll_number);
  printf("Name: %s\n", student->name);
  printf("Total: %d\n", student->total_marks);
  printf("Average: %.2f\n", student->average_marks);
  printf("Grade: %c\n", student->grade);
}

static void display_results(const Student students[], int total_students) {
  for (int student_index = 0; student_index < total_students; student_index++) {
    print_student(&students[student_index]);

    if (students[student_index].average_marks < MIN_PASSING_AVG) {
      putchar('\n');
      continue;
    }

    printf("Performance: ");
    print_performance_pattern(students[student_index].grade);
    putchar('\n');
  }

  printf("List of Roll Numbers (via recursion): ");
  print_roll_numbers_recursive(students, 0, total_students);
  putchar('\n');
}

static bool read_line(char *buf, size_t size, bool skip_blank) {
  while (fgets(buf, (int)size, stdin) != NULL) {
    size_t len = strlen(buf);

    if (len > 0 && buf[len - 1] != '\n' && !feof(stdin)) {
      return false;
    }

    if (skip_blank && strspn(buf, " \t\r\n") == len) {
      continue;
    }

    return true;
  }

  return false;
}

static bool parse_student(const char *line, Student *student) {
  char name_token[LINE_BUF_SIZE];
  int consumed = 0;

  if (sscanf(line, "%d %255s %d %d %d %n", &student->roll_number, name_token,
             &student->marks[0], &student->marks[1], &student->marks[2],
             &consumed) != 5) {
    return false;
  }

  if (line[consumed] != '\0') {
    return false;
  }

  if (strlen(name_token) >= MAX_NAME_LEN) {
    return false;
  }

  strcpy(student->name, name_token);
  return true;
}

int main(void) {
  char line[LINE_BUF_SIZE];
  int num_students = 0;
  int consumed = 0;

  if (!read_line(line, sizeof line, true) ||
      sscanf(line, "%d %n", &num_students, &consumed) != 1 ||
      line[consumed] != '\0') {
    fprintf(stderr, "Error: Invalid number of students.\n");
    return EXIT_FAILURE;
  }

  if (num_students < 1 || num_students > MAX_STUDENTS) {
    fprintf(stderr, "Error: Number of students must be between 1 and %d.\n",
            MAX_STUDENTS);
    return EXIT_FAILURE;
  }

  Student students[MAX_STUDENTS];

  for (int student_index = 0; student_index < num_students; student_index++) {
    if (!read_line(line, sizeof line, true)) {
      fprintf(stderr, "Error: Missing or oversized record %d.\n",
              student_index + 1);
      return EXIT_FAILURE;
    }

    if (!parse_student(line, &students[student_index])) {
      fprintf(stderr,
              "Error: Invalid input format for student record %d "
              "(name must be under %d characters, marks must be "
              "integers).\n",
              student_index + 1, MAX_NAME_LEN);
      return EXIT_FAILURE;
    }

    if (students[student_index].roll_number <= 0) {
      fprintf(stderr, "Error: Roll number in record %d must be positive.\n",
              student_index + 1);
      return EXIT_FAILURE;
    }

    if (is_duplicate_roll(students, student_index,
                          students[student_index].roll_number)) {
      fprintf(stderr, "Error: Duplicate roll number %d.\n",
              students[student_index].roll_number);
      return EXIT_FAILURE;
    }

    if (!validate_marks(students[student_index].marks, NUM_SUBJECTS)) {
      fprintf(stderr,
              "Error: Marks for student %d must be between %d and %d.\n",
              students[student_index].roll_number, MIN_MARKS, MAX_MARKS);
      return EXIT_FAILURE;
    }

    evaluate_student(&students[student_index]);
  }

  if (read_line(line, sizeof line, true)) {
    fprintf(stderr, "Warning: Extra input after %d records was ignored.\n",
            num_students);
  }

  if (g_students_evaluated != num_students) {
    fprintf(stderr, "Error: Internal evaluation count mismatch.\n");
    return EXIT_FAILURE;
  }

  display_results(students, num_students);

  return EXIT_SUCCESS;
}