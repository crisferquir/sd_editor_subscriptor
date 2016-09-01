/*
 Incluya en este fichero todas las implementaciones que pueden
 necesitar compartir todos los modulos (editor, subscriptor y
 proceso intermediario), si es que las hubiera.
 */

#include "comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


char * quitarCaracterLinea(char * cadena,int n){

  char*nuevo=NULL;
  nuevo=malloc(sizeof(char)*64);
  nuevo=strncpy(nuevo,cadena,n);
  char nulo='\0';
  strcat(nuevo,&nulo);
  return nuevo;
}
  
