//Practica tema 6, Lozano Olmedo Jose Maria
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>


#define TAMANO_RECIBIR_CLIENTE 200 
#define TAMANO_ENVIAR_CLIENTE 200 
#define TAMANO_OBTENER_FECHA 200 
#define TAMANO_OBTENER_HOST 200 

int sockfd;
char bufferRecibirCliente[TAMANO_RECIBIR_CLIENTE];

static void sigintHandler(int sig)
{
    
    //printf("cierro el socket: %d\n",sockfd);
    
   
    
    //close cierra el file descriptor para cerrar el socket
    if(close(sockfd)<0){
    	perror("close ha fallado: "); 
        exit(EXIT_FAILURE); 
    }
    
    
    //printf("cerrado el socket: %d\n",sockfd);
    exit(EXIT_SUCCESS);
}


int main(int argc,char *argv[])
{

	//signal establece que funcion maneja una señal
	//primero me llega el numero de la señal y despues la funcion que la va a manejar
	//sigint es la señal mandada por el usuario al pulsar control c
	//devuelve sig_err si error
	if(signal(SIGINT, sigintHandler)==SIG_ERR){
		perror("signal ha fallado: "); 
        	exit(EXIT_FAILURE); 
	}
        
	//variables que voy a utilizar
  	char bufferEnviarCliente[TAMANO_ENVIAR_CLIENTE];
    	char bufferObtenerFecha[TAMANO_OBTENER_FECHA];
    	char bufferObtenerHost[TAMANO_OBTENER_HOST];
    	FILE *fich;
	int acceptFd,ret;
	struct sockaddr_in servaddr, cli;
	int puerto; 
    	struct servent *servinfo; 
    	pid_t childpid;
    	socklen_t cli_addr_len = 0;


	//socket crea un socket para la comunicacion. 
	//af_inet es para que use ipv4
	//sock stream para que use tcp
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		perror("socket ha fallado: "); 
        	exit(EXIT_FAILURE); 
	}
	
	//printf("socket despues de socket: %d\n",sockfd);
	
	
	
	servaddr.sin_family = AF_INET;//para usar ipv4
	servaddr.sin_addr.s_addr = INADDR_ANY;// para que cualquier ip se pueda conectar al socket
	
	
	if(argc==1){ 	
   		servinfo = getservbyname("daytime", "tcp");//busco el puerto que tiene el servicio de daytime con el protocolo tcp
    		if(!servinfo) {
        		fprintf(stderr,"no he encontrado el servicio daytime\n");
        		exit(1);
    		}
   
    		servaddr.sin_port = htons(ntohs(servinfo -> s_port));//asigno el puerto del daytime al servidor 
    	
   		}
   
   	else if(argc==3){
   	//con -p puerto
   		if(strcmp("-p",argv[1])==0){
   			sscanf(argv[2]," %d",&puerto);//transformo el puerto de argumento a numero
    			servaddr.sin_port = htons(puerto);//asigno el puerto que viene de argumento al servidor
    		}
    		//puerto -p
    		else{
    			printf("se utiliza: ./daytime-tcp-server-lozano-olmedo -p puerto\n");
    			exit(1);
    		}
    		
  	}else{
   		printf("se utiliza: ./daytime-tcp-server-lozano-olmedo -p puerto\n");
   		exit(1);
   	}
    
    
	
	

	
	
	

	//bind asigna direccion de servaddr al socket sockfd
	//sockfd es el file descriptor del socket
	//servaddr es la direccion que se le va a asignar al socket del file descriptor
	//por ultimo como argumento llega el tamano de la estrutura que tiene la direccion que se va a asignar al socket
	if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) < 0) {
		perror("bind ha fallado: "); 
        	exit(EXIT_FAILURE);
	}


	//listen marca el socket como pasivo, por lo que acepta conexiones
	//sockfd es el file descriptor del socket
	//el segundo argumento dice el numero maximo que puede alcanzar la cola de conexiones pendientes
	if ((listen(sockfd, 5)) < 0) {
		perror("listen ha fallado: "); 
        	exit(EXIT_FAILURE);
	}
	
	
	 //cojo el tamano de la estructura que guarda los datos del cliente
    	cli_addr_len = sizeof(cli); 


	while(1){//entro en bucle infinito
	
	
	

	
		//con accept hace que coja la primera peticion pendiente de la cola del socket por lo tanto es bloqueante porque 
		//espera la conexion de un cliente
		//sockfd se refiere al file descriptor del socket
		//el segundo argumento cli es la estructura que tiene la direccion del cliente
		//el ultimo argumento es el tamano de la anterior estructura
		//devuelve un file descriptor para el socket aceptado
		if ((acceptFd = accept(sockfd, ( struct sockaddr*)&cli, &cli_addr_len))<0) {
			perror("accept ha fallado: "); 
        		exit(EXIT_FAILURE);
		}

 
       	//printf("socket despues de accept: %d\n",acceptFd);
       	 //printf("conexion aceptada desde %s:%d\n",inet_ntoa(cli.sin_addr),ntohs(cli.sin_port));
	
		




		if ((childpid = fork()) == 0) {
			//la fecha se busca dentro del fork para que se actualice a cada cliente la que corresponda
			//obtengo fecha y hora segun se ha explicado en el enunciado
    			system("date > /tmp/tt.txt");
			fich = fopen("/tmp/tt.txt","r");
			if (fgets(bufferObtenerFecha,TAMANO_OBTENER_FECHA,fich)==NULL) {
				printf("Error en system(), en fopen(), o en fgets()\n");
				exit(EXIT_FAILURE);
			}
	
	
	
   
			//obtengo nombre del host
    			ret = gethostname(&bufferObtenerHost[0], TAMANO_OBTENER_HOST);
    			if (ret == -1) {
        			perror("error en el gethostname: ");
        			exit(EXIT_FAILURE);
    			}
    
    
    			//lo pongo en el formato que se pide
    			strcpy(bufferEnviarCliente,bufferObtenerHost);
    			strcat(bufferEnviarCliente,": ");
    			strcat(bufferEnviarCliente,bufferObtenerFecha);
		
		
		
		
		
		
			
		//send envia el mensaje por el socket
		//el primer argumento es el file descriptor del socket devuelto por accept
		//bufferEnviarCliente es el buffer que tiene el mensaje a enviar
		//ademas se pasa el tamano del mensaje
		// el ultimo argumento es una bandera que dice el tipo de transmision.con 0 no se pone ninguna bandera
	 		if (send(acceptFd, bufferEnviarCliente, strlen(bufferEnviarCliente), 0) < 0){
        			perror("error en el send: ");
        			exit(EXIT_FAILURE);
    			}
			//printf("le he enviado mensaje al cliente por el socket: %d\n",acceptFd);
	
	
			 //este recv debe devolver cero bytes, indicando que el otro extremo se 
			 //ha dado por enterado y por lo que ha cerrado la conexion.
			if (recv(acceptFd, bufferRecibirCliente, TAMANO_RECIBIR_CLIENTE, 0) !=0){
         			perror("recv ha fallado: "); 
        			exit(EXIT_FAILURE); 
    			}
    			
    			
    			 //shutdown deshabilita los envios y recepciones con el socket
    			 //argumentos el file descriptor del socket y como voy a apagar el socket
    			 // el 2 equivale a shut_rdwr: desactiva operaciones para enviar y recibir
    			if(shutdown(acceptFd,2)<0){
    				perror("shutdown ha fallado: "); 
        			exit(EXIT_FAILURE); 
    			}
    
    			//close cierra el file descriptor para cerrar el socket
    			if(close(acceptFd)<0){
    				perror("close ha fallado: "); 
        			exit(EXIT_FAILURE); 
    			}
    			//printf("he cerrado en el hilo el socket del accept: %d\n",acceptFd);
    			
    			exit(EXIT_SUCCESS);//termino el hilo porque si no invoca la funcion signal_handler varias veces
    			
 		} //fork  

}//while




	
}

