#include <stdio.h>

#define FILE_NAME "users.txt"

struct User {
  int id;
  char name[50];
  int age;
};

void createFile() {
  FILE *file = fopen(FILE_NAME, "a");

  if (file == NULL) {
    printf("Error creating file.\n");
    return;
  }

  fclose(file);
}

void addUser() {
  struct User user;
  FILE *file = fopen(FILE_NAME, "a");

  if (file == NULL) {
    printf("Error opening file.\n");
    return;
  }

  printf("Enter ID: ");
  scanf("%d", &user.id);

  printf("Enter Name: ");
  scanf(" %49[^\n]", user.name);

  printf("Enter Age: ");
  scanf("%d", &user.age);

  fprintf(file, "%d|%s|%d\n", user.id, user.name, user.age);

  fclose(file);

  printf("User added successfully.\n");
}

void displayUsers() {
  struct User user;
  FILE *file = fopen(FILE_NAME, "r");

  if (file == NULL) {
    printf("No users found.\n");
    return;
  }

  printf("\nUser List\n");

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {

    printf("ID: %d | Name: %s | Age: %d\n", user.id, user.name, user.age);
  }

  fclose(file);
}

void updateUser() {
  struct User user;
  int id;
  int found = 0;

  FILE *file = fopen(FILE_NAME, "r");
  FILE *temp = fopen("temp.txt", "w");

  if (file == NULL || temp == NULL) {
    printf("Error opening file.\n");
    return;
  }

  printf("Enter ID to update: ");
  scanf("%d", &id);

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {

    if (user.id == id) {
      found = 1;

      printf("Enter new name: ");
      scanf(" %49[^\n]", user.name);

      printf("Enter new age: ");
      scanf("%d", &user.age);
    }

    fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
  }

  fclose(file);
  fclose(temp);

  remove(FILE_NAME);
  rename("temp.txt", FILE_NAME);

  if (found) {
    printf("User updated successfully.\n");
  } else {
    printf("User ID not found.\n");
  }
}

void deleteUser() {
  struct User user;
  int id;
  int found = 0;

  FILE *file = fopen(FILE_NAME, "r");
  FILE *temp = fopen("temp.txt", "w");

  if (file == NULL || temp == NULL) {
    printf("Error opening file.\n");
    return;
  }

  printf("Enter ID to delete: ");
  scanf("%d", &id);

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {

    if (user.id == id) {
      found = 1;
      continue;
    }

    fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
  }

  fclose(file);
  fclose(temp);

  remove(FILE_NAME);
  rename("temp.txt", FILE_NAME);

  if (found) {
    printf("User deleted successfully.\n");
  } else {
    printf("User ID not found.\n");
  }
}

int main() {
  int choice;

  createFile();

  while (1) {

    printf("\nUser Management System\n");
    printf("1. Add User\n");
    printf("2. Display Users\n");
    printf("3. Update User\n");
    printf("4. Delete User\n");
    printf("5. Exit\n");

    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {

    case 1:
      addUser();
      break;

    case 2:
      displayUsers();
      break;

    case 3:
      updateUser();
      break;

    case 4:
      deleteUser();
      break;

    case 5:
      printf("Program ended.\n");
      return 0;

    default:
      printf("Invalid choice.\n");
    }
  }

  return 0;
}