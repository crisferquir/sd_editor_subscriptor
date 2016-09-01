#include <sys/socket.h>
#include <netinet/in.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "comun.h"




typedef struct nodo {
		char *nombre;
                int numeroSubscritos;
                struct ListSubscritos *subscritos;
		struct nodo *siguiente;
}nodoLista;


typedef struct ListaTemas {
                int numeroTemas;
		nodoLista *inicio;
		nodoLista *fin;
	    
}ListaTemas;


typedef struct nodoSubscrito {
                struct sockaddr_in * dirSubscrito;
		struct nodoSubscrito *siguiente;
}nodoSubscrito;

typedef struct ListSubscritos{
    	        nodoSubscrito *inicio;
		nodoSubscrito *fin;
}ListSubscritos;

static char * temaSave;
void imprimir_subs (ListSubscritos *lista);
void imprimir_temas (ListaTemas *lista);
int inser_lista_vacia (ListaTemas* lista, char *dato);
int ins_fin_lista (ListaTemas* lista, nodoLista * actual, char *dato);
int  ins_fin_listaSubs(ListSubscritos* lista, struct sockaddr_in * dirSubscrito);
int sup_en_listSubs(ListSubscritos* lista,int pos);
ListSubscritos *  inser_lista_vaciaSubs(ListSubscritos* lista, struct sockaddr_in * dirSubscrito);
int sup_inicio (ListSubscritos * lista,int nsubscritos);
int ins_lista (ListaTemas *lista, char *dato,int pos);
ListaTemas* inicializaListTemas(FILE * fp);
nodoLista* buscarTema(char* tema, ListaTemas* listaTemas);
int realizarNotificacion(mensaje_evento* mens,ListaTemas* listaTemas);
int realizarBaja(mensaje_evento * mensaje,ListaTemas * listaTemas);
int realizarAlta(mensaje_evento * mensaje,ListaTemas * listaTemas);
int realizarEvento(mensaje_evento* mensaje,ListaTemas * listaTemas);
void enviarPeticion( mensaje_evento* mens,struct sockaddr_in * dirCliente); 
int sonIguales(  struct sockaddr_in * sock1,   struct sockaddr_in * sock2);

/* visualización de la lista */
void imprimir_temas (ListaTemas * lista){
  nodoLista *actual;
  actual = lista->inicio;
  while (actual != NULL){
    printf ("%p - %s- nSubscritos:%d\n", actual, actual->nombre,actual->numeroSubscritos);
      actual = actual->siguiente;
  }
}


/* visualización de la lista */
void imprimir_subs (ListSubscritos * lista){
  nodoSubscrito *actual;
  actual = lista->inicio;
  int puerto;
  while (actual != NULL){
    puerto=htons(actual->dirSubscrito->sin_port);
    printf ("%p puerto %d\n", actual, puerto);
      actual = actual->siguiente;
  }
}

void inicializacion (ListaTemas *lista){
		lista->inicio = NULL;
		lista->fin = NULL;
		lista->numeroTemas= 0;
}

/* inserción en una lista vacía */
ListSubscritos* inser_lista_vaciaSubs(ListSubscritos* lista, struct sockaddr_in * dirSubscrito){
    nodoSubscrito* nuevo_nodo;
  printf(" inser lista vaciaSubs\n");
  if ((nuevo_nodo = (nodoSubscrito *) malloc (sizeof (nodoSubscrito))) == NULL)
        return NULL;
  
  
     memcpy((void*)nuevo_nodo->dirSubscrito,(void*)dirSubscrito,sizeof(struct sockaddr_in));
     printf("inser 2\n");
     nuevo_nodo->siguiente = NULL;
     lista->inicio= nuevo_nodo;
     lista->fin=nuevo_nodo;
     printf("inser 3\n");
  return lista;
}


