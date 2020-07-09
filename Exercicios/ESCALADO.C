#include<system.h>
#include<stdio.h>

PTR_DESC da,db,esc,prin;

void far corotinaA(){
     while(1){
         printf("A\n");
     }
}

void far corotinaB(){
     while(1){
         printf("B\n");
     }
}

void far escalador(){
     p_est->p_destino = da;
     p_est->p_origem = esc;
     p_est->num_vetor = 8;
     while(1){
         iotransfer();
         if(p_est->p_destino == da)
             p_est->p_destino = db;
         else
             p_est->p_destino = da;
     }
}

main(){
       da = cria_desc();
       db = cria_desc();
       esc = cria_desc();
       prin = cria_desc();
       newprocess(corotinaA,da);
       newprocess(corotinaB,db);
       newprocess(escalador,esc);
       transfer(prin,esc);
}