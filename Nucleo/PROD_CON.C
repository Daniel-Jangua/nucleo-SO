#include<nucleo.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>

/*O produtor funciona como um gracom de um restaurante que anota um pedido (produz()) e coloca
em uma fila de pedidos (buffer circular) a serem preparados (retira()) pelo cozinheiro (consumidor)
na ordem em que chegam*/

#define N 100           /*Tamanhho do buffer*/
#define M 1000          /*Numero de iteracoes do produtor/consumidor*/
#define MAX_CHAR 32     /*Tamanho maximo do nome dos pratos*/
#define QTD_PRATOS 20   /*Quantidade de pratos no cardapio*/

semaforo mutex;     /*exclusao mutua no acesso ao buffer*/
semaforo vazias;    /*numero de celulas vazias*/
semaforo cheias;    /*numero de celulas cheias*/

/*indices do produtor e consumidor para acessar o buffer*/
int index_p = 0, index_c = 0;

char buffer[N][MAX_CHAR];

FILE *arq;

char pratos[QTD_PRATOS][MAX_CHAR] = {"Canelone de Espinafre", "Tagliatelle", "Pizza de Marguerita", 
"Pizza La Pasta", "Pizza de Pepperoni", "Raviole de Pato", "Bruschetta", "Tiramisu", "Polpette", "Lasagna", "Nhoque",
"Salada Caprese", "Aracini de Parmesao", "Bife a Parmegiana", "Antepasto de Berinjela", "Carpaccio", "Focaccia de Calabresa",
"Strata Italiana", "Macarrao a Bolonhesa", "Biscotti Italiano"};

char* far produz(){
    /*gera um numero aleatorio relativo a um item do cardapio, representando um pedido*/
    int prato;
    prato = rand() % QTD_PRATOS;      /*gera um numero aleatorio entre 0 e QTD_PRATOS*/
    return pratos[prato];               /*retorna o nome do prato*/
}

void far deposita(char m[MAX_CHAR]){
    strcpy(buffer[index_p],m);
    fprintf(arq,"Garcom %s anotou pedido: \"%s\" para a mesa %d (depositou - buffer[%d] = \"%s\")\n\n",nome_proc_atual(),m,index_p,index_p,m);
    /*se index_p == N -> index_p = 0*/
    index_p = (index_p+1)%N;
}

void far produtor(){
    char mensagem[MAX_CHAR];
    int i;
    for(i = 0; i < M; i++){
        /*produz a mensagem*/
        strcpy(mensagem,produz());
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

char* far retira(){
    char m[MAX_CHAR];
    strcpy(m,buffer[index_c]);
    fprintf(arq,"Cozinheiro %s preparou o pedido: \"%s\" para a mesa %d (retirou - buffer[%d] = \"%s\")\n\n",nome_proc_atual(),m,index_c,index_c,m);
    /*se index_c == N -> indexx_c = 0*/
    index_c = (index_c+1)%N;
    return m;
}

void far consumidor(){
    char mensagem[MAX_CHAR];
    int i;
    for(i = 0; i < M; i++){
        /*verifica se existem celulas cheias*/
        P(&cheias);
        P(&mutex);
        /*Regiao critica - acesso ao buffer*/
        strcpy(mensagem,retira());
        V(&mutex);
        /*adiciona uma celula vazia*/
        V(&vazias);
    }
    terminar_processo();
}

void far main(){
    srand(time(NULL));
    criar_processo("Daniel",produtor);
    criar_processo("Lucas",produtor);
    criar_processo("Joao",consumidor);
    criar_processo("Lucca",consumidor);
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