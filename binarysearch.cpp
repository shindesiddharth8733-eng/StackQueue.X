#include <iostream>
using namespace std;
int binary_search(int a[], int n, int x)
{
    int first=0;
    int last=n-1;
    int mid;

    while(first<=last)
    {
        mid=(first+last)/2;
        if (a[mid]==x)
        {
            return mid;
        }
        else if (a[mid]<x)
        {
            first=mid+1;
        }
        else
        {
            last=mid-1;
        }
    }
    return -1;
}
int main()
{
    int n,x;
    int a[20];
    cout<<"Enter the size of the array:";
    cin>>n;
    cout<<"Enter"<<n<<"sorted integers:\n";
    for(int i=0; i<n;i++)
    {
        cin>>a[i];
    
    }
    cout<<"Enter the Target element to search for:";
    cin>>x;
    int position = binary_search(a,n,x);
    if (position!=1)
    cout<<"Element fount at position:"<<position<<endl;
    cout<<"Element not found."<<endl;
return 0;
}


//output
Enter the size of the array:5
Enter5sorted integers:
1
2

3
2
4
Enter the Target element to search for:1
Element fount at position:0
Element not found.
PS D:\siddharth247> 