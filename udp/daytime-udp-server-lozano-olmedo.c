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
    

#define TAMANO_RECIBIR_CLIENTE 200
#define TAMANO_ENVIAR_CLIENTE 200 
#define TAMANO_OBTENER_FECHA 200 
#define TAMANO_OBTENER_HOST 200 
    

int main(int argc,char *argv[]) { 

    //defino las variables a utilizar
    int sockfd; 
    char bufferRecibirCliente[TAMANO_RECIBIR_CLIENTE]; 
    char bufferEnviarCliente[TAMANO_ENVIAR_CLIENTE];
    char bufferObtenerFecha[TAMANO_OBTENER_FECHA];
    char bufferObtenerHost[TAMANO_OBTENER_HOST];
    FILE *fich;
    struct sockaddr_in servaddr, cliaddr; 
    int puerto; 
    struct servent *servinfo; 
    int  n; 
    int ret;
    socklen_t cli_addr_len = 0;
    
    
    //creo el socket, con af_inet para ipv4, sock_dgram para udp, y 0 para el protocolo
      if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("error en la creacion del socket"); 
        exit(EXIT_FAILURE); 
    } 
        
    servaddr.sin_family    = AF_INET; // tipo de direcciones que usa el socket en este caso ipv4
    servaddr.sin_addr.s_addr = INADDR_ANY; //admito cualquier ip 
    
    
    
    
    
   //sin -p puerto
   if(argc==1){ 	
   	servinfo = getservbyname("daytime", "tcp");//busco el puerto que tiene el servicio de daytime con el protocolo udp
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
    		printf("se utiliza: ./daytime-server-lozano-olmedo -p puerto\n");
    		exit(1);
    	}
    		
   }else{
   	printf("se utiliza: ./daytime-server-lozano-olmedo -p puerto\n");
   	exit(1);
   }
    
    
     
   
        
   //con bind asigno direccion de servaddr al socket sockfd, ademas paso el tamano de la direccion en servaddr. 
   // es como asignar un nombre al socket
    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) 
    { 
        perror("error en el bind"); 
        exit(EXIT_FAILURE); 
    }
    
     
        
    
    //cojo el tamano de la estructura que guarda los datos del cliente
    cli_addr_len = sizeof(cliaddr);  
    //recvfrom recibo mensajes del socket.
    //paso el file descriptor del socket, puntero al bufferRecibirCliente donde almaceno el mensaje
    //tamano del bufferRecibirCliente
    //la bandera waitall es para que se bloquee hasta que toda la peticion este completa
    //sockaddr donde esta la direccion del que me esta mandando datos
    //la longitud de esta direccion
    //entro en bucle infinito
    while(1){
    	n = recvfrom(sockfd, (char *)bufferRecibirCliente, TAMANO_RECIBIR_CLIENTE,  MSG_WAITALL, ( struct sockaddr *) &cliaddr, &cli_addr_len); 
    	if(n<0){
      		perror("error en el recvfrom"); 
        	exit(EXIT_FAILURE); 
    	}
    	bufferRecibirCliente[n] = '\0'; // recvfrom me devuelve la longitud del mensaje, delimito final del buffer
    
    
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
        	perror("error en el gethostname");
        	exit(EXIT_FAILURE);
    	}
    
    
    	//lo pongo en el formato que se pide
    	strcpy(bufferEnviarCliente,bufferObtenerHost);
    	strcat(bufferEnviarCliente,": ");
    	strcat(bufferEnviarCliente,bufferObtenerFecha);
    
    
    
    
    	//sendto manda el mensaje por el socket
    	//paso el file descriptor del socket,
    	//puntero al bufferRecibirCliente donde esta el mensaje a enviar
    	//tamano del mensaje
    	//bandera msgconfirm para enviarle la respuesta a lo que me ha mandado el cliente
    	//puntero a sockaddr que tiene la direccion destino
    	//tamano del anterior sockaddr
    	if(sendto(sockfd, (const char *)bufferEnviarCliente, strlen(bufferEnviarCliente),  MSG_CONFIRM, (const struct sockaddr *) &cliaddr, cli_addr_len)<0){
    		perror("error en el sendto");
        	exit(EXIT_FAILURE);
    	}
    	//sendto devuelve el numero de bytes enviados
    
    
    	printf("he procesado un cliente\n");//mensaje de informacion
      } 
      
      // en el servidor no he cerrado el socket porque como los clientes siempre mandan el 
      //mismo mensaje, no puedo diferenciar cuando se quiere cerrar el socket
      // se podria hacer que cuando un cliente mande un mensaje vacio se cerrase el socket en el servidor 
    return 0; 
}
