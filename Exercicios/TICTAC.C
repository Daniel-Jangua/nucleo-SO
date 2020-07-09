#include<system.h>
#include<stdio.h>

PTR_DESC dtic,dtac,dmain;

void far tic(){
     while(1){
         printf("tic-");
         transfer(dtic,dtac);
     }
}

void far tac(){
     while(1){
         printf("tac\n");
         transfer(dtac,dtic);
     }
}

main(){
       clrscr();
       dtic = cria_desc();
       dtac = cria_desc();
       dmain = cria_desc();
       newprocess(tic,dtic);
       newprocess(tac,dtac);
       /*newprocess(main,dmain);*/
       transfer(dmain,dtic);
}