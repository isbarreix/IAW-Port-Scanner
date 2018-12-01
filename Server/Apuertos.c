/**
 * Port scanner
 * 
 * @author Barreix, 	Iñaki Salvador 		
 * @author Leguizamon, 	Marcos Agustín 		
 */

#include <sys/socket.h>		
#include <sys/select.h>	
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/time.h>	
#include <fcntl.h>
#include <sysexits.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <time.h>
#include "list.h"
#include "Apuertos.h"

#define MAXTIME			1
#define MIN_PORT		1
#define MAX_PORT		1000
#define IPSIZE			40
#define PORTCHARSIZE	80
#define ls				75
#define lf				85
#define DELIM 			"."
#define EXIT_ERR_INPUT 	69
#define EXIT_ERR_IP 	70


/**
 * valid_digit(char* ip_str)
 * Descripción: Dado un char, chequea si es un dígito válido. Es un método auxiliar de is_valid_ip
 * Parámetros de entrada: char * ip_str
 * Salida: 0 o 1 dependiendo si es un dígito váli
 * */
	/* return 1 si el string contiene solo digitso, else return 0 */
int valid_digit(char *ip_str){
    while (*ip_str) {
        if (*ip_str >= '0' && *ip_str <= '9')
            ++ip_str;
        else
            return 0;
    }
    return 1;
}

/**
 * is_valid_ip(char* ip_str)
 * Descripción: Dada una IP, determina si es válida.
 * Parámetros de entrada: char* ip_str
 * Salida: 1 si es correcto, EXIT_ERR_IP caso contrario.
 */
/** return 1 if IP string es valido, else return EXIT_ERR_INPUT */
int is_valid_ip(char *ip_str){
    int num, dots = 0;
    char *ptr;
 
    if (ip_str == NULL)
        return EXIT_ERR_INPUT;
 
    ptr = strtok(ip_str, DELIM);
 
    if (ptr == NULL)
        return EXIT_ERR_INPUT;
 
    while (ptr) {
 
        /* despues del parsing string, debe contener olo digitos */
        if (!valid_digit(ptr))
            return EXIT_ERR_IP;
 
        num = atoi(ptr);
 
        /* check por IP valido */
        if (num >= 0 && num <= 255) {
            /* traduce el string restante*/
            ptr = strtok(NULL, DELIM);
            if (ptr != NULL)
                ++dots;
        } else
            return EXIT_ERR_IP;
    }
 
    /* El IP debe contener 3 puntos */
    if (dots != 3)
        return EXIT_ERR_IP;
    return 1;
}
/**
 * get_ip(char* hostname, char* ip)
 * Descripción: Dado un nombre de host, devuelve la IP correspondiente.
 * Parámetros de entrada: un nombre de host y una ip que será retornada por referencia.
 * Salida: 1 si es correcto, EXIT_ERR_INPUT caso contrario.
 * 
 * */
int get_ip(char * hostname , char* ip){  
   struct hostent *he;     
   struct in_addr **addr_list;     
   int i;     
   if ( (he = gethostbyname( hostname ) ) == NULL){ 
     return EXIT_ERR_INPUT;
   }     
   addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++){   
		strcpy(ip , inet_ntoa(*addr_list[i]));
        return 0;
     }
    return 1;
}
/** init_socket(int *sock)
 * Descripción: realiza llamada a socket(int domain, int type, int protocol). Es una llamada al sistema, esta función devuelve un descriptor de archivo, al igual que la función open() al crear o abrir un archivo.
 * Parámetros de entrada: descriptor a crear
 * Salida: 0 si se creó exitosamente 
 * */
 
int init_socket(int *sock) {
	if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error a la hora de crear socket\n");
		exit(1);
	}
	return 0;
}

/** set_server(struct sockaddr_in *server_addr, struct hostent *host, int port)
 * Descripción: establece parámetros de conexión con socket servidor destino.
 * Parámetros de entrada: un número de puerto, un host, y una estructura para almacenar la información.
 * Salida: estructura sockadrr_in con parámetros base inicializados 
 * */
