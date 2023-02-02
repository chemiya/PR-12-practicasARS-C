//Practica tema 8, Lozano Olmedo Jose Maria

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include "ip-icmp-ping.h"

unsigned short checksum(void *b, int numShorts)
{ 

	//he encontrado esta forma que funciona
	//la he adaptado a los nombres que aparecen en las diapositivas
	//he tratado de modificarla para que siga los pasos de las diapositivas pero no he conseguido que funcionase
	//por lo tanto esta forma, funciona y sigue en medida de lo posible los pasos de las diapositivas
	//supongo que habra muchas soluciones diferentes para realizar el checksum
	
	
	//numShorts tiene el tamano en bytes del datagrama
	//no lo divido entre dos porque lo voy decrementando de 2 en 2 
	

	unsigned short *puntero = b;
	unsigned int acumulador=0;
	unsigned short resultadoFinal;


	//recorro el datagrama saltando de 2 en 2 segun numshorts, guardo en el acumulador e incremento puntero
	for ( acumulador = 0; numShorts > 1; numShorts-= 2 ){
		acumulador += *puntero++;
		}
	
	//guardo en caso que quede uno suelto por incrementar de 2 en 2	
	if ( numShorts == 1 ){
		acumulador += *(unsigned char*)puntero;
		}
		
	//sumar la parte alta del acumulador a la parte baja
	acumulador = (acumulador >> 16) + (acumulador & 0xFFFF);
	
	//esto hay que hacerlo dos veces
	acumulador += (acumulador >> 16);
	
	//not logico
	resultadoFinal = ~acumulador;
	return resultadoFinal;
}




void descripcionRespuesta(int type, int code){
	//he cogido los tipos y codigo s de error de la wikipedia en ingles sobre icmp como pone en el enunciado
	//me respondio por correo que solo habia que poner un par de switch para codigo y tipo
	//he tratado de poner los que no estan deprecated y un poco de variedad
	//si hubiese que completar con todos los codigos y tipo simplemente seria anadir case en el witch que corresponda
	//donde almacenamos la descripcion del error
	char error[100];
	
	
	//switch por tipo
	switch(type){
	
	
		//---------------------------------
		case 0:
			switch(code){
				case 0:
					strcpy(error,"respuesta correcta");
					break;

			
				}//swicth codigo
			break;
			
		//fin caso 0
			
				
	
		//--------------------------------------
		case 3:
			strcpy(error,"destination ureachable: ");
			switch(code){
				case 13:
					strcat(error,"Communication administratively prohibited");
					break;
					
				case 9:
					strcat(error,"Network administratively prohibited");
					break;
					
				case 2:
					strcat(error,"Destination protocol unreachable");
					break;
					
				case 3:
					strcat(error,"Destination port unreachable");
					break;
				default:
					strcpy(error,"codigo incorrecto");
					break;
					
				}//switch codigo
			break;
			
		//fin caso 3	
		
		
		
		
		//----------------------------------------	
		case 5:
			strcpy(error,"Redirect Message: ");
			switch(code){
				case 0:
					strcat(error,"Redirect Datagram for the Network");
					break;
					
				case 1:
					strcat(error,"Redirect Datagram for the Host");
					break;
				default:
					strcpy(error,"codigo incorrecto");
					break;
					
				}//switch codigo
			break;
			
		//fin caso 5
		
		
		
		//---------------------------------------
		case 11:
			strcpy(error,"Time Exceeded: ");
			switch(code){
				case 0:
					strcat(error,"TTL expired in transit");
					break;
					
				case 1:
					strcat(error,"Fragment reassembly time exceeded");
					break;
				default:
					strcpy(error,"codigo incorrecto");
					break;
					
				}//switch codigo
			break;
			
		//fin caso 11
		
		
		//---------------------------------
		case 12:
			strcpy(error,"Parameter Problem: Bad IP header: ");
			switch(code){
				case 0:
					strcat(error,"Pointer indicates the error");
					break;
					
				case 1:
					strcat(error,"Missing a required option");
					break;
				default:
					strcpy(error,"codigo incorrecto");
					break;
					
				}//switch codigo
			break;
		//fin caso 12
		
		
		//--------------------------------------
		case 43:
			strcpy(error,"Extended Echo Reply: ");
			switch(code){
				case 2:
					strcat(error,"No Such Interface");
					break;
					
				case 1:
					strcat(error,"Malformed Query");
					break;
				default:
					strcpy(error,"codigo incorrecto");
					break;
					
				}//switch codigo
			break;
		//fin caso 43
		
		
		//-------------------------------------------	
		default:
			strcpy(error,"tipo incorrecto");
			break;
			
	
	
	
	}//switch tipo
	
	
	//imprimios tipo codigo y descripcion error
	printf("Descripcion de la respuesta: %s  (Type: %d , code: %d)\n",error,type, code);
	
	





}//descripcionRespuesta