/* inserción en una lista vacía */
int inser_lista_vacia  (ListaTemas * lista, char *dato){
  nodoLista *nuevo_nodo;
  if ((nuevo_nodo = (nodoLista *) malloc (sizeof (nodoLista))) == NULL)
        return -1;
   if ((nuevo_nodo->nombre = (char *) malloc (64 * sizeof (char)))== NULL)
     return -1;
  strcpy (nuevo_nodo->nombre, dato);
  nuevo_nodo->numeroSubscritos=0;
   
  nuevo_nodo->siguiente = NULL;
  lista->inicio = nuevo_nodo;
  lista->fin = nuevo_nodo;
  lista->numeroTemas++;
  return 0;
}
 /*inserción al final de la lista */
int ins_fin_lista (ListaTemas * lista, nodoLista * actual, char *dato){
  nodoLista* nuevo_nodo;
  if ((nuevo_nodo = (nodoLista *) malloc (sizeof (nodoLista))) == NULL)
    return -1;
    if ((nuevo_nodo->nombre = (char *) malloc (65 * sizeof (char)))== NULL)
     return -1;
  strcpy (nuevo_nodo->nombre, dato);
  nuevo_nodo->numeroSubscritos=0;
  actual->siguiente = nuevo_nodo;
  nuevo_nodo->siguiente = NULL;
  lista->fin =  nuevo_nodo;
  lista->numeroTemas++;
  return 0;
}
/* eliminación al inicio de la lista subscritos */
int sup_inicio (ListSubscritos * lista,int nsubscritos){

  nodoSubscrito  *sup_elemento;
  sup_elemento = lista->inicio;
  lista->inicio = lista->inicio->siguiente;
  if (nsubscritos == 1)
    lista->fin = NULL;
 
  //free(sup_elemento->dirSubscrito);
    printf("1"); 
  free(sup_elemento);
  return 0;
}



 /*inserción al final de la lista subscritos*/
int ins_fin_listaSubs(ListSubscritos* lista, struct sockaddr_in * dirSubscrito){
  nodoSubscrito* nuevo_nodo;
  printf("1 inser fin lista subs\n");
  if ((nuevo_nodo = (nodoSubscrito *) malloc (sizeof (nodoSubscrito))) == NULL)
    return -1;
  
  printf("2 inser fin lista subs\n");
  memcpy((void*)nuevo_nodo->dirSubscrito,(void*)dirSubscrito,sizeof(struct sockaddr_in));
  printf("fin inser fin lista subs \n");
  (lista->fin)->siguiente=nuevo_nodo;
  nuevo_nodo->siguiente = NULL;
  lista->fin =  nuevo_nodo;
  printf("2");
  return 0;
}

ListaTemas* inicializaListTemas(FILE * fp) {
  
  ListaTemas *temas=( ListaTemas*)malloc(sizeof( ListaTemas));
	  temas->numeroTemas=0;

          size_t leidos = 0;
          size_t lineaMaxima = 64;  
        	char *buffer = (char *)malloc(lineaMaxima);
		
		           
             while((leidos = getline(&buffer,&leidos,fp)) != -1){
	       //printf("buffer valor: %s\n",buffer);
	      
		  char* cadena= quitarCaracterLinea(buffer,(int)leidos);
		  // printf("cadena valor: %s\n",cadena);
              if( temas->numeroTemas==0) {
                    
		if(inser_lista_vacia(temas,cadena)!=0){
                     printf("error al insertar en lista vacia");
                    }
              }
              else{
                    if(ins_fin_lista (temas, temas->fin, cadena)!=0){
                       printf("error al insertar en la ultima posicion");
		       }
              }  
          }
		
	fclose(fp);
	return temas;
}


int  sup_en_listaSubs(ListSubscritos *lista, int pos){
  //   if(lista->numeroSubscritos<=1||pos<1||pos>=lista->numeroSubscritos){
  // return -1;
  // }
   int i;
   nodoSubscrito *actual;
   nodoSubscrito *supElement;
   actual=lista->inicio;

   for(i=1;i<pos;++i)
     actual=actual->siguiente;
   printf("supElement\n");
   supElement=actual->siguiente;
   actual->siguiente=actual->siguiente->siguiente;
   if(actual->siguiente==NULL){
     lista->fin=actual; 
   }
   printf("free supElement\n");
   // free(supElement->dirSubscrito);
   free(supElement);
   // lista->numeroSubscritos--;
   return 0;
}


