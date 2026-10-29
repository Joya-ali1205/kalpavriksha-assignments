#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void print_matrix(int **mtrx,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%d ",*(*(mtrx+i)+j));
        }
        printf("\n");
    }
}

void generate_matrix(int **mtrx,int n){
    //generating n*n matrix
    int x;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            x=rand()%256;
            *(*(mtrx+i)+j)=x;
        }
    }
    //displaying matrix
    printf("Original:\n");
    print_matrix(mtrx,n);
}

void rotate_matrix(int **mtrx,int n){
    //transposing the matrix first
    int temp;
    for(int i=0;i<n;i++){
        for(int j=i+1;j<n;j++){
            temp=*(*(mtrx+i)+j);
            *(*(mtrx+i)+j)=*(*(mtrx+j)+i);
            *(*(mtrx+j)+i)=temp;
        }
    }
    //now reversing the transposed matrix
    for(int i=0;i<n;i++){
        int left=0;
        int right=n-1;
        while(left<right){
            temp=*(*(mtrx+i)+left);
            *(*(mtrx+i)+left)=*(*(mtrx+i)+right);
            *(*(mtrx+i)+right)=temp;
            left++;
            right--;
        }
    }
    //displaying rotated matrix
    printf("Rotated:\n");
    print_matrix(mtrx,n);
}



void smoothing_filter(int **mtrx, int n) {
    int *prev = calloc(n, sizeof(int));
    int *curr = calloc(n, sizeof(int));
    int *next = calloc(n, sizeof(int));

    if (prev == NULL || curr == NULL || next == NULL) {
        free(prev);
        free(curr);
        free(next);
        printf("Memory allocation failed\n");
        return;
    }

    // Store the original first row
    for (int j = 0; j < n; j++) {
        curr[j] = *(*(mtrx) + j);
    }

    // Store the original second row
    if (n > 1) {
        for (int j = 0; j < n; j++) {
            next[j] = *(*(mtrx + 1) + j);
        }
    }

    for (int i = 0; i < n; i++) {
        // Calculate averages using neighboring rows
        for (int j = 0; j < n; j++) {
            int sum = 0;
            int count = 0;

            for (int r = i - 1; r <= i + 1; r++) {
                for (int c = j - 1; c <= j + 1; c++) {
                    if (r >= 0 && r < n &&
                        c >= 0 && c < n) {

                        if (r == i - 1) {
                            sum += prev[c];
                        } else if (r == i) {
                            sum += curr[c];
                        } else {
                            sum += next[c];
                        }

                        count++;
                    }
                }
            }

            *(*(mtrx + i) + j) = sum / count;
        }

        // Shift the row buffers
        int *temp = prev;
        prev = curr;
        curr = next;
        next = temp;

        // Load the next original row before it is modified
        if (i + 2 < n) {
            for (int j = 0; j < n; j++) {
                next[j] = *(*(mtrx + i + 2) + j);
            }
        }
    }

    free(prev);
    free(curr);
    free(next);

    //display smoothed matrix
    printf("Final Output:\n");
    print_matrix(mtrx, n);
}




int main(){
    int n;
    scanf("%d",&n);
    if (n < 2 || n > 10) {
        printf("Invalid matrix size\n");
        return 1;
    }   
    int **mtrx=calloc(n,sizeof(int *));
    for(int i=0;i<n;i++){
        mtrx[i]=calloc(n,sizeof(int));
    }
    //calling generate matrix fxn
    srand(time(NULL));
    generate_matrix(mtrx,n);
    //calling rotate matrix fxn
    rotate_matrix(mtrx,n);
    //calling smoothing fxn
    smoothing_filter(mtrx,n);

    //free the memory
    for(int i=0;i<n;i++){
        free(*(mtrx+i));
    }
    free(mtrx);

}