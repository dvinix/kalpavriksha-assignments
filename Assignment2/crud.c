#include <stdio.h>
#include <string.h>

#define FILE_NAME "users.txt"
#define TEMP_FILE_NAME "temp.txt"

struct User {
  int id;
  char name[50];
  int age;
};

void clearInputBuffer() {
  int c;
  while ((c = getchar()) != '\n' && c != EOF)
    ;
}

int idExists(int id) {
  struct User user;
  FILE *file = fopen(FILE_NAME, "r");
  if (file == NULL) {
    return 0;
  }

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {
    if (user.id == id) {
      fclose(file);
      return 1;
    }
  }

  fclose(file);
  return 0;
}

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
  FILE *file;

  printf("Enter ID: ");
  if (scanf("%d", &user.id) != 1) {
    printf("Invalid ID. Must be a number.\n");
    clearInputBuffer();
    return;
  }
  clearInputBuffer();

  if (user.id <= 0) {
    printf("ID must be positive.\n");
    return;
  }

  if (idExists(user.id)) {
    printf("Error: User with ID %d already exists.\n", user.id);
    return;
  }

  printf("Enter Name: ");
  if (fgets(user.name, sizeof(user.name), stdin) == NULL) {
    printf("Error reading name.\n");
    return;
  }
  user.name[strcspn(user.name, "\r\n")] = '\0';

  if (strlen(user.name) == 0) {
    printf("Error: Name cannot be empty.\n");
    return;
  }

  if (strchr(user.name, '|') != NULL) {
    printf("Error: Name cannot contain the '|' character.\n");
    return;
  }

  printf("Enter Age: ");
  if (scanf("%d", &user.age) != 1) {
    printf("Invalid Age. Must be a number.\n");
    clearInputBuffer();
    return;
  }
  clearInputBuffer();

  if (user.age < 0 || user.age > 150) {
    printf("Invalid Age range.\n");
    return;
  }

  file = fopen(FILE_NAME, "a");
  if (file == NULL) {
    printf("Error opening file.\n");
    return;
  }

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
  FILE *file;
  FILE *temp;

  printf("Enter ID to update: ");
  if (scanf("%d", &id) != 1) {
    printf("Invalid ID. Must be a number.\n");
    clearInputBuffer();
    return;
  }
  clearInputBuffer();

  file = fopen(FILE_NAME, "r");
  if (file == NULL) {
    printf("Error opening file or no users found.\n");
    return;
  }

  temp = fopen(TEMP_FILE_NAME, "w");
  if (temp == NULL) {
    printf("Error creating temporary file.\n");
    fclose(file);
    return;
  }

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {
    if (user.id == id) {
      char newName[50];
      int newAge;

      found = 1;

      printf("Enter new name: ");
      if (fgets(newName, sizeof(newName), stdin) == NULL) {
        printf("Error reading name.\n");
      } else {
        newName[strcspn(newName, "\r\n")] = '\0';
        if (strlen(newName) > 0 && strchr(newName, '|') == NULL) {
          strcpy(user.name, newName);
        } else if (strchr(newName, '|') != NULL) {
          printf("Name contains '|', retaining old name.\n");
        }
      }

      printf("Enter new age: ");
      if (scanf("%d", &newAge) == 1 && newAge >= 0 && newAge <= 150) {
        user.age = newAge;
      } else {
        printf("Invalid age entered, retaining old age.\n");
      }
      clearInputBuffer();
    }

    fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
  }

  fclose(file);
  fclose(temp);

  rename(TEMP_FILE_NAME, FILE_NAME);

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
  FILE *file;
  FILE *temp;

  printf("Enter ID to delete: ");
  if (scanf("%d", &id) != 1) {
    printf("Invalid ID. Must be a number.\n");
    clearInputBuffer();
    return;
  }
  clearInputBuffer();

  file = fopen(FILE_NAME, "r");
  if (file == NULL) {
    printf("Error opening file or no users found.\n");
    return;
  }

  temp = fopen(TEMP_FILE_NAME, "w");
  if (temp == NULL) {
    printf("Error creating temporary file.\n");
    fclose(file);
    return;
  }

  while (fscanf(file, "%d|%49[^|]|%d", &user.id, user.name, &user.age) == 3) {
    if (user.id == id) {
      found = 1;
      continue;
    }

    fprintf(temp, "%d|%s|%d\n", user.id, user.name, user.age);
  }

  fclose(file);
  fclose(temp);

  rename(TEMP_FILE_NAME, FILE_NAME);

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
    if (scanf("%d", &choice) != 1) {
      printf("Invalid input. Please enter a valid number.\n");
      clearInputBuffer();
      continue;
    }

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