/*

Install libwebsock with:

./autogen.sh
./configure && make && sudo make install


Then, compile this echo server with:

gcc -g -O2 -o echo echo.c -lwebsock

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <websock/websock.h>
#include "Apuertos.h"

/*

Define message callback with same prototype as below.
Accepts pointer to libwebsock_client_state structure and pointer to libwebsock_message structure.
libwebsock_client_state has int sockfd, struct sockaddr_storage *sa
libwebsock_message has unsigned int opcode, unsigned long long payload_len, and char *payload

You probably shouldn't modify any of the data contained in the structures passed to the callback.  It will probably cause bad things to happen.
You can, of course, make copies of the data contained therein.

Here is the default receive_callback as an example:
(libwebsock_send_text accepts libwebsock_client_state * and character array)

int some_callback_name(libwebsock_client_state *state, libwebsock_message *msg) {
	libwebsock_send_text(state, msg->payload);
	return 0;
}

This callback just provides echoing messages back to the websocket client.

You would register this callback via:

ctx->onopen = some_callback_name;


*/


//basic onmessage callback, prints some information about this particular message
//then echos back to the client.
int 
onmessage(libwebsock_client_state *state, libwebsock_message *msg) {
	fprintf(stderr, "Received message from client: %d\n", state->sockfd);
	fprintf(stderr, "Message opcode: %d\n", msg->opcode);
	fprintf(stderr, "Payload Length: %llu\n", msg->payload_len);

	const char delimitador[2] = "-";
	char IP[50];
	char rango[50];

	memset(IP, '\0', sizeof(IP));
	memset(rango, '\0', sizeof(rango));
  

	/* Obtengo la IP */
	char * token = strtok(msg->payload, delimitador);
	strcpy(IP, token);

	/* Obtengo el rango */
	token = strtok(NULL, delimitador);
	strcpy(rango, token);
	

	fprintf(stderr,"IP : %s\n",IP);
	fprintf(stderr,"Rango de puertos es: %s\n",rango);

	//now let's send it back.
	int argc = 4, pinitial, pfinal;
	char comando[] = "apuertos";
	char opcion[] = "-r";
	char *argv[] = { comando, IP, opcion, rango };
	char *server_name;	
	server_name = (char *) malloc(50);

	validate_argv(argc, argv, server_name, &pinitial, &pfinal);
	char json_report[2048];
	getJSONReport(server_name, pinitial, pfinal, json_report);
	fprintf(stderr, "%s",json_report);
			
	libwebsock_send_text(state, json_report);
	free(msg->payload);
	msg->payload = NULL;
	return 0;
}

int
onopen(libwebsock_client_state *state) {
	fprintf(stderr, "onopen: %d\n", state->sockfd);
	return 0;
}

int
onclose(libwebsock_client_state *state) {
	fprintf(stderr, "onclose: %d\n", state->sockfd);
	return 0;
}

int
main(int argc, char *argv[]) {	
	libwebsock_context *ctx = NULL;
	if(argc != 2) {
	fprintf(stderr, "Usage: %s <port to listen on>\n\nNote: You must be root to bind to port below 1024\n", argv[0]);
	exit(0);
	}
	ctx = libwebsock_init();
	if(ctx == NULL) {
	fprintf(stderr, "Error during libwebsock_init.\n");
	exit(1);
	}
	libwebsock_bind(ctx, "0.0.0.0", argv[1]);
	fprintf(stderr, "libwebsock listening on port %s\n", argv[1]);
	ctx->onmessage = onmessage;
	ctx->onopen = onopen;
	ctx->onclose = onclose;
	libwebsock_wait(ctx);
	//perform any cleanup here.
	fprintf(stderr, "Exiting.\n");
  return 0;
}
