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
	//fprintf(stderr, "Payload Length: %llu\n", msg->payload_len);
	//fprintf(stderr, "Payload: %s\n", msg->payload);
	
	free(msg->payload);
	msg->payload = NULL;

	//now let's send it back.
	int argc = 4, pinitial, pfinal;
	char a0[] = "apuertos";
	char a1[] = "195.144.107.198";
	char a2[] = "-r";
	//char a3[] = "1:81";
	char a3[] = "12:28";
	char *argv[] = { a0, a1, a2, a3 };
	char *server_name;	
	server_name = (char *) malloc(50);

	validate_argv(argc, argv, server_name, &pinitial, &pfinal);
	char json_report[2048];
	getJSONReport(server_name, pinitial, pfinal, json_report);
			
//	char *example = "{\n	\"IP\": \"195.144.107.198\",\n	\"inicial\": 1,\n	\"final\": 1024,\n	\"cerrados\": {\n		\"cant\": 0,\n		\"list\": []\n	},\n	\"filtrados\": {\n		\"cant\": 1013,\n		\"list\": []\n	},\n	\"abiertos\": {\n		\"cant\": 1013,\n		\"list\": [\n			{\n				\"port\":\"13\",\n				\"service\":\"daytime\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"21\",\n				\"service\":\"ftp\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"22\",\n				\"service\":\"ssh\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"25\",\n				\"service\":\"smtp\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"37\",\n				\"service\":\"time\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"80\",\n				\"service\":\"http\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"143\",\n				\"service\":\"imap2\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"443\",\n			\"service\":\"https\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"444\",\n				\"service\":\"snpp\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"990\",\n				\"service\":\"ftps\",\n				\"state\":\"OPEN\"\n			},\n			{\n				\"port\":\"993\",\n				\"service\":\"imaps\",\n				\"state\":\"OPEN\"\n			}\n		]\n	}\n}\n";

	//printf("-SIZE: %lu\n", strlen(json_report));
	libwebsock_send_text(state, json_report);
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
