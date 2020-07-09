
#include<nucleo.h>
#include<stdio.h>

void far processo1(){
    int i;
    for(i = 0; i < 10000; i++)
        printf("processo1");
    terminar_processo();
}

void far processo2(){
    int i;
    for(i = 0; i < 15000; i++)
        printf("processo2");
    terminar_processo();
}

void far processo3(){
    int i;
    for(i = 0; i < 8000; i++)
        printf("processo3");
    terminar_processo();
}

int main(){
    criar_processo("P1",processo1);
    criar_processo("P2",processo2);
    criar_processo("P3",processo3);
    dispara_sistema();
}