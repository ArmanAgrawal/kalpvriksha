#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STUDENTS 100
#define MAX_SIZE 50
#define MAX_SUBJECT 3

int studentCount = 0;

struct Student {
    int rollnumber;
    char name[MAX_SIZE];
    float marks[MAX_SUBJECT];
    float total;
    float average;
    char grade;
};

void calculatePerformance(struct Student *s);
char assignGrade(float average);
void printPerformancePattern(char grade);
void printRollNumbersRecursive(struct Student students[], int index, int count);
void trimSpaces(char str[]);
void studentsInput(struct Student students[], int n);

int main()
{
    printf("Enter the number of students (1-100): ");
    int n;
    while (scanf("%d", &n) != 1 || n < 1 || n > 100){
        while (getchar() != '\n');
        printf("Enter a valid number between 1 to 100 \n");
    }
    studentCount = n;
    while (getchar() != '\n');
    struct Student students[MAX_STUDENTS];
    
    studentsInput(students, n);


    for (int i = 0; i < n; i++){
        printf("Roll: %d\n", students[i].rollnumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %.0f\n", students[i].total);
        printf("Average: %.2f\n", students[i].average);
        printf("Grade: %c\n", students[i].grade);

        if (students[i].average < 35.0f){
            printf("\n");
            continue;
        }

        printf("Performance: ");
        printPerformancePattern(students[i].grade);
        printf("\n\n");
    }

    printf("List of Roll Numbers : ");
    printRollNumbersRecursive(students, 0, n);
    printf("\n");

    return 0;
}

void studentsInput(struct Student students[], int n) {
    char inputDetails[256];

    for (int i = 0; i < n; i++) {
        while (1) {
            printf("\nEnter details for student %d (RollNo Name Mark1 Mark2 Mark3): ", i + 1);
            if (fgets(inputDetails, sizeof(inputDetails), stdin) == NULL) {
                continue;
            }

            int roll;
            char tempName[MAX_SIZE];
            float m1, m2, m3;

            if (sscanf(inputDetails, "%d %49[a-zA-Z ] %f %f %f", &roll, tempName, &m1, &m2, &m3) != 5) {
                printf("Invalid input enter numeric roll, name, and 3 marks.\n");
                continue;
            }

            int unique = 1;
            for (int j = 0; j < i; j++) {
                if (students[j].rollnumber == roll) {
                    printf("Roll number %d already exists.\n", roll);
                    unique = 0;
                    break;
                }
            }
            if (!unique) continue;

            if (m1 < 0.0f || m1 > 100.0f || m2 < 0.0f || m2 > 100.0f || m3 < 0.0f || m3 > 100.0f) {
                printf("Marks must be between 0 and 100.\n");
                continue;
            }

            students[i].rollnumber = roll;
            trimSpaces(tempName);
            strcpy(students[i].name, tempName);
            students[i].marks[0] = m1;
            students[i].marks[1] = m2;
            students[i].marks[2] = m3;

            calculatePerformance(&students[i]);
            break;
        }
    }
}


void calculatePerformance(struct Student *s){
    float sum = 0.0f;
    for (int i = 0; i < MAX_SUBJECT; i++){
        sum += s->marks[i];
    }
    s->total = sum;
    s->average = sum / MAX_SUBJECT;
    s->grade = assignGrade(s->average);
}

char assignGrade(float average){
    if (average >= 85.0f){
        return 'A';
    }else if (average >= 70.0f){
        return 'B';
    }else if (average >= 50.0f){
        return 'C';
    }else if (average >= 35.0f){
        return 'D';
    }else{
        return 'F';
    }
}

void printPerformancePattern(char grade){
    int stars = 0;
    if (grade == 'A'){
        stars = 5;
    }else if (grade == 'B'){
        stars = 4;
    }else if (grade == 'C'){
        stars = 3;
    }else if (grade == 'D'){
        stars = 2;
    }else{
        stars = 0;
    }
    
    for (int i = 0; i < stars; i++){
        printf("*");
    }
}

void printRollNumbersRecursive(struct Student students[], int index, int count){
    if (index >= count){
        return;
    }
    printf("%d ", students[index].rollnumber);
    printRollNumbersRecursive(students, index + 1, count);
}


void trimSpaces(char str[]){
    int s = 0;
    int e = strlen(str)-1;
    
    while(str[s]!= '\0' && isspace(str[s])){
        s++;
    }
    while(e>=s && isspace(str[e])){
        e--;
    }
    int i = 0;
    while(s<=e){
        str[i++] = str[s++];
    }
    str[i] = '\0';
}