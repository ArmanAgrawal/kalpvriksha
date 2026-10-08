#include <stdio.h>

#define MAX_STUDENTS 100
#define MAX_SIZE 50
#define MAX_SUBJECT 3

struct Student
{
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

int main()
{
    int n;
    while (scanf("%d", &n) != 1 || n < 1 || n > 100){
        while (getchar() != '\n');
        printf("Enter a valid number between 1 to 100 \n");
    }
    getchar();
    struct Student students[100];
    for (int i = 0; i < n; i++){
        scanf("%d", &students[i].rollnumber);

        scanf("%49[^0-9]", students[i].name);

        scanf("%f %f %f", &students[i].marks[0], &students[i].marks[1], &students[i].marks[2]);
        getchar();
        calculatePerformance(&students[i]);
    }
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