int realizarBaja(mensaje_evento* mensaje,ListaTemas * listaTemas) {
          	
        nodoLista * temaBuscado;
        //temaBuscado = buscarTema(mensaje->evento, listaTemas);
         nodoLista *actual;
         actual = listaTemas->inicio;
         printf("BUSCOOOOOOO  %s \n",mensaje->evento);
         int encontrado=0;
         while (actual != NULL&&encontrado==0){
           printf("TENGOOOOOOOO %s  \n",actual->nombre);
	   if (strcmp(actual->nombre, mensaje->evento) == 0) {
			fprintf(stdout,"buscarTema: hemos encontrado el tema: %s\n",actual->nombre);
                         temaBuscado= actual;
                         encontrado=1;
                         }
           actual = actual->siguiente;
          }
         if(!encontrado){
	   printf("No encontrado para dar de baja\n");
          return -1;  
         }

	 int encontrado2=0;
         printf("Vemos si es vacia\n");      
         int i;
           if(temaBuscado->numeroSubscritos==0){ //Es vacia
               temaBuscado->subscritos=( ListSubscritos*)malloc(sizeof( ListSubscritos));
               bzero((void*)temaBuscado->subscritos,sizeof(ListSubscritos)); 
	   }
	   else{
	     temaBuscado->subscritos=realloc(temaBuscado->subscritos,(temaBuscado->numeroSubscritos*sizeof(ListSubscritos)));
             
         nodoSubscrito *actual2=(nodoSubscrito*)malloc(sizeof(nodoSubscrito));
         bzero((void*)actual2,sizeof(nodoSubscrito));
	 actual2 =(temaBuscado->subscritos)->inicio;
	  printf("1");
          
	 for (i = 0; (actual2!=NULL)&& (i<temaBuscado->numeroSubscritos); i++) {               
                printf("Antes de la comprobacion \n");
		if (sonIguales(actual2->dirSubscrito, mensaje->dirCliente)==1){
			encontrado2=1;   // esta dado de alta.
                        printf("Esta dado de alta \n");
		} 
               else
		  actual2=actual2->siguiente;
                  
    	   }
	 }
	   if(encontrado2==0){
	     return -1; //no esta dado de alta
            }
	    
             printf("Eliminamos de la lista \n");
  
          // añadimos a la lista.
	     
           
	     //  fprintf(stdout,"direccion del cliente %d\n",inet_ntoa(mensaje->dirCliente->sin_addr));
	     if(temaBuscado->numeroSubscritos==1){
           	       printf("eliminamos subscrito\n");
		       sup_inicio(temaBuscado->subscritos,1);
          
            }   
            else
	       sup_en_listaSubs(temaBuscado->subscritos,i-1); 
	  
           temaBuscado->numeroSubscritos--;
	   printf("comprobacion de añadido a la lista subscritos\n");
           
	   imprimir_subs(temaBuscado->subscritos);
	 
	return 0;
}





void enviarPeticion( mensaje_evento* mens,struct sockaddr_in * dirCliente) {
	int s;
         
        printf("se mete por enviarPeticion\n");
 	if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return;
	}
        printf("hemos creado el socket\n");

	if (connect(s,(struct sockaddr *) dirCliente, sizeof(struct sockaddr_in))< 0) {
		perror("error en connect");
		close(s);
		return;
	}

        printf("despues de connect  %d\n",s);
      
	//*(mens->evento)=temaSave;
	if (write(s, mens,sizeof(mensaje_evento)) < 0) {
		perror("error en write");
		close(s);
		return;
	}
        printf("despues de write\n");
	close(s);
	return;
}


