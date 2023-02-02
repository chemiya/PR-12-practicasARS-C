//Practica tema 5, Lozano OLmedo Jose Maria
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h> 
    

#define TAMANO_RECIBIR_SERVIDOR 200 
    

int main(int argc, char *argv[]) { 
    //defino las variables que utlizo
    int sockfd; 
    char bufferRecibirServidor[TAMANO_RECIBIR_SERVIDOR]; 
    char *bufferEnviarServidor = "hola desde cliente"; 
    struct sockaddr_in     servaddr;
    int puerto;
    struct servent *servinfo; 
    struct in_addr addr;
    int n; 
    socklen_t server_addr_len = 0;
    
    
    
     //creo el socket, con af_inet para ipv4, sock_dgram para udp, y 0 para el protocolo
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("error en la creacion del socket"); 
        exit(EXIT_FAILURE); 
    } 
    
    
    
     if(argc==2){
     
     	if (inet_aton(argv[1], &addr) == 0) {//convierto la direccion a ip de argumento a un numero en network byte order
        	perror("error en el inet_aton");
        	exit(EXIT_FAILURE);
    	}
   	
    	servinfo = getservbyname("daytime", "tcp");//busco el puerto que tiene el servicio de daytime con el protocolo udp
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
    		printf("se utiliza: ./daytime-client-lozano-olmedo direccion -p puerto\n");
   		exit(1);
    	}	
   }else{
   	printf("se utiliza: ./daytime-client-lozano-olmedo direccion -p puerto\n");
   	exit(1);
   }
    
    
    
    
    
    servaddr.sin_family = AF_INET; // tipo de direcciones que usa el socket en este caso ipv4
    inet_aton(argv[1], &servaddr.sin_addr); //pongo la ip que me pasa de argumento
    
        
    
       
       
       
       
    //sendto manda el mensaje por el socket
    //paso el file descriptor del socket,
    //puntero al bufferEnviarServidor donde esta el mensaje a enviar
    //tamano del mensaje
    //bandera msgconfirm para enviarle la respuesta a lo que me ha mandado el cliente
    //puntero a sockaddr que tiene la direccion destino
    //tamano del anterior sockaddr 
    if(sendto(sockfd, (const char *)bufferEnviarServidor, strlen(bufferEnviarServidor), MSG_CONFIRM, (const struct sockaddr *)&servaddr,sizeof(servaddr))<0){
     	perror("error en el sendto"); 
        exit(EXIT_FAILURE); 
    
    } 
    
    
    //guardo el tamano de la direccion del servidor
    server_addr_len=sizeof(servaddr);
    
    //recvfrom recibo mensajes del socket.
    //paso el file descriptor del socket, puntero al bufferRecibirServidor donde almaceno el mensaje
    //tamano del bufferRecibirServidor
    //la bandera waitall es para que se bloquee hasta que toda la peticion este completa
    //sockaddr donde esta la direccion del que me esta mandando datos
    //la longitud de esta direccion      
    n = recvfrom(sockfd, (char *)bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR,  MSG_WAITALL, (struct sockaddr *) &servaddr, &server_addr_len);
    if(n<0){
     	perror("error en el recvfrom"); 
        exit(EXIT_FAILURE); 
    } 
    bufferRecibirServidor[n] = '\0'; // recvfrom me devuelve la longitud del mensaje, pongo en el ultimo caracter como final
    printf("%s",bufferRecibirServidor); 
    
    
    
    if(close(sockfd)<0){//cierro el socket
    	perror("error en el close"); 
        exit(EXIT_FAILURE); 
    }
    
  
    return 0; 
}
