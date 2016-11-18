#include <stdio.h>
#include <stdlib.h>




void quickSort(int arr[], int left, int right) {
    int i = left, j = right;
    int tmp;
    int pivot = arr[(left + right) / 2];
    
    /* partition */
    while (i <= j) {
        while (arr[i] < pivot)
            i++;
        while (arr[j] > pivot)
            j--;
        if (i <= j) {
            tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
            i++;
            j--;
        }
    };
    
    /* recursion */
    if (left < j)
        quickSort(arr, left, j);
    if (i < right)
        quickSort(arr, i, right);
}


int main(int argc, char * argv[])
{
    int n = 10; 
    int *list;


    scanf("%d", &n);
    list = (int*)malloc(sizeof(int)*n);
    for(int i = 0; i < n; i++){
        scanf("%d", &list[i]);
	
    }

    quickSort(list, 0, n - 1);

    for(int i = 0; i < n; i++){
        printf("%d ", list[i]);
    }
    printf("\n");
    return 0;
}
