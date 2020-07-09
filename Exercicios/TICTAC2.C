#include<system.h>
#include<stdio.h>

PTR_DESC dtic,dtac,prin;

void far tic(){
     int i;
     for(i = 0; i < 50; i++){
           printf("tic-");
           transfer(dtic,dtac);
     }
}

void far tac(){
     int i;
     for(i = 0; i < 50; i++){
           printf("tac\n");
           transfer(dtac,dtic);
     }
     transfer(dtac,prin);
}

void far main(){
       clrscr();
       dtic = cria_desc();
       dtac = cria_desc();
       prin = cria_desc();
       newprocess(tic,dtic);
       newprocess(tac,dtac);
       newprocess(main,prin);
       transfer(prin,dtic);
}