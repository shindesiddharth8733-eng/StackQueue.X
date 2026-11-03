#include <iostream>
using namespace std;

void bubble_sort(int a[], int n) 
{
    for (int i = 0; i < n - 1; i++) 
    {
    for (int j = 0; j < n - 1; j++)
    {
        if (a[j] > a[j + 1]) 
        {
        int temp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = temp;
        }
    }
    }
}

int main() {
    int n;
    int a[10]; 
    cout << "Enter the size of the array: ";
    cin >> n;
    cout << "Enter " << n << " integers in any order: " << endl;
    for (int i = 0; i < n; i++) 
    {
        cin >> a[i];
    }

    bubble_sort(a, n);

    cout << "Sorted array: ";
    for (int i = 0; i < n; i++) 
    {
        cout << a[i] << " ";
    }
    cout << endl;

    return 0;
}


//output
PS D:\siddharth247> ./bubblesort
Enter the size of the array: 10
Enter 10 integers in any order:
1
2
3
4
56
67
78
79
90
87
Sorted array: 1 2 3 4 56 67 78 79 87 90
PS D:\siddharth247> 