int main(int argc, char *argv[])
{

	//variables------------------------------------
	int sockfd;
	struct sockaddr_in     servaddr;
 	struct in_addr addr;
 	int puerto=0;
 	ECHORequest echoRequest;
	ECHOResponse echoResponse;
	socklen_t addr_len;	
	int i,n;
	int depuracion=0;//0 no depuracion, 1 si depuracion
 
 
 


	//abro socket--------------------------------------------
	//af_inet para ipv4
	//sock_raw utiliza un protocolo de red directo, aqui se ponia tcp o udp
	//IPPROTO_ICMP para utilizar paquetes icmp
	sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if(sockfd<0)
	{
		perror("error en la creacion del socket"); 
		exit(EXIT_FAILURE);
	}
	
	
	
	
	
	//control numero argumentos-----------------------------------
	if(argc==2 || argc==3){
	
	
	
	
		//proceso la direccion ip-------------------------------------------
		//convierto la direccion a ip de argumento a un numero en network byte order
		
		if (inet_aton(argv[1], &addr) == 0) {
			perror("error en el inet_aton");
			exit(EXIT_FAILURE);
	   	}
	    
	    	
	    	
	    	
	    	//argumento para si hay que depurar---------------------------------------
	    	if(argc==3){
	    		
	    		if(strcmp(argv[2],"-v")==0){
	    			depuracion=1;
	    			
	    		
	    		
	    		}else{
	    			printf("./miping direccionIP [-v]\n");
	    			exit(EXIT_FAILURE);
	    		
	    		}
	    		
	    	
	    	}
	    	
    	}//argc==2 o 3
    	
    	else{
    		printf("./miping direccionIP [-v]\n");
	    	exit(EXIT_FAILURE);
    	
    	}
    	
    	
    	
    	
    	
    	
    	//argumentos del servidor------------------------------------
    	// tipo de direcciones que usa el socket en este caso ipv4
    	servaddr.sin_family = AF_INET; 
    	
    	//pongo la ip que me pasa de argumento
    	inet_aton(argv[1], &servaddr.sin_addr);
    	
    	//asigno el puerto de la comunicacion
    	servaddr.sin_port = htons(puerto);
    	
    	//tamano de la estructura que guarda la direccion del servidor
    	addr_len=sizeof(servaddr);
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	//preparo paquete icmp--------------------------------------
    	
    	
    	
    	if(depuracion){
    		printf("->Generando cabecera ICMP\n");
    	}
    	
    	
    	//poner a 0
		
	bzero(&echoRequest, sizeof(echoRequest));
		
		
	//type
	echoRequest.icmpHeader.Type=ICMP_ECHO;
		
	if(depuracion){
    		printf("->Type: %d\n",echoRequest.icmpHeader.Type);
    	}
	
		
		
		
	//code
	echoRequest.icmpHeader.Code=0;
		
	if(depuracion){
    		printf("->Code: %d\n",echoRequest.icmpHeader.Code);
    	}
	
		
		
	//pid
	echoRequest.ID=getpid();
		
	if(depuracion){
    		printf("->Identifier(pid): %d\n",echoRequest.ID);
    	}
		
		
		
	//Numero secuencia
		
	echoRequest.SeqNumber=0;
		
	if(depuracion){
    		printf("->SeqNumber: %d\n",echoRequest.SeqNumber);
    	}
		
		
		
	//contenido
	
	for ( i = 0; i < sizeof(echoRequest.payload)-1; i++ ){
		echoRequest.payload[i] = i+'0';
	}
		
	echoRequest.payload[i]=0;
		
		
	if(depuracion){
    		printf("->Cadena a enviar: %s\n",echoRequest.payload);
    	}
		
		
		
	//checksum
	
	echoRequest.icmpHeader.Checksum=checksum(&echoRequest, sizeof(echoRequest));
		
		
		
	if(depuracion){
    		printf("->Checksum: 0x%x\n",echoRequest.icmpHeader.Checksum);
    	}
		
		
		
		
		
		
		
		
		
	//mando paquete------------------------------------
	if(depuracion){
    		printf("->Tamano del paquete ICMP enviado: %ld\n",sizeof(echoRequest));
    	}
    	
    	
    	
    //sendto manda el mensaje por el socket
    //paso el file descriptor del socket,
    //puntero al echoRequest donde esta el paquete icmp
    //tamano del mensaje: 64 bytes del payload +8 de los demas campos
    //bandera 0
    //puntero a sockaddr que tiene la direccion destino
    //tamano del anterior sockaddr 
  
	n= sendto(sockfd, &echoRequest, sizeof(echoRequest), 0,(const struct sockaddr *)&servaddr,sizeof(servaddr));
		
		
    	printf("Paquete ICMP enviado a %s en el sendto con tamano: %d\n",argv[1],n);
    	
		
		
	if ( n< 0){
			perror("error en el sendto");
			exit(EXIT_FAILURE);
			
	}
		
		
		
		
		
		
		
		
		
		
	//recibo respuesta-------------------------------------------------
	//recvfrom recibo mensajes del socket.
    	//paso el file descriptor del socket, puntero al echoResponse donde almaceno la respuesta
    	//tamano del echoResponse
    	//bandera 0
    	//servaaddr donde esta la direccion del que me esta mandando datos
    	//la longitud de esta direccion   
	n=recvfrom(sockfd, &echoResponse, sizeof(echoResponse), 0,(struct sockaddr*)&servaddr, &addr_len);
	
	if (n  < 0){
			perror("error en el recvfrom");
			exit(EXIT_FAILURE);
	}
		
		
		
		
	//proceso respuesta--------------------------

	
	//muestro los campos de la respuesta recibida
	printf("Recibida respuesta de %s\n",argv[1]);
	if(depuracion){
    		
	    	printf("->Tamano de la respuesta: %d\n",n);
	    	printf("->Cadena recibida: %s\n",echoResponse.payload);
	    	printf("->Identifier(pid): %d\n",echoResponse.ID);
	    	printf("->TTL: %d\n",echoResponse.ipHeader.TTL);
    	}
    	
		
			
	//mostrar la respuesta. llamo a funcion que hace switch para el tipo y codigo ya sea correcta o erronea
	descripcionRespuesta(echoResponse.icmpHeader.Type,echoResponse.icmpHeader.Code);
				
			
	
	
		
		
	
	
	
	//cierro el socket
	if(close(sockfd)<0){
		perror("error al close");
		exit(EXIT_FAILURE);
	}
	
	
	//eliminar warning
	return 0;
	


}//main



