//Practica tema 6, Lozano Olmedo Jose Maria
#include <arpa/inet.h> 
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> 
#include <sys/socket.h>
#include <unistd.h> 


#define TAMANO_RECIBIR_SERVIDOR 200



int main(int argc, char *argv[])
{
	 int sockfd;
	 struct sockaddr_in servaddr;
	 struct in_addr addr;
	 struct servent *servinfo; 
	 int puerto;
	 char bufferRecibirServidor[TAMANO_RECIBIR_SERVIDOR];


	//socket crea un socket para la comunicacion. 
	//af_inet es para que use ipv4
	//sock stream para que use tcp
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0) {
		 perror("socket ha fallado: "); 
        	exit(EXIT_FAILURE); 
	}
	
	if(argc==2){
     
     		if (inet_aton(argv[1], &addr) == 0) {//convierto la direccion a ip de argumento a un numero en network byte order
        		perror("error en el inet_aton");
        		exit(EXIT_FAILURE);
    		}
   	
    		servinfo = getservbyname("daytime", "tcp");//busco el puerto que tiene el servicio de daytime con el protocolo tcp
    		if(!servinfo) {
        		fprintf(stderr,"no he encontrado el servicio daytime\n");
        		exit(1);
    		}
   
    		servaddr.sin_port = htons(ntohs(servinfo -> s_port)); //asigno el puerto del daytime al servidor 

   	}else if(argc==4){
   		//con direccion -p puerto
   		if(strcmp("-p",argv[2])==0){
   			sscanf(argv[3]," %d",&puerto);
   			//aqui pense en permitir solo la entrada de numeros pero he visto que sscanf aunque sea texto lo pasa a numero
    			servaddr.sin_port = htons(puerto);//asigno el puerto que viene de argumento al servidor
    		}else{
    			//sin -p
    			printf("se utiliza: ./daytime-tcp-client-lozano-olmedo direccion -p puerto\n");
   			exit(1);
    		}	
   	}else{
   		printf("se utiliza: ./daytime-tcp-client-lozano-olmedo direccion -p puerto\n");
   		exit(1);
   	}
    
	
	


	servaddr.sin_family = AF_INET;//para ipv4
	inet_aton(argv[1], &servaddr.sin_addr); //pongo la ip que me pasa de argumento


	//connect conecta el socket del file descriptor sockfd con la direccion de servaddr
	//el primer argumento es el file descriptor del socket
	//servaddr es la estructura que tiene la direccion que se quiere conectar
	//por ultimo se pasa el tamano de la anterior estructura
	if (connect(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
		 perror("connect ha fallado: "); 
        	exit(EXIT_FAILURE); 
	}


	
	
	

	//recv recibe mensajes del socket
	//sockfd es el file descriptor del socket 
	//despues paso el buffer donde guardar el mensaje
	//el tamano de ese buffer
	//por ultimo una bandera para definir el tipo de recepcion del mensaje. con 0 no hay banderas
	if (recv(sockfd, bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR, 0) < 0){
         	perror("recv ha fallado: "); 
        	exit(EXIT_FAILURE); 
    	}
	printf("%s\n", bufferRecibirServidor);

	//shutdown deshabilita los envios y recepciones con el socket
    	//argumentos el file descriptor del socket y como voy a apagar el socket
    	// el 2 equivale a shut_rdwr: desactiva operaciones para enviar y recibir
	if(shutdown(sockfd,2)<0){
    		perror("shutdown ha fallado: "); 
        	exit(EXIT_FAILURE); 
    	}
    
    
     //este recv debe devolver cero bytes, indicando que el otro extremo se 
	//ha dado por enterado y por lo que ha cerrado la conexion.
     	if (recv(sockfd, bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR, 0) !=0){
         	perror("recv ha fallado: "); 
        	exit(EXIT_FAILURE); 
    	}
	
	
	//close cierra el file descriptor para cerrar el socket
	if(close(sockfd)<0){
    		perror("close ha fallado: "); 
        	exit(EXIT_FAILURE); 
    	}
    	
    	return 0;
}

