#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LENGTH 50
#define DESIGNATION_LENGTH 50
#define INPUT_BUFFER_SIZE 128
#define INDENT_WIDTH 4

typedef struct Employee {
    int employeeId;
    char name[NAME_LENGTH];
    char designation[DESIGNATION_LENGTH];
    struct Employee *reportingManager;
    struct Employee *firstReport;
    struct Employee *nextSibling;
} Employee;

static Employee *organizationRoot = NULL;

static void readLine(const char *prompt, char *buffer, size_t bufferSize)
{
    size_t length;

    printf("%s", prompt);
    if (fgets(buffer, (int)bufferSize, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }
}

static int readInteger(const char *prompt, int *value)
{
    char inputBuffer[INPUT_BUFFER_SIZE];
    char *parseEnd;
    long parsedValue;

    readLine(prompt, inputBuffer, sizeof(inputBuffer));
    if (inputBuffer[0] == '\0') {
        return 0;
    }
    parsedValue = strtol(inputBuffer, &parseEnd, 10);
    if (*parseEnd != '\0') {
        return 0;
    }
    *value = (int)parsedValue;
    return 1;
}

static Employee *createEmployee(int employeeId, const char *name,
                                const char *designation, Employee *manager)
{
    Employee *newEmployee = (Employee *)calloc(1, sizeof(Employee));

    if (newEmployee == NULL) {
        return NULL;
    }
    newEmployee->employeeId = employeeId;
    strncpy(newEmployee->name, name, NAME_LENGTH - 1);
    strncpy(newEmployee->designation, designation, DESIGNATION_LENGTH - 1);
    newEmployee->reportingManager = manager;
    return newEmployee;
}

static Employee *findEmployee(Employee *current, int employeeId)
{
    Employee *foundEmployee;

    if (current == NULL) {
        return NULL;
    }
    if (current->employeeId == employeeId) {
        return current;
    }
    foundEmployee = findEmployee(current->firstReport, employeeId);
    if (foundEmployee != NULL) {
        return foundEmployee;
    }
    return findEmployee(current->nextSibling, employeeId);
}

static void appendToReportList(Employee **listHead, Employee *chain)
{
    Employee **tailSlot = listHead;

    while (*tailSlot != NULL) {
        tailSlot = &(*tailSlot)->nextSibling;
    }
    *tailSlot = chain;
}

static void detachFromReportList(Employee **listHead, Employee *target)
{
    Employee **slot = listHead;

    while (*slot != NULL && *slot != target) {
        slot = &(*slot)->nextSibling;
    }
    if (*slot == target) {
        *slot = target->nextSibling;
        target->nextSibling = NULL;
    }
}

static void assignManagerToChain(Employee *chain, Employee *newManager)
{
    if (chain == NULL) {
        return;
    }
    chain->reportingManager = newManager;
    assignManagerToChain(chain->nextSibling, newManager);
}

static int countReports(const Employee *chain);

static int countSubtree(const Employee *employee)
{
    if (employee == NULL) {
        return 0;
    }
    return 1 + countReports(employee->firstReport);
}

static int countReports(const Employee *chain)
{
    if (chain == NULL) {
        return 0;
    }
    return countSubtree(chain) + countReports(chain->nextSibling);
}

static void displayReports(const Employee *chain, int level);

static void displaySubtree(const Employee *employee, int level)
{
    if (employee == NULL) {
        return;
    }
    printf("%*s%d - %s - %s\n", level * INDENT_WIDTH, "",
           employee->employeeId, employee->name, employee->designation);
    displayReports(employee->firstReport, level + 1);
}

static void displayReports(const Employee *chain, int level)
{
    if (chain == NULL) {
        return;
    }
    displaySubtree(chain, level);
    displayReports(chain->nextSibling, level);
}

static void freeReports(Employee *chain)
{
    if (chain == NULL) {
        return;
    }
    freeReports(chain->firstReport);
    freeReports(chain->nextSibling);
    free(chain);
}

static void addEmployee(void)
{
    int employeeId;
    int managerId;
    char name[NAME_LENGTH];
    char designation[DESIGNATION_LENGTH];
    Employee *manager = NULL;
    Employee *newEmployee;

    if (!readInteger("Enter Employee ID: ", &employeeId) || employeeId <= 0) {
        printf("Error: Employee ID must be a positive number.\n");
        return;
    }
    if (findEmployee(organizationRoot, employeeId) != NULL) {
        printf("Error: Employee ID %d already exists.\n", employeeId);
        return;
    }

    readLine("Enter Name: ", name, sizeof(name));
    readLine("Enter Designation: ", designation, sizeof(designation));
    if (name[0] == '\0' || designation[0] == '\0') {
        printf("Error: Name and designation cannot be empty.\n");
        return;
    }

    if (organizationRoot == NULL) {
        printf("Organization is empty. %s becomes the top-level employee.\n", name);
    } else {
        if (!readInteger("Enter Reporting Manager ID: ", &managerId)) {
            printf("Error: Invalid manager ID.\n");
            return;
        }
        if (managerId == employeeId) {
            printf("Error: An employee cannot report to themselves.\n");
            return;
        }
        manager = findEmployee(organizationRoot, managerId);
        if (manager == NULL) {
            printf("Error: Reporting manager %d not found.\n", managerId);
            return;
        }
    }

    newEmployee = createEmployee(employeeId, name, designation, manager);
    if (newEmployee == NULL) {
        printf("Error: Out of memory.\n");
        return;
    }

    if (manager == NULL) {
        organizationRoot = newEmployee;
    } else {
        appendToReportList(&manager->firstReport, newEmployee);
    }
    printf("Employee %d added successfully.\n", employeeId);
}

static void removeEmployee(void)
{
    int employeeId;
    Employee *target;
    Employee *manager;
    Employee *promotedEmployee;
    Employee *remainingReports;

    if (!readInteger("Enter Employee ID to remove: ", &employeeId)) {
        printf("Error: Invalid employee ID.\n");
        return;
    }
    target = findEmployee(organizationRoot, employeeId);
    if (target == NULL) {
        printf("Error: Employee %d not found.\n", employeeId);
        return;
    }

    manager = target->reportingManager;

    if (manager != NULL) {
        detachFromReportList(&manager->firstReport, target);
        assignManagerToChain(target->firstReport, manager);
        appendToReportList(&manager->firstReport, target->firstReport);
        printf("Employee %d removed. Their reports now report to %d.\n",
               employeeId, manager->employeeId);
    } else if (target->firstReport == NULL) {
        organizationRoot = NULL;
        printf("Employee %d removed. The organization is now empty.\n", employeeId);
    } else {
        promotedEmployee = target->firstReport;
        remainingReports = promotedEmployee->nextSibling;

        promotedEmployee->nextSibling = NULL;
        promotedEmployee->reportingManager = NULL;
        assignManagerToChain(remainingReports, promotedEmployee);
        appendToReportList(&promotedEmployee->firstReport, remainingReports);

        organizationRoot = promotedEmployee;
        printf("Employee %d removed. %d (%s) is the new top-level employee.\n",
               employeeId, promotedEmployee->employeeId, promotedEmployee->name);
    }

    free(target);
}

static void displayEmployeeHierarchy(void)
{
    int employeeId;
    Employee *selectedEmployee;

    if (!readInteger("Enter Employee ID: ", &employeeId)) {
        printf("Error: Invalid employee ID.\n");
        return;
    }
    selectedEmployee = findEmployee(organizationRoot, employeeId);
    if (selectedEmployee == NULL) {
        printf("Error: Employee %d not found.\n", employeeId);
        return;
    }

    printf("\n===== HIERARCHY OF EMPLOYEE %d =====\n", employeeId);
    displaySubtree(selectedEmployee, 0);
    printf("Total Employees Under %d: %d\n", employeeId,
           countReports(selectedEmployee->firstReport));
}

static void displayCompleteHierarchy(void)
{
    if (organizationRoot == NULL) {
        printf("The organization has no employees.\n");
        return;
    }
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    displaySubtree(organizationRoot, 0);
    printf("Total Employees: %d\n", countSubtree(organizationRoot));
}

int main(void)
{
    int menuChoice;
    int isRunning = 1;

    while (isRunning) {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        if (!readInteger("Enter your choice: ", &menuChoice)) {
            if (feof(stdin)) {
                break;
            }
            printf("Invalid choice. Please enter a number from 1 to 5.\n");
            continue;
        }

        switch (menuChoice) {
            case 1:
                addEmployee();
                break;
            case 2:
                removeEmployee();
                break;
            case 3:
                displayEmployeeHierarchy();
                break;
            case 4:
                displayCompleteHierarchy();
                break;
            case 5:
                isRunning = 0;
                break;
            default:
                printf("Invalid choice. Please enter a number from 1 to 5.\n");
                break;
        }
    }

    freeReports(organizationRoot);
    printf("Exiting program.\n");
    return 0;
}