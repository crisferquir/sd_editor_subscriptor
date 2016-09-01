/*
 Incluya en este fichero todas las implementaciones que pueden
 necesitar compartir los módulos editor y subscriptor,
 si es que las hubiera.
 */

#include "edsu_comun.h"
#include "comun.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int conectarIntermediario() {


	char* servidor = getenv("SERVIDOR");
	char* puerto = getenv("PUERTO");
  struct hostent *host_info;
	struct sockaddr_in dir;
	host_info = gethostbyname(servidor);
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dir.sin_port = htons(atoi(puerto));
  int s;
    
	dir.sin_family = PF_INET;

	if ((s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("error creando socket");
		return -1;
	}

	if (connect(s, (struct sockaddr *) &dir, sizeof(dir)) < 0) {
		perror("error en connect");
		close(s);
		return -1;
	}
	return s;
}

