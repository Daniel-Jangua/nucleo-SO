#include<stdio.h>
#include<system.h>
#include<stdlib.h>
#include<string.h>

/*
typedef struct desc_p{
    char nome[35];
    enum{ativo, terminado} estado;
    PTR_DESC contexto;
    struct desc_p *prox_desc;
}DESCRITOR_PROC;

typedef DESCRITOR_PROC *PTR_DESC_PROC;

extern PTR_DESC_PROC prim;
extern PTR_DESC d_esc;*/

extern void far criar_processo(char nomep[35], void far (*proc)());
extern void far terminar_processo();
extern void far dispara_sistema();
extern void far inicia_semaforo(semaforo *sem, int n);

