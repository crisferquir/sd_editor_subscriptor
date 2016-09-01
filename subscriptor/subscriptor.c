#include "subscriptor.h"
#include "comun.h"
#include "edsu_comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <malloc.h>
#include <pthread.h>
#include <signal.h>

static int inicioHecho = 0;
static int socketUtilizado=0;
static pthread_t threadPID = 0;
static void (*notificar)(const char*, const char*);
static int port;

void* escucharEventos(void* socket);

int fin_subscriptor() {

  close(socketUtilizado);
  inicioHecho=0;
  socketUtilizado = 0;
  threadPID = 0;
  return 0;
}

int alta_subscripcion_tema(const char *tema) {
        if(inicioHecho==0){
	  return -1;
        }
	int s;

	if ((s = conectarIntermediario()) < 0) {
		return s;
	}

        mensaje_evento mens;
        strcpy(mens.codigo,"alta");
        strcpy(mens.evento,tema);
        mens.puerto=port;

       if (write(s, &mens, sizeof(mensaje_evento)) < 0) {
		perror("error en write");
		close(s);
		return -1;
	}
	shutdown(s, SHUT_WR);
	int leido = 0;
	mensaje_evento mensResp;
	if ((leido = read(s, &mensResp,  sizeof(mensaje_evento))) < 0) {
		perror("Error en read de respuesta");
	}
	printf("333");
	if(strcmp(mensResp.codigo,"error")==0){
	 return -1;
        }
	close(s);
	return 0;
  
}

int baja_subscripcion_tema(const char *tema) {
  int s;
         

       if(inicioHecho==0){
	 return -1;
       }
       inicioHecho=1;
	if ((s = conectarIntermediario()) < 0) {
		return s;
	}
         
         mensaje_evento mens;
         strcpy(mens.codigo,"baja");
         strcpy(mens.evento,tema);
         mens.puerto=port;
  
        if (write(s, &mens, sizeof(mensaje_evento)) < 0) {
		perror("error en write");
		close(s);      
		return -1;
	}
	shutdown(s, SHUT_WR);
	int leido = 0;
	mensaje_evento mensResp;
	if ((leido = read(s, &mensResp,  sizeof(mensaje_evento))) < 0) {
		perror("Error en read de respuesta");
	}

	if(strcmp(mensResp.codigo,"error")==0){
	 return -1;
         }
        close(s);
	return 0;
}

int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
		void (*alta_tema)(const char *), void (*baja_tema)(const char *)) {
  printf("inicio\n");
	if (inicioHecho!= 0)
		return -1;

	notificar = notif_evento;
	inicioHecho=1;       
	int s;
	struct sockaddr_in dir;
	int opcion = 1;
  
         printf("creamos el socket\n");

	if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return 1;
	}

	/* Para reutilizar puerto inmediatamente
	 */
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0) {
		perror("error en setsockopt");
		return 1;
	}

	dir.sin_addr.s_addr = INADDR_ANY;
	dir.sin_port = 0;
	dir.sin_family = PF_INET;
  
        printf("hacemos el bind\n");
	if (bind(s, (struct sockaddr *) &dir, sizeof(dir)) < 0) {
		perror("error en bind");
		close(s);
		return 1;
	}

	if (listen(s, 60) < 0) {
		perror("error en listen");
		close(s);
		return 1;
	}

        printf("despues de listen\n");
	struct sockaddr_in aux;
	int tam = sizeof(aux);
	getsockname(s, (void*) &aux, (socklen_t *) &tam);
        port=ntohs(aux.sin_port);
        

         printf("creamos el thread que escucha eventos\n");
	pthread_create(&threadPID, NULL, escucharEventos, (void*) s);

	return 0;
}

void* escucharEventos(void* socket) {
         printf("thread escuchar eventos\n");
    
	int s_conec, leido;
        int socketN= (int) socket;
	struct sockaddr_in dir_cliente;
         printf("antes de accept del thread que escucha\n");
        
	while (1) {
          unsigned int tam=sizeof(dir_cliente);
	 
	  if ((s_conec = accept(socketN, (struct sockaddr *) &dir_cliente,&tam)) < 0) {
			perror("error en accept");
			close(socketN);
			pthread_exit(NULL);

			return NULL ;
		}

         printf("despues de accept del thread que escucha\n");
         mensaje_evento mens;
         if((leido = read(s_conec, &mens, sizeof(mens))) <0) {
            perror("error en el recv");
		}
	 int car;
         car=strcspn(mens.evento,"\n");
         char *search = "\n";
         char *tema = strtok(mens.evento,search);
         printf("Palabra de strtol %s\n",tema);
 	 strncat(mens.evento,"\0",(car-1));
 
           fprintf(stdout,"codigo respuesta recibido %s: ", mens.codigo);

			(*notificar)(mens.evento, mens.texto);

	
           printf("leemos el evento \n");
		if (leido < 0) {
			perror("error en read");
			close(socketN);
			close(s_conec);
			pthread_exit(NULL );
			return NULL ;
		}
    
        printf("antes del close \n");
		close(s_conec);
	}

	close(socketN);
	pthread_exit(NULL );
	return NULL ;
}

