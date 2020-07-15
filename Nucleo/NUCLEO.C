#include<stdio.h>
#include<system.h>
#include<stdlib.h>
#include<string.h>

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

typedef union k{
    regis x;
    char far *y;
}APONTA_REG_CRIT;

APONTA_REG_CRIT a;

typedef struct{
    int s;
    PTR_DESC_PROC Q;
}semaforo;

PTR_DESC_PROC prim = NULL;
PTR_DESC d_esc;

void far inicia_semaforo(semaforo *sem, int n){
    sem->s = n;
    sem->Q = NULL;
}

void far criar_processo(char nomep[35], void far (*proc)()){
    PTR_DESC_PROC p_aux;
    p_aux = (PTR_DESC_PROC)malloc(sizeof(struct desc_p));
    strcpy(p_aux->nome,nomep);
    p_aux->estado = ativo;
    p_aux->fila_sem = NULL;
    p_aux->contexto = cria_desc();
    newprocess(proc,p_aux->contexto);

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

void far terminar_processo(){
    disable();
    prim->estado = terminado;
    enable();
    while(1);
}

void far volta_dos(){
    disable();
    setvect(8,p_est->int_anterior);
    enable();
    exit(0);
}

PTR_DESC_PROC far procura_prox_ativo(){
    PTR_DESC_PROC p = prim;
    while((p = p->prox_desc) != prim){
        if(p->estado == ativo)
            return p;
    }
    if(p->estado == ativo)
        return p;
    return NULL;
}

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

void far escalador(){
    p_est->p_origem = d_esc;
    p_est->p_destino = prim->contexto;
    p_est->num_vetor = 8;
    _AH=0x34;
    _AL=0x00;
    geninterrupt(0x21);
    a.x.bx1 = _BX;
    a.x.es1 = _ES;
    while(1){
        iotransfer();
        disable();
        if(!*a.y){
            if((prim = procura_prox_ativo()) == NULL)
                volta_dos();
            p_est->p_destino = prim->contexto;
        }
        enable();
    }
}

void far dispara_sistema(){
    PTR_DESC desc_dispara;
    d_esc = cria_desc();
    desc_dispara = cria_desc();
    newprocess(escalador,d_esc);
    transfer(desc_dispara,d_esc);
}

