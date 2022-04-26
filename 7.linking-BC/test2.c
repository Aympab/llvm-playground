#include <stdio.h>
extern int func(int a);

int main(){
    int num = 5;
    num = func(num);
    printf("Number is : %d", num);
    return num;
}