

/*
 Incluya en este fichero todas las definiciones que pueden
 necesitar compartir todos los módulos (editor, subscriptor y
 proceso intermediario), si es que las hubiera.
 */

typedef struct MessageEvent{
  char codigo[32];       //tipo de mensaje {alta,baja,evento}
  char evento[64];     //evento al que pertenece(envio o subscripcion) solo valido hasta el primer espacio
  char texto[64];  //datos finales
  int puerto;
  struct sockaddr_in * dirCliente; // en iPv6 va de 16 a 46.
}mensaje_evento;

char *quitarCaracterLinea(char* cadena,int n);