int set_server(struct sockaddr_in *server_addr, struct hostent *host, int port) {
	server_addr->sin_family = AF_INET;														// Familia TCP/IP
	server_addr->sin_port = htons(port);													// #Puerto de Server
	server_addr->sin_addr = *((struct in_addr *)host->h_addr);
	bzero(&(server_addr->sin_zero), 8);														// Funcion que rellena con 0's la estructura

	return 0;
}
/**
 * Descripción: Dado que la función connect de sockets es una llamada bloqueante, este método busca establecer una conexión a un puerto no bloqueante. Esto lo realiza a partir de la función select(). Está opera apartir de actividades de descriptores asociados a un socket, notificando una nueva actividad.
 * Parámetros de entrada: descriptor, estructura de conexión, longitud de estructura, y segundos por los que se buscará establecer una conexión.
 * Salida: -3 si puerto a conectar está cerrado; -2 si se encuentra filtrado; -1 error al ver puerto;0 si el puerto está abierto. 
 * */
int conectar_sin_bloquear(int sockfd, struct sockaddr_in *host, socklen_t len , int sec)  {
	int flags, error, n;
	socklen_t elen;
	fd_set rset, wset;
	struct timeval time;

	if ((flags = fcntl (sockfd, F_GETFL, 0)) == -1) {
		perror("Fcntl error");
	}
	if (fcntl (sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("Fcntl error");

	}
	error = 0;

	if ((n = connect (sockfd, (struct sockaddr *) host, len)) < 0)  {
		if (errno != EINPROGRESS){
			perror("Connect error");
			return (-1);
		}
	}
	if (n == 0) {
		goto done;
	}
	FD_ZERO (&rset);
	FD_SET (sockfd, &rset);
	wset = rset;
	time.tv_sec = 0;
	time.tv_usec = 500000;			//	0.5 sec

	if ((n = select (sockfd + 1, &rset, &wset, NULL, sec ? &time : NULL)) == 0) {
		close (sockfd);
		errno = ETIMEDOUT;
		// Puerto filtrado.
		return (-2);
	}
	if (FD_ISSET (sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
		elen = sizeof(error);
		if (getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &elen) < 0) {
			perror("Getsockopt error");
			// Error al inspeccionar el puerto.
			return (-1);
		}
	}

	done:

	if (fcntl (sockfd, F_SETFL, flags) == -1){
		// Error al cambiar atributos del socket.
		perror ("Fcntl error");
	}
	if (error){
		close (sockfd);
		errno = error;
		// Puerto cerrado.
		return (-3);
	}
	// Puerto abierto.
	return (0);

}
/**
 * connect_server(int *sock, struct sockaddr_in server_addr)
 * Descripción: Ilama al conectar sin bloquear.
 * Parámetros de entrada: descriptor, estructura de información de server.
 * Salida: -3 si puerto a conectar está cerrado; -2 si se encuentra filtrado; -1 error al ver puerto;0 si el puerto está abierto.
 * */
int connect_server(int *sock, struct sockaddr_in server_addr){
	socklen_t len = sizeof(server_addr);
	return conectar_sin_bloquear(*sock, &server_addr, len, 1); 
}	
/**
 * connectServer(char *serverName, int port)
 * Descripción: inicializa estructuras y elementos del servidor a conectar
 * Parámetros de entrada: Nombre de servidor y puerto.
 * Salida: descriptor de conexión.

 * */
int connectServer(char *serverName, int port) {
	/*INIT connection */
	int sock;
	struct hostent *host;
	struct sockaddr_in server_addr;
		
	if (init_socket(&sock) != 0)
		exit(EXIT_FAILURE);
	host = gethostbyname(serverName);
	set_server(&server_addr, host, port);
	
	// Conectarnos al servidor (Llama al conectar sin bloquar)
	if(connect_server(&sock, server_addr)!=0)
		exit(EXIT_FAILURE);			// el servidor se cerro o filtro luego de analizarlo.
	
	return sock;
	/*FIN  connection*/
}


char* validate_argv(int argc, char **argv, char* server_name, int *pinitial, int *pfinal) {
	int validIP, validHost, esIP = 0, j, c;
	/* Valido la IP de entrada*/
	char ip[IPSIZE], inputAux[IPSIZE];
	char *token;

	fprintf(stderr,"argv: %s\n", argv[3]);

	*pinitial = MIN_PORT;
	*pfinal = MAX_PORT;
	strcpy(ip, argv[1]);

	strcpy(inputAux, argv[1]);
	if(strcmp(inputAux,"") == 0 || strcmp(inputAux,"-r") == 0) {
		return	"EXIT_ERR_INPUT: El campo [IP/HOST] no puede ser vacío\n";
		exit(EXIT_ERR_INPUT);
	}
	
	validIP = is_valid_ip(argv[1]);
	validHost = get_ip(inputAux,server_name);
	
	if(validIP != EXIT_ERR_IP){ //verifica que el parámetro ingresado no sea nulo y que la IP sea válida
		server_name = ip;
		esIP = 1;
	}
	else {	
		if (validHost == EXIT_ERR_INPUT && validIP == EXIT_ERR_IP){	//Se ingresó un domino inválido
			 return	"EXIT_ERR_INPUT: El [HOST/IP] ingresado no es válido\n";
			 exit(EXIT_ERR_INPUT);
		}
	}

	char inicioAux[50];
	char finAux[50];

	memset(inicioAux, '\0', sizeof(inicioAux));
	memset(finAux, '\0', sizeof(finAux));

	token = strtok(argv[3],":");
	strcpy(inicioAux,token);
	*pinitial = atoi (inicioAux);

	if(*pinitial <= 0){
		return	"EXIT_ERR_INPUT: El puerto inicial debe ser mayor o igual que 1. Vuelva a intentar\n";
		exit(EXIT_ERR_INPUT);
	}

	token = strtok(NULL, ":");
	strcpy(finAux, token);

	*pfinal = atoi(finAux);
	if (*pfinal < *pinitial){
		return	"EXIT_ERR_INPUT: El puerto final debe ser más grande que el puerto inicial. Vuelva a intentar\n"; 
		exit(EXIT_ERR_INPUT);
	}
	if (*pfinal > 1000){
		return	"EXIT_ERR_INPUT: El máximo valor para el puerto final es 1024.\n"; 
		exit(EXIT_ERR_INPUT);
	}

	if(esIP)
		return "_OK";
	else
		return "EXIT_ERR_INPUT";
}

int scan_ports(char* server_name, int pinitial, int pfinal, int *pabiertos, int *pfiltrados, int *pcerrados, tLista *list_port) {
	int sock, n, port;  						// descriptor a usar con el socket
	struct hostent *host;						// obtiene el nombre del host
	struct sockaddr_in server_addr;  
	
	for(port = pinitial; port <= pfinal; port++){
		if (init_socket(&sock) != 0)
			exit(EXIT_FAILURE);
		host = gethostbyname(server_name);
		set_server(&server_addr, host, port);
		// Conectarnos al servidor (Llama al conectar sin bloquar)
		n = connect_server(&sock, server_addr);
		switch (n) {
			case -3:									//puerto cerrado
				(*pcerrados)++;
				break;
			case -2:									// puerto filtrado
				(*pfiltrados)++;								
				break;
			case -1:									//dio error conectar server
				exit(EXIT_FAILURE);
				break;
			case 0:										//puerto abierto.
				(*pabiertos)++;
				insertarFinal(list_port, port); 		// lo agrego a los puertos abiertos
				break;
		}		
		close(sock);									// Cerramos descriptor del socket
	}

	return 0;
}

/**
 * getServiceByPort (int port)
 * Descripción: Dado un puerto, devuelve el posible servicio asociado a este.
 * Parámetros de entrada: un puerto tcp
 * Salida: el posible servicio (por ejemplo, puerto 80: posible servicio HTTP)
 * 
 * */

char* getServiceByport(int port) {
	struct servent *service; 
	service = getservbyport(htons(port), "tcp");	
	return service->s_name;
}

void getPortServicesList(int pabiertos, tLista list_port, char *json_listports) {
	char *service;
	char cport[7];
	int iport;
	strcat(json_listports, "[\n");

	while(list_port != NULL) {									// puertos abiertos parse to json
		fflush(stdout);	//esto es por los printf
		iport = list_port->elto;
		service = getServiceByport(iport);
	
		//generamos json
		strcat(json_listports, "\t\t\t{\n\t\t\t\t\"port\":\"");
		sprintf(cport, "%d", iport);									//itoa
		strcat(json_listports, cport);	
		strcat(json_listports, "\"");								// { "port":"puertoX"
		
		strcat(json_listports, ",\n\t\t\t\t\"service\":\"");
		strcat(json_listports, service);	
		strcat(json_listports, "\"");								// { "port":"puertoX", "service":"servicioX"
		
		strcat(json_listports, ",\n\t\t\t\t\"state\":\"OPEN\"\n\t\t\t}");			// { "port":"puertoX", "service":"servicioX", "state":"OPEN" }
		
		if(list_port->sig != NULL)								// El ultimo no lleva coma
			strcat(json_listports, ",\n");	
		else
			strcat(json_listports, "\n");
		list_port = list_port->sig;
	}
	strcat(json_listports, "\t\t]\n");
}

void getJSONReport(char *server_name, int pinitial, int pfinal, char *json_report) {
	int abiertos = 0, cerrados = 0, filtrados = 0;
	tLista list_port;
 	char cport[7];
 		
	list_port = NULL;	
	scan_ports(server_name, pinitial, pfinal, &abiertos, &filtrados, &cerrados, &list_port);
	
	char json_listports[1024];
	memset(json_listports, 0, strlen(json_listports)); 				// se inicializa el string
	getPortServicesList(abiertos, list_port, json_listports);


	memset(json_report, 0, strlen(json_listports)); 				// se inicializa el string
		
	//	armamos json
	strcat(json_report, "{\n");
	strcat(json_report, "\t\"IP\": \"");
	strcat(json_report, server_name);
	
	strcat(json_report, "\",\n");
	strcat(json_report, "\t\"inicial\": ");
	sprintf(cport, "%d", pinitial);
	strcat(json_report, cport);	
	strcat(json_report, ",\n");
	
	strcat(json_report, "\t\"final\": ");
	sprintf(cport, "%d", pfinal);
	strcat(json_report, cport);	
	strcat(json_report, ",\n");
	
	strcat(json_report, "\t\"cerrados\": {\n");
	strcat(json_report, "\t\t\"cant\": ");
	sprintf(cport, "%d", cerrados);
	strcat(json_report, cport);
	strcat(json_report, ",\n");
	strcat(json_report, "\t\t\"list\": ");	
	strcat(json_report, "[]\n");		//vacio
	strcat(json_report, "\t},\n");
	
	strcat(json_report, "\t\"filtrados\": {\n");
	strcat(json_report, "\t\t\"cant\": ");
	sprintf(cport, "%d", filtrados);
	strcat(json_report, cport);
	strcat(json_report, ",\n");
	strcat(json_report, "\t\t\"list\": ");	
	strcat(json_report, "[]\n");		//vacio
	strcat(json_report, "\t},\n");
		
	strcat(json_report, "\t\"abiertos\": {\n");
	strcat(json_report, "\t\t\"cant\": ");
	sprintf(cport, "%d", abiertos);
	strcat(json_report, cport);
	strcat(json_report, ",\n");
	strcat(json_report, "\t\t\"list\": ");	
	strcat(json_report, json_listports);	// obtenemos lista de puertos
	strcat(json_report, "\t}\n");
	
	strcat(json_report, "}\n");

}
