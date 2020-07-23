#include<stdio.h>
#include<system.h>
#include<stdlib.h>
#include<string.h>

/*Descritor dos processos*/
typedef struct desc_p{
    char nome[35];
    enum{ativo, bloq_p, terminado} estado;
    PTR_DESC contexto;
    struct desc_p *fila_sem;
    struct desc_p *prox_desc;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

typedef struct resgistros{
    unsigned bx1,es1;
}regis;

/*Estrutura que aponta se o processo esta na regiao critica do DOS*/
typedef union k{
    regis x;
    char far *y;
}APONTA_REG_CRIT;

APONTA_REG_CRIT a;

/*Definição do semaforo*/
typedef struct{
    int s;
    PTR_DESC_PROC Q;
}semaforo;

PTR_DESC_PROC prim = NULL;
PTR_DESC d_esc;

/*funcao para o usuario iniciar um semaforo*/
void far inicia_semaforo(semaforo *sem, int n){
    sem->s = n;
    sem->Q = NULL;
}

/*funcao que retorna o nome do processo corrente*/
char* far nome_proc_atual(){
    char nomep[35] = "";
    if(prim == NULL)
        return nomep;
    strcpy(nomep,prim->nome);
    return nomep; 
}

/*funcao para criar um processo a partir de uma co-rotina*/
void far criar_processo(char nomep[35], void far (*proc)()){
    PTR_DESC_PROC p_aux;
    p_aux = (PTR_DESC_PROC)malloc(sizeof(struct desc_p));       /*Aloca a estrutura do descritor de processo*/
    strcpy(p_aux->nome,nomep);                                  /*Atribui nome ao processo*/
    p_aux->estado = ativo;                                      /*Define estado como ativo*/
    p_aux->fila_sem = NULL;                                     
    p_aux->contexto = cria_desc();                              /*Cria a estrutura que guarda o contexto do processo*/
    newprocess(proc,p_aux->contexto);                           /*Associa a estrutura ao processo*/
    /*Inserirndo o novo processo no final da fila dos processos correntes*/
    if(prim == NULL){
        prim = p_aux;
    }else{
        PTR_DESC_PROC p = prim;
        while(p->prox_desc != prim)
            p = p->prox_desc;
        p->prox_desc = p_aux;
    }
    p_aux->prox_desc = prim;
}

/*Funcao que muda o estado do processo para "terminado"*/
void far terminar_processo(){
    disable();
    prim->estado = terminado;
    enable();
    while(1);
}

/*Funcao que restaura o vetor de interrupcoes devolvendo o controle ao DOS*/
void far volta_dos(){
    disable();
    setvect(8,p_est->int_anterior);
    enable();
    exit(0);
}

/*Funcao que retorna o proximo processo ativo para ser escalado*/
PTR_DESC_PROC far procura_prox_ativo(){
    PTR_DESC_PROC p = prim;
    while((p = p->prox_desc) != prim){          /*Percorre toda a fila circular em busca do primeiro processo ativo*/
        if(p->estado == ativo)
            return p;
    }
    if(p->estado == ativo)                      /*Se percorreu todos e voltou a cabeca da fila-> se este processe esta ativo ele e escalado*/
        return p;               
    return NULL;                                /*Caso contrario, nenhum processo ativo para rodar, retorna NULL*/
}

/*Primitiva P do mecanismo de semaforo*/
void far P(semaforo *sem){
    PTR_DESC_PROC p_aux;
    disable();
    if(sem->s > 0)
        sem->s--;
    else{
        prim->estado = bloq_p;
        p_aux = sem->Q;
        /*se a fila estiver vazia*/
        if(p_aux == NULL)
            sem->Q = prim;
        else{
            /*posiciona o ponteiro no ultimo elemento da fila*/
            while(p_aux->fila_sem != NULL)
                p_aux = p_aux->fila_sem;
            /*insere o prim no final da fila Q*/
            p_aux->fila_sem = prim;
        }
        p_aux = prim;
        /*encontra o proximo processo ativo para rodar*/
        if((prim = procura_prox_ativo()) == NULL){
            /*todos bloqueados/terminados - deadlock*/
            volta_dos();
        }
        /*passa o controle para o proximo processo rodar (enable() implícito)*/
        transfer(p_aux->contexto,prim->contexto);
    }
    enable();
}

/*Primitiva V do mecanismo de semaforo*/
void far V(semaforo *sem){
    PTR_DESC_PROC p_aux;
    disable();
    /*Se a fila não está vazia, retira o primeiro da fila*/
    if(sem->Q != NULL){
        sem->Q->estado = ativo;
        p_aux = sem->Q;
        /*retirando o primeiro da fila*/
        sem->Q = sem->Q->fila_sem;
        p_aux->fila_sem = NULL;
    }else{
        /*A fila está vazia*/
        sem->s++;
    }
    enable();
}

/*Funcao do escalador, que implementa o algoritmo Round Robin*/
void far escalador(){
    /*Cofigura a estrutura global que representa a passagem de parametro da funcao iotransfer*/
    p_est->p_origem = d_esc;                        
    p_est->p_destino = prim->contexto;
    p_est->num_vetor = 8;
    /*Preparando para verificacao se o processo esta na regiao critica do DOS*/
    _AH=0x34;
    _AL=0x00;
    geninterrupt(0x21);
    a.x.bx1 = _BX;
    a.x.es1 = _ES;
    while(1){
        iotransfer();                                       /*Transfere o controle para o processo escalado*/
        disable();
        if(!*a.y){                                          /*Se o processoa nao esta ma regiao critica, escala outro*/
            if((prim = procura_prox_ativo()) == NULL)       /*Caso contrario, o mesmo ganha mais uma fatia de tempo*/
                volta_dos();                                /*Caso nao esxistam mais processos ativos, reotorna o controle ao DOS*/
            p_est->p_destino = prim->contexto;
        }                                   
        enable();
    }
}

/*Funcao para o usuario colocar os processos para rodar*/
void far dispara_sistema(){
    PTR_DESC desc_dispara;              
    d_esc = cria_desc();                                    /*Cria contexto para o escalador*/
    desc_dispara = cria_desc();                             /*Cria contexto para a funcao dispara*/
    newprocess(escalador,d_esc);                            /*Associa o escalador ao seu descritor*/
    transfer(desc_dispara,d_esc);                           /*Transfere o controle para o escalador*/
}

