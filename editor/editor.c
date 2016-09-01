#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int generar_evento(const char *tema, const char *valor) {
        int s;
        if((s=conectarIntermediario())<0){
          return s;
        }

        mensaje_evento mensaje;
        strcpy(mensaje.codigo,"evento");
        strcpy(mensaje.evento,tema);
        strcpy(mensaje.texto,valor);

        if(write(s,&mensaje,sizeof(mensaje))<0){
          perror("error en el write de editor");
          close(s);
          return -1;
        }
        shutdown(s, SHUT_WR);
	printf("despues de escribir\n");
        int leido=0;
  
        if((leido=read(s,&mensaje,sizeof(mensaje)))<0){
	  perror("Error en read de la respuesta");
        }
         
        close(s);

        if(strcmp(mensaje.codigo,"error")==0){
	  return -1;
          }


	return 0;
}
/* solo para la version avanzada */
int crear_tema(const char *tema) {
	return 0;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	return 0;
}

	/*	char* mensaje=NULL;
        int mensLong=32;
        int copiado=0;
        char*codigo=malloc(sizeof(char)*32);
        strcpy(codigo,"EVENTO");
        
       
	char * separacion="$"; 
         char * fin="\0"; 
	 printf("despues de conect\n"); 
         int copiar=0; 
        
	 mensaje=malloc(sizeof(char)*strlen(codigo)); 
         bzero(mensaje,sizeof(mensaje)); 
         copiar=strlen(codigo); 
         memcpy((void*)mensaje,(void*)codigo,copiar); 
         printf("el mensaje tiene codigo, %d  %s\n ",copiar,mensaje);
	
        copiado+=sizeof(char)*strlen(codigo);
        copiar=sizeof(char)*strlen(fin);
        mensaje=realloc(mensaje,copiado+copiar);
        memcpy((void*)(mensaje+copiado),(void*)fin,copiar);

	
        copiado+=sizeof(char)*strlen(fin);
        copiar=sizeof(char)*strlen(separacion);
        mensaje=realloc(mensaje,copiado+copiar);
        memcpy((void*)(mensaje+copiado),(void*)separacion,copiar);
 
        copiado+=sizeof(char)*strlen(separacion);
        copiar=sizeof(char)*strlen(tema);
	mensaje=realloc(mensaje,copiado+copiar);
	memcpy((void*)(mensaje+copiado),(void*)tema,copiar);
       	printf("el mensaje tiene tema %s\n",mensaje);   

        copiado+=sizeof(char)*strlen(tema);
        copiar=sizeof(char)*strlen(separacion);
       	mensaje=realloc(mensaje,copiado+copiar);
	memcpy((void*)(mensaje+copiado),(void*)separacion,copiar);
         
        copiado+=sizeof(char)*strlen(separacion);
        copiar=sizeof(char)*strlen(valor);
       	mensaje=realloc(mensaje,copiado+copiar);
	memcpy((void*)(mensaje+copiado),(void*)valor,copiar);
	
	copiado+=sizeof(char)*strlen(valor);
        copiar=sizeof(char)*strlen(separacion);
       	mensaje=realloc(mensaje,copiado+copiar);
	memcpy((void*)(mensaje+copiado),(void*)separacion,copiar);
	

        copiado+=sizeof(char)*strlen(separacion);
        copiar=sizeof(char)*strlen(fin);
       	mensaje=realloc(mensaje,copiado+copiar);
	memcpy((void*)(mensaje+copiado),(void*)fin,copiar);       
	printf("el mensaje tiene el texto %s\n",mensaje); 



        if(write(s,mensaje,copiado+copiar)<0){
          perror("error en el write de editor");
          close(s);
          return -1;
        }
        shutdown(s, SHUT_WR);
	printf("despues de escribir\n");
        int leido=0;
        char buf[copiado];
        if((leido=read(s,buf,copiado))<0){
	  perror("Error en read de la respuesta");
        }
	free(mensaje);      
        close(s);

	*/
