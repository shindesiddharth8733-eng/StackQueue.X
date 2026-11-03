#include<iostream>
using namespace std;
class complex 
{
    private:
    float real;
    float imag;

    public:
    complex() 
    {
        real=0;
        imag=0;

    }
    complex(float r,float i) 
    {
        real=r;
        imag=i;
    }
    ~complex() 
    {
    } 
    void display(void) 
    {
      cout<<real<<"+"<<imag<<"i"<<endl;
    }
    complex operator+(complex c2)
    {
        float r = real+c2.real;
        float i = imag+c2.imag;
        return(complex(r,i));
    }
    complex operator-(complex c2) 
    {
        float r= real-c2.real;
        float i=imag-c2.imag;
        return(complex(r,i));
    }
    complex operator*(complex c2)
    {
        float r= real*c2.real - imag*c2.imag;
        float i= real*c2.real + imag*c2.imag;
        return(complex(r,i));
    }
};
int main() 
{
    complex c1(9,7);
    complex c2(8,5);

    cout<<"first complex number :";
    c1.display();
    cout<<"second complex number :";
    c2.display();

    complex sum = c1+c2;
    cout<<"addition is :";
    sum.display();

    complex diff = c1-c2;
    cout<<"substaction is :";
    diff.display();

    complex prod = c1*c2;
    cout<<"multiplication is :";
    prod.display();

    return(0);
}



//output
PS D:\siddharth 247> cd "d:\siddharth 247\" ; if ($?) { g++ practical1.cpp -o practical1 } ; if ($?) { .\practical1 }
first complex number :9+7i
second complex number :8+5i
addition is :17+12i
substaction is :1+2i
multiplication is :37+107i
PS D:\siddharth 247> 