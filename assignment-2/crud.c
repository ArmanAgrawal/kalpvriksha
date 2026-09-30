#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50

typedef struct {
    int id;
    char name[MAX_NAME];
    int age;
} User;

void addUser() {
    User user, temp;
    printf("Enter ID: "); scanf("%d", &user.id);
    printf("Enter Name: "); scanf("%s", user.name);
    printf("Enter Age: "); scanf("%d", &user.age);

    FILE *fp = fopen("users.txt", "r");
        while (fscanf(fp, "%d %s %d", &temp.id, temp.name, &temp.age) == 3) {
            if (temp.id == user.id) {
                printf("Error: ID already exists so try with different id.\n");
                fclose(fp);
                return;
            }
        }
        fclose(fp);

    fp = fopen("users.txt", "a");
    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
    fclose(fp);
    printf("User added.\n");
}

void showUsers() {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) { printf("Error: File not found.\n"); return; }

    User user;
    int get = 0;
    printf("\n Users \n");
    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        printf("ID: %d  Name: %s  Age: %d\n", user.id, user.name, user.age);
        get = 1;
    }
    if (!get) printf("Users Not found.\n");
    fclose(fp);
}

void updateUser() {
    int id, get = 0;
    printf("Enter ID: "); scanf("%d", &id);

    FILE *fpoi = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    User user;
    while (fscanf(fpoi, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            get = 1;
            printf("Enter new name: "); scanf("%s", user.name);
            printf("Enter new age: "); scanf("%d", &user.age);
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }
    fclose(fpoi);
    fclose(temp);

    if (!get) { 
        remove("temp.txt"); 
        printf("Error: User not found.\n"); 
        return; 
    }
    remove("users.txt");
    rename("temp.txt", "users.txt");
    printf("User updated.\n");
}

void deleteUser() {
    int id, get = 0;
    printf("Enter ID to delete: "); scanf("%d", &id);

    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    User user;
    while (fscanf(fp, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) { get = 1; continue; }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
    fclose(temp);

    if (!get) { 
        remove("temp.txt"); 
        printf("Error: User not found.\n"); 
        return;
     }
    remove("users.txt");
    rename("temp.txt", "users.txt");
    printf("User deleted.\n");
}

int main() {
    int choice;
        FILE *fp = fopen("users.txt", "a");
    if (fp) fclose(fp);
    else printf("Error: Unable to create file.\n");
    while (1) {
        printf("\n===== User Management System =====\n1. Create User\n2. Read Users\n3. Update User\n4. Delete User\n5. Exit\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addUser(); break;
            case 2: showUsers(); break;
            case 3: updateUser(); break;
            case 4: deleteUser(); break;
            case 5: printf("Exiting program\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }
    return 0;
}