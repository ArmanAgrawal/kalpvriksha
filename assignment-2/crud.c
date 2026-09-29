#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
} User;

void createFile() {
    FILE *fp = fopen("users.txt", "a");
    if (fp) fclose(fp);
    else printf("Error: Unable to create file.\n");
}

void createUser() {
    User user, temp;
    printf("Enter ID: "); scanf("%d", &user.id);
    printf("Enter Name: "); scanf("%s", user.name);
    printf("Enter Age: "); scanf("%d", &user.age);

    FILE *fp = fopen("users.txt", "r");
    if (fp) {
        while (fscanf(fp, "%d %s %d", &temp.id, temp.name, &temp.age) == 3) {
            if (temp.id == user.id) {
                printf("Error: ID already exists.\n");
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }

    fp = fopen("users.txt", "a");
    if (!fp) { printf("Error: Unable to open file.\n"); return; }
    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
    fclose(fp);
    printf("User added successfully.\n");
}

void readUsers() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) { printf("Error: File not found.\n"); return; }

    User user;
    int found = 0;
    printf("\n----- Users -----\n");
    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        printf("ID: %d  Name: %s  Age: %d\n", user.id, user.name, user.age);
        found = 1;
    }
    if (!found) printf("No users found.\n");
    fclose(fp);
}

void updateUser() {
    int id, found = 0;
    printf("Enter ID to update: "); scanf("%d", &id);

    FILE *fp = fopen("users.txt", "r");
    if (!fp) { printf("Error: File not found.\n"); return; }
    FILE *temp = fopen("temp.txt", "w");
    if (!temp) { printf("Error: Unable to create temporary file.\n"); fclose(fp); return; }

    User user;
    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            found = 1;
            printf("Enter new name: "); scanf("%s", user.name);
            printf("Enter new age: "); scanf("%d", &user.age);
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
    fclose(temp);

    if (!found) { remove("temp.txt"); printf("Error: User not found.\n"); return; }
    remove("users.txt");
    rename("temp.txt", "users.txt");
    printf("User updated successfully.\n");
}

void deleteUser() {
    int id, found = 0;
    printf("Enter ID to delete: "); scanf("%d", &id);

    FILE *fp = fopen("users.txt", "r");
    if (!fp) { printf("Error: File not found.\n"); return; }
    FILE *temp = fopen("temp.txt", "w");
    if (!temp) { printf("Error: Unable to create temporary file.\n"); fclose(fp); return; }

    User user;
    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) { found = 1; continue; }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
    fclose(temp);

    if (!found) { remove("temp.txt"); printf("Error: User not found.\n"); return; }
    remove("users.txt");
    rename("temp.txt", "users.txt");
    printf("User deleted successfully.\n");
}

int main() {
    int choice;
    createFile();
    while (1) {
        printf("\n===== User Management System =====\n1. Create User\n2. Read Users\n3. Update User\n4. Delete User\n5. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: createUser(); break;
            case 2: readUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 5: printf("Exiting program...\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}