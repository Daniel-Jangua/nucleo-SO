#include<nucleo.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>

#define N 100
#define M 1000

semaforo mutex;     /*exclusao mutua no acesso ao buffer*/
semaforo vazias;    /*numero de celulas vazias*/
semaforo cheias;    /*numero de celulas cheias*/

/*índices do produtor e consumidor para acessar o buffer*/
int index_p = 0, index_c = 0;

int buffer[N];

int produz(int p){
    return 2*p;
}

FILE *arq;

void deposita(int m){
    buffer[index_p] = m;
    fprintf(arq,"Produtor %s depositou: buffer[%d] = %d\n\n",nome_proc_atual(),index_p,m);
    /*se index_p == N -> index_p = 0*/
    index_p = (index_p+1)%N;
}

void far produtor(){
    int mensagem;
    int i;
    for(i = 0; i < M; i++){
        /*produz a mensagem*/
        mensagem = produz(index_p);
        /*verifica se existem celulas vazias*/
        P(&vazias);
        P(&mutex);
        /*Regiao critica - acesso ao buffer*/
        deposita(mensagem);
        V(&mutex);
        /*adiciona uma celula cheia*/
        V(&cheias);
    }
    terminar_processo();
}

int retira(){
    int m = buffer[index_c];
    fprintf(arq,"Consumidor %s retirou: buffer[%d] = %d\n\n",nome_proc_atual(),index_c,m);
    /*se index_c == N -> indexx_c = 0*/
    index_c = (index_c+1)%N;
    return m;
}

void consome(int m){
    FILE *arq2;
    /*anota no historico do consumidor (garçom) o pedido atendido*/
    char nome_arq[64];
    strcpy(nome_arq,nome_proc_atual());
    strcat(nome_arq,".txt"); 
    arq2 = fopen(nome_arq,"a+");
    if(!arq2){
        printf("Erro ao abrir arquivo %s\n",nome_arq);
        terminar_processo();
    }
    fprintf(arq2,"Pedido atendido: %d\n",m);
    fclose(arq2);
}

void far consumidor(){
    int mensagem;
    int i;
    for(i = 0; i < M; i++){
        /*verifica se existem celulas cheias*/
        P(&cheias);
        P(&mutex);
        /*Regiao crítica - acesso ao buffer*/
        mensagem = retira();
        V(&mutex);
        /*adiciona uma celula vazia*/
        V(&vazias);
        /*consome(mensagem);*/
    }
    terminar_processo();
}

void far main(){
    criar_processo("Prod1",produtor);
    criar_processo("Prod2",produtor);
    criar_processo("Cons1",consumidor);
    criar_processo("Cons2",consumidor);
    inicia_semaforo(&mutex,1);
    inicia_semaforo(&cheias,0);
    inicia_semaforo(&vazias,N);
    arq = fopen("prod_con.txt","w");
    if(!arq){
        printf("Erro ao abrir aquivo de saída.");
        exit(1);
    }
    dispara_sistema();
}