int realizarAlta(mensaje_evento* mensaje,ListaTemas * listaTemas) {
	
        nodoLista * temaBuscado;
        //temaBuscado = buscarTema(mensaje->evento, listaTemas);
         nodoLista *actual;
         actual = listaTemas->inicio;
         printf("BUSCOOOOOOO  %s \n",mensaje->evento);
         int encontrado=0;
         while (actual != NULL&&encontrado==0){
           printf("TENGOOOOOOOO %s  \n",actual->nombre);
	   if (strcmp(actual->nombre, mensaje->evento) == 0) {
			fprintf(stdout,"buscarTema: hemos encontrado el tema: %s\n",actual->nombre);
                         temaBuscado= actual;
                         encontrado=1;
                         }
           actual = actual->siguiente;
          }
         if(!encontrado)
          return -1;  
        
       
         printf("1");      
         int i;
           if(temaBuscado->numeroSubscritos==0){ //Es vacia
               temaBuscado->subscritos=( ListSubscritos*)malloc(sizeof( ListSubscritos));
               bzero((void*)temaBuscado->subscritos,sizeof(ListSubscritos)); 
	   }
	   else{
	     temaBuscado->subscritos=realloc(temaBuscado->subscritos,(temaBuscado->numeroSubscritos*sizeof(ListSubscritos)));
             
         nodoSubscrito *actual2=(nodoSubscrito*)malloc(sizeof(nodoSubscrito));
         bzero((void*)actual2,sizeof(nodoSubscrito));
	 actual2 =(temaBuscado->subscritos)->inicio;
	 
	 for (i = 0; (actual2!=NULL)&& (i<temaBuscado->numeroSubscritos); i++) {               
                printf("Antes de la comprobacion \n");
		if (sonIguales(actual2->dirSubscrito, mensaje->dirCliente)==1)
			return -1;   //ya esta dado de alta.
                else
		  actual2=actual2->siguiente;
                  
    	   }
	 }
	  
	    
             printf("Añadimos a la lista \n");
  
          // añadimos a la lista.
             printf("mensaje puerto %d\n", mensaje->puerto);
             mensaje->dirCliente->sin_port=ntohs(mensaje->puerto);
             mensaje->dirCliente->sin_family=PF_INET;	     
           
	     //  fprintf(stdout,"direccion del cliente %d\n",inet_ntoa(mensaje->dirCliente->sin_addr));
	     if(temaBuscado->numeroSubscritos==0){
           	       printf("YYYYYYYYY");
		      temaBuscado->subscritos= inser_lista_vaciaSubs(temaBuscado->subscritos,mensaje->dirCliente);
          
            }   
            else
	       ins_fin_listaSubs(temaBuscado->subscritos,mensaje->dirCliente); 
	  
           temaBuscado->numeroSubscritos++;
	   printf("comprobacion de añadido a la lista subscritos\n");
           
	   imprimir_subs(temaBuscado->subscritos);
	 
	return 0;
}


nodoLista* buscarTema(char* tema, ListaTemas* listaTemas) {
  
  nodoLista *actual;
  actual = listaTemas->inicio;
  printf("BUSCOOOOOOO  %s \n",tema);
  while (actual != NULL){
    printf("TENGOOOOOOOO %s  \n",actual->nombre);
      if (strcmp(actual->nombre, tema) == 0) {
			fprintf(stdout,"buscarTema: hemos encontrado el tema: %s\n",actual->nombre);
                         return actual;
                         }
      actual = actual->siguiente;
  }
  return NULL;  
}



int sonIguales(  struct sockaddr_in * dir1,   struct sockaddr_in  * dir2) {
  printf("se mete por sonIguales\n");
  if ((dir1->sin_addr.s_addr == dir2->sin_addr.s_addr)&&(dir2->sin_port==dir1->sin_port)) {
		return 1;
	}
	return 0;
}


int realizarNotificacion(mensaje_evento* mens, ListaTemas* listaTemas) {
    char  baja [64]="baja";
    char  alta [64]="alta";
    char  evento [64]="evento";

  
   if (strcmp(mens->codigo,evento) == 0) {
     printf("se mete por evento\n");
			return realizarEvento(mens,listaTemas);
		  }   
  if (strcmp(mens->codigo,baja) == 0) {
          printf("se mete por baja\n");
          return realizarBaja(mens,listaTemas);
		  }
   if (strcmp(mens->codigo,alta) == 0) {
     printf("se mete por alta\n");
           return realizarAlta(mens,listaTemas);
		  }
	return -1;
}



