#include<stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_DESIGNATION 100

struct Employee;

struct ReportNode
{
    struct Employee * emp;
    struct ReportNode *next;
};

struct Employee
{
    int id;
    char name[MAX_NAME];
    char designation[MAX_DESIGNATION];
    struct Employee *manager;
    struct ReportNode *reports_head;
};

struct Employee *top_level_root = NULL;


struct Employee* create_employee(int id,char *name,char *designation);
struct Employee* find_employee(struct Employee * root,int id);
void add_report(struct Employee * manager,struct Employee * subordinate);
void add_empoyees();
int count_direct_reports(struct Employee* emp);
struct Employee * find_sibling_with_fewest_reports(struct Employee * target);
void remove_report_from_manager(struct Employee *manager,struct Employee * subordinate);
void remove_empoyees();
void display_employee_hierarchy(struct Employee *emp, int depth);
int count_subordinates(struct Employee *emp); 
void disply_single_employee_hierarchy();
int count_total_employees(struct Employee *emp); 
void display_complete_hierarchy();


int main(){
    int choice;

    while(1){
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n1. Add Employee\n2. Remove Employee\n3. Display Employee Hierarchy\n4. Display Complete Hierarchy\n5. Exit\n");
        printf("Enter your choice: ");

        if(scanf("%d",&choice)!=1){
            printf("Enter a valid choice between(1-5)");
            getchar();
            continue;
        }
        switch (choice)
        {
        case 1:
            add_empoyees();
            break;
        case 2:
            remove_empoyees();
            break;
        case 3:
            disply_single_employee_hierarchy();
            break;
        case 4:
            display_complete_hierarchy();
            break;
        case 5:
            printf("Exiting program.\n");
            return 0;
        default:
            printf("Invalid choice! Please select 1-5.\n");
            return 0;
        }
    }
}



struct Employee* create_employee(int id,char *name,char *designation){
    struct Employee * emp = (struct Employee*)malloc(sizeof(struct Employee));
    if(!emp){
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    emp->id = id;
    strncpy(emp->name,name,MAX_NAME-1);
    emp->name[MAX_NAME - 1] = '\0';
    strncpy(emp->designation, designation, MAX_DESIGNATION - 1);
    emp->designation[MAX_DESIGNATION - 1] = '\0';
    emp->manager = NULL;
    emp->reports_head = NULL;
    return emp;
}


struct Employee* find_employee(struct Employee * root,int id){
    if(root==NULL)return NULL;
    if(root->id==id) return root;

