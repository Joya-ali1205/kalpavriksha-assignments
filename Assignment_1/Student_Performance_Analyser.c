#include <stdio.h>

struct Student{
    int rollno;
    char name[50];
    int marks[3];
};

int calculateTotal(struct Student student){
    int total;
    total=student.marks[0]+student.marks[1]+student.marks[2];
    return total;
}

float calculateAvg(int total){
    float avg=total/3.0;
    return avg;
}

char calculateGrade(float avg){
    char grade;
    if (avg >= 85){
        grade='A';
    }
    else if (avg >= 70){
        grade='B';
    }
    else if (avg >= 50){
        grade='C';
    }
    else if (avg >= 35){
        grade='D';
    }
    else{
        grade='F';
    }
    return grade;

}

void printRollNumbers(struct Student student[], int i, int n){
    if (i==n){
        return;
    }
    printf("%d ",student[i].rollno);
    printRollNumbers(student, i+1, n);
}

void readStudents(struct Student student[], int i, int n){
    while (i!=n){
        int total=calculateTotal(student[i]);
        float avg=calculateAvg(total);
        char grade=calculateGrade(avg);
        printf("Roll: %d\n",student[i].rollno);
        printf("Name: %s\n",student[i].name);
        printf("Total: %d\n",total);
        printf("Average: %.2f\n",avg);
        printf("Grade: %c\n",grade);
        if (grade=='A'){
            printf("Performance: *****\n\n");
        }
        else if (grade=='B'){
            printf("Performance: ****\n\n");
        }
        else if (grade=='C'){
            printf("Performance: ***\n\n");
        }
        else if (grade=='D'){
            printf("Performance: **\n\n");
        }
        else{
            i++;
            printf("\n");
            continue;
        }
        i++;
    }
    return;
}

int main(){
    int n;
    struct Student student[100];
    printf("number of students: ");
    scanf("%d",&n);

    int i=0;
    while (i<n){
        printf("enter student roll no., name and marks for 3 subjects: ");
        scanf("%d",&student[i].rollno);
        scanf("%49s",student[i].name);
        scanf("%d",&student[i].marks[0]);
        scanf("%d",&student[i].marks[1]);
        scanf("%d",&student[i].marks[2]);
        i++;
    }
    readStudents(student, 0, n);
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbers(student, 0, n);

}