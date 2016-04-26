
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


#include "../../COMUNES/nsockets.c"

#define IP "127.0.0.1"
#define PUERTO "9997"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

#include <signal.h>

int serverSocket;
void intHandler(int dummy);
int main(){
 signal(SIGINT, intHandler);


serverSocket = cliente("127.0.0.1",1201);

	if(serverSocket==0){
		printf("No encontre NUCLEO me cierro :'( %d",&serverSocket);
	  exit (EXIT_FAILURE);
	}
	

	int enviar = 1;
	char message[PACKAGESIZE];
	t_header header;

	printf("Conectado al servidor. Bienvenido al sistema, ya puede enviar mensajes. Escriba 'exit' para salir\n");

	while(enviar){
		fgets(message, PACKAGESIZE, stdin);			// Lee una linea en el stdin (lo que escribimos en la consola) hasta encontrar un \n (y lo incluye) o llegar a PACKAGESIZE.
		if (!strcmp(message,"exit\n")) enviar = 0;			// Chequeo que el usuario no quiera salir

		
		header.id = 101;
		header.size = strlen(message) + 1;
		header.data = message;

		enviar_paquete(serverSocket, header);

		//if (enviar) send(serverSocket, message, strlen(message) + 1, 0); 	// Solo envio si el usuario no quiere salir.
	}





	close(serverSocket);
	return 0;

	
}



void intHandler(int dummy) {
	//clrscr();
	
	close(serverSocket);
	//close(listenningSocket);

  printf("cierro Todo...\n\n");
  exit(0);
}