    struct ReportNode * curr = root->reports_head;
    while(curr!=NULL){
        struct Employee * found = find_employee(curr->emp,id);
        if(found!=NULL)return found;
        curr = curr->next;
    }
    return NULL;

}


void add_report(struct Employee * manager,struct Employee * subordinate){
    struct ReportNode * new_node = (struct ReportNode*)malloc(sizeof(struct ReportNode));
    if(!new_node){
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    new_node->emp = subordinate;
    new_node->next = manager->reports_head;
    manager->reports_head = new_node;
    subordinate->manager = manager;
}

void add_empoyees(){
    int id,manager_id;
    char name[MAX_NAME], designation[MAX_DESIGNATION];

    printf("Enter Employee id: ");
    if (scanf("%d", &id) != 1) return;

    if(top_level_root !=NULL && find_employee(top_level_root,id)!=NULL){
        printf("Employee id already exists\n");
        return;
    }

    getchar();
    printf("Enter Name : ");
    fgets(name,MAX_NAME,stdin);
    name[strcspn(name,"\n")] = '\0';
    printf("Enter Designation : ");
    fgets(designation, MAX_DESIGNATION, stdin);
    designation[strcspn(designation, "\n")] = '\0';
    printf("Enter Manager ID (0 if Top-Level / CEO): ");
    if (scanf("%d", &manager_id) != 1) return;

    struct Employee *new_employee = create_employee(id,name,designation);

    if(manager_id==0){
        if(top_level_root!=NULL){
            printf("Top-level employee already exists\n");
            free(new_employee);
            return;
        }
        top_level_root = new_employee;
        printf("Successfully added as Top-Level Employee\n");
    }else{
        if(top_level_root==NULL){
            printf("Create a top-level manager first\n");
            free(new_employee);
            return;
        }
        struct Employee * manager =find_employee(top_level_root,manager_id);
        if(manager==NULL){
            printf("Manager Not found \n");
            free(new_employee);
            return;
        }

        add_report(manager,new_employee);
        printf("Successfully added %s under manager %s (ID: %d).\n", name, manager->name, manager->id);
    }

}


int count_direct_reports(struct Employee* emp){
    if(emp==NULL)return 0;
    int count = 0;
    struct ReportNode *curr = emp->reports_head;
    while(curr!=NULL){
        count++;
        curr = curr->next;
    }
    return count;
}


struct Employee * find_sibling_with_fewest_reports(struct Employee * target){
    if (target == NULL || target->manager == NULL) return NULL;
    struct Employee * manager = target->manager;
    struct ReportNode * curr = manager->reports_head;

    struct Employee * best_sibling  = NULL;
    int min_employee = 1e9;

    while(curr!=NULL){
        if(curr->emp!=target){
            int current_reports = count_direct_reports(curr->emp);
            if(current_reports<min_employee){
                min_employee = current_reports;
                best_sibling = curr->emp;
            }
        }
        curr = curr->next;
    }
    return best_sibling;

}

void remove_report_from_manager(struct Employee *manager,struct Employee * subordinate){
    if (!manager || !manager->reports_head) return;
    struct ReportNode *curr = manager->reports_head;
    struct ReportNode *prev = NULL;

    while (curr != NULL) {
        if (curr->emp == subordinate) {
            if (prev == NULL) {
                manager->reports_head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            subordinate->manager = NULL;
            return;
        }
        prev = curr;
        curr = curr->next;
    }


}


void remove_empoyees(){
    int id;
    printf("Enter Employee ID to remove: ");
    if (scanf("%d", &id) != 1) {
        printf("Invalid employee ID.\n");
        return;
    }
    if (top_level_root == NULL) {
        printf("Organization hierarchy is empty.\n");
        return;
    }
    struct Employee *target = find_employee(top_level_root,id);
    if(target==NULL){
        printf("Employee ID %d not found \n",id);
        return;
    }
    if (target == top_level_root) {
        printf("CEO cannot be removed.\n");
        return;
    }


    struct Employee *manager = target->manager;
    struct Employee * target_recipient = find_sibling_with_fewest_reports(target); 

    if (target_recipient != NULL) {
        printf("Direct reports will be transferred to sibling %s (ID: %d) who has the fewest direct reports.\n",
               target_recipient->name, target_recipient->id);
    } else {
        target_recipient = manager;
        printf("No siblings found. Direct reports will be transferred directly to superior %s (ID: %d).\n",
               manager->name, manager->id);
    }
    struct ReportNode *curr = target->reports_head;

    while(curr!=NULL){
        struct ReportNode *next = curr->next;
        add_report(target_recipient, curr->emp);
        free(curr);
        curr = next;
    }
    target->reports_head = NULL;
    remove_report_from_manager(manager, target);

    printf("Employee ID %d (%s) removed successfully.\n", target->id, target->name);
    free(target);

}


void display_employee_hierarchy(struct Employee *emp, int depth) {
    if (emp == NULL) return;

    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("%d - %s - %s\n", emp->id, emp->name, emp->designation);

    struct ReportNode *curr = emp->reports_head;
    while (curr != NULL) {
        display_employee_hierarchy(curr->emp, depth + 1);
        curr = curr->next;
    }
}

int count_subordinates(struct Employee *emp) {
    if (emp == NULL) return 0;

    int count = 0;
    struct ReportNode *curr = emp->reports_head;
    while (curr != NULL) {
        count += 1 + count_subordinates(curr->emp);
        curr = curr->next;
    }
    return count;
}

void disply_single_employee_hierarchy(){
    int id;
    printf("Enter Employee ID: ");
    if (scanf("%d", &id) != 1) return;

    if(top_level_root==NULL){
        printf("Organization hierarchy is empty.\n");
        return;
    }
    struct Employee * emp = find_employee(top_level_root,id);
    if(emp==NULL){
        printf("Employee ID %d not found.\n", id);
        return;
    }
    display_employee_hierarchy(emp,0);
    int under_count = count_subordinates(emp);
    printf("Total Employees Under %d: %d\n", emp->id, under_count);
}

int count_total_employees(struct Employee *emp) {
    if (emp == NULL) return 0;
    return 1 + count_subordinates(emp);
}

void display_complete_hierarchy(){
    if(top_level_root==NULL){
        printf("Organization hierarchy is empty \n");
        return;
    }
    printf("===== ORGANIZATION HIERARCHY =====\n");
    display_employee_hierarchy(top_level_root, 0);
    int total = count_total_employees(top_level_root);
    printf("Total Employees: %d\n", total);
}

