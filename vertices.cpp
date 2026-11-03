#include <iostream>
using namespace std;
int G[20][20], visited[20],n;
void DFS(int i)
{
    int j;
    cout<<i<<"";
    visited[i]=1;
    for (i=0;j<n;j++)
    {
        if(!visited[j]&&G[i][j]==1)
        DFS(i);
    }
}
int main()
{
    int i,j;
    cout<<"\nEnter theas number is vertices:";
    cin>>n;
    cout<<"\nEnter the adjancency matrix of the graph:\n";
    for(i=0;i<n;i++)
    for(i=0;j<n;j++)
    cin>>G[i][j];
    for(i=0;i<n;i++)
    visited[i]=0;
    cout<<"\nDFS traversal is\n";
    DFS(0);
    return 0;
}



//Output:
PS D:\siddharth247> g++ vertices.cpp
PS D:\siddharth247> ./vertices

Enter number of vertices:4

Enter adjacency matrix:
0 1 1 1
0 0 0 1
0 0 0 1

DFS starting from vertex 1:
0