int realizarEvento( mensaje_evento* mensaje, ListaTemas* listaTemas) {

       
        nodoLista * temaBuscado;//=(nodoLista*)malloc(sizeof(nodoLista));
        printf("buscamos el tema \n");
        temaBuscado = buscarTema(mensaje->evento, listaTemas);
        printf("hemos buscado el tema\n");
	if (temaBuscado == NULL ) {
             printf("Se recibio el tema:  %s y no existe \n", mensaje->evento);
		return -1;
	}
        fprintf(stdout,"procesar evento:hemos encontrado el tema: %s\n",temaBuscado->nombre);
	int i;
         
       if(temaBuscado->numeroSubscritos!=0){ //No Es vacia
       
         nodoSubscrito *actual;//=(nodoSubscrito*)malloc(sizeof(nodoSubscrito));
	 printf("miramos si hay alguien subscrito al tema\n");
         if(temaBuscado == NULL)
           printf("No tenemos la lista de subscritos\n");
         imprimir_subs(temaBuscado->subscritos);
         
         actual=(temaBuscado->subscritos)->inicio;
	 printf("1");
	 for (i = 0; (actual!=NULL)&&(i < temaBuscado->numeroSubscritos); i++) {          printf("2");
	  enviarPeticion(mensaje,actual->dirSubscrito);
	  actual=actual->siguiente;
          printf("3");
	}
       }
       printf("salimos de procesar evento\n");
	return 0;

}

///////////////////////////////////////////////CODIGO PRINCIPAL///////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
	if (argc!=3) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}
 
	int puerto= atoi(argv[1]);
	
  char ficheroTemas[128];
 	sprintf(ficheroTemas,"./%s",argv[2]); //Obtenemos en ficheroTemas la ruta del fichero de temas
  FILE *fp;
	if((fp = fopen(argv[2],"r"))==NULL)
		exit(1);  
        
	       
	   ListaTemas *temas= inicializaListTemas(fp);
	  
	      	imprimir_temas(temas);
	     
  

	int s,s_conec, leido;
	unsigned int tam_dir;
	struct sockaddr_in dir, dir_cliente;
	mensaje_evento mensajeResp;
	int opcion = 1, resp;
	
	        
        printf("vamos a crear el socket\n");
	if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return 1;
	}
	
	// Para reutilizar puerto inmediatamente
	 
	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opcion, sizeof(opcion)) < 0) {
		perror("error en setsockopt");
		return 1;
	}
          printf("se ha creado el socket\n");
	  
	dir.sin_addr.s_addr = INADDR_ANY;
	dir.sin_port = htons(puerto);
	dir.sin_family = PF_INET;
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

	printf("Despues de listen\n");
        while(1){
       
		tam_dir = sizeof(dir_cliente);
		if ((s_conec = accept(s, (struct sockaddr *) &dir_cliente, &tam_dir))< 0) {
			perror("error en accept");
			close(s);
			return 1;
		}
         
	    
		printf("Despues de accept\n");




		mensaje_evento mens;
                printf("Despues dE MENS\n");
		if((leido = read(s_conec, &mens, sizeof(mens))) <0 ){
		  perror("Error en el recv");
		}
		fprintf(stdout,"Bytes leidos %d\n",leido);
                
                temaSave=mens.evento;                                                            
                strcat(mens.evento,"\n");
                fprintf(stdout,"Tema a emitir \"%s\", texto \"%s\", codigo \"%s\"\n",mens.evento, mens.texto,mens.codigo);
		  
                struct sockaddr_in dirCliente;		
                int tam =sizeof(dirCliente);
                getpeername(s_conec,(void*) &dirCliente,(socklen_t *)&tam);
                mens.dirCliente=&dirCliente;
		printf("antes de procesar notificacion \n");
                resp=realizarNotificacion(&mens,temas);
                /*if(temas->inicio!=NULL){
		imprimir_subs(temas->inicio->subscritos);
                }*/
                
                if(resp==-1){  //codigo error
                strcpy(mens.codigo,"error");
                } 
         	if (write(s_conec, &mens, sizeof(mensajeResp)) < 0) {
		      
			perror("error en write");
			close(s);
			close(s_conec);
			return 1;
		}
                
		close(s_conec);
	        printf("Despues de close de conexion\n");
                imprimir_temas(temas); 
          } 
      
	close(s);

	return 0;

}
