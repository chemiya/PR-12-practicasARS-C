//Practica tema 7, Lozano Olmedo Jose Maria



/*
CONSIDERACIONES:
ultimos bugs corregidos:
en la escritura, si mandaba paquetes con mas de codigo de bloque 10 se paraba porque codigo con dos numeros no estaba bien puesto
en la lectura, escribia todo el buffer en el archivo donde guardaba lo leido, lo he corregido y la cabecera no se escribe

el principal obstaculo ha sido entender como se debian mandar los paquetes, es decir, los codigos de opcode y las cabeceras
en esto me ha ayudado usted, y se lo agradezco porque si no no hubiese podido avanzar


*/

/*

esquema general:

Escritura:
	mando peticion
	recibo ack
	bucle leo fichero a enviar
		envio paquete
		recibo ack

lectura.
	Mando peticion
	bucle recibir paquetes
		recibo pauqete
		mando ack
		si tamano! cierro



*/

/*
en el primer sendto y primer recvfrom le pongo comentado su funcionamiento, que es lo mismo que la primera practica
en los demas sendto y recvfrom ya no pongo comentarios de su funcionamiento porque seria lo mismo siempre

*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define TAMANO_RECIBIR_SERVIDOR 600
#define TAMANO_ENVIAR_SERVIDOR 600
#define TAMANO_DATOS 512

int main(int argc, char *argv[]) {
	
	//variables que utilizo
	int sockfd;
	char bufferRecibirServidor[TAMANO_RECIBIR_SERVIDOR]; 
	char bufferEnviarServidor[TAMANO_ENVIAR_SERVIDOR];
	struct sockaddr_in servaddr;
	struct servent *servinfo; 
	struct in_addr addr;
	int n;
	socklen_t server_addr_len = 0;
	int opcion=0;//0 para lectura, 1 para escritura
	char filename [100]; 
	int depuracion=0;//0 sin mostrar avances comunicacion, 1 mostrandolos
	FILE *fichero;
	FILE *resultadoLeido;
	char *resultadoLeidoFichero="resultadoLeido.txt";//fichero donde guardo lo que lea
	char caracter;
	int numero;
	int cierre=1;
	int leidos;
	int contador=1;
	int seguir=1;	
	char bufferDatosEnvio[TAMANO_DATOS];
	char bufferSoloDatos[TAMANO_DATOS];
	int indice;
	




 	//creo el socket, con af_inet para ipv4, sock_dgram para udp, y 0 para el protocolo
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("error en la creacion del socket"); 
		exit(EXIT_FAILURE);
	}
	
	
	
	
	
	

	
	
	if(argc>=4){
		//convierto la direccion a ip de argumento a un numero en network byte order
		if (inet_aton(argv[1], &addr) ==0) { 
			perror("error en el inet_aton"); 
			exit(EXIT_FAILURE);
		}
		//opcion de lectura o escritura
		if(strcmp("-w",argv[2])==0){ 
			opcion=1;
		}
		//nombre del fichero con el que voy a realizar las operaciones
		strcpy(filename, argv[3]);

		//si se quiere mostrar mensajes en la comunicacion
		if(argc==5){
			if(strcmp("-v",argv[4])==0){
				depuracion=1;
			}else{
				printf("se utiliza: tftp-client ip-servidor {-r|-w} archivo [-v]");
				exit(EXIT_FAILURE);
			}
		}

	}

	else{
		printf("se utiliza: tftp-client ip-servidor {-r|-w} archivo [-v]"); 
		exit(EXIT_FAILURE);
	}
	
	
	
	
	
	
	
	
	
	
	
	//busco el puerto que tiene el servicio de tftp con el protocolo udp
	servinfo = getservbyname("tftp", "udp");
	if(!servinfo) {
		
		fprintf(stderr, "no he encontrado el servicio daytime\n");
		exit(1);


	}
	
	//asigno el puerto del tftp al servidor 
	servaddr.sin_port = htons(ntohs (servinfo -> s_port));
	 // tipo de direcciones que usa el socket en este caso ipv4
	servaddr.sin_family = AF_INET; 
	//pongo la ip que me pasa de argumento
	inet_aton(argv[1], &servaddr.sin_addr);
	//tamano de la estructura que guarda la direccion del servidor
	server_addr_len=sizeof(servaddr);
	
	
	
	
	
	
	
	
	
	


	//preparo la peticion
	
	//primer caracter es un 0
	numero =0;
	caracter=numero;
	bufferEnviarServidor[0]=caracter;
	
	//segundo caracter depende de si la peticion es de lectura/escritura
	if(opcion==0){
		numero=1;
	}else{
		numero=2;
	}
	caracter=numero;
	bufferEnviarServidor[1]=caracter;
	
	
	//despues pongo el nombre del fichero que voy a utilizar
	for (indice = 0; indice < strlen(filename); indice++)
	{

		char actual = filename [indice];
		bufferEnviarServidor[indice+2]=actual;
	}

	//pongo otro 0
	numero =0;
	caracter=numero;
	bufferEnviarServidor[indice+2]=caracter;
	
	//pongo el modo
	bufferEnviarServidor[indice+3]='o';
	bufferEnviarServidor[indice+4]='c';
	bufferEnviarServidor[indice+5]='t';
	bufferEnviarServidor[indice+6]= 'e';
	bufferEnviarServidor[indice+7]='t';
	
	//pongo el 0 final
	numero =0;
	caracter=numero;
	bufferEnviarServidor[indice+8]=caracter;


	



	//mando la peticion

	//sendto manda el mensaje por el socket
    //paso el file descriptor del socket,
    //puntero al bufferEnviarServidor donde esta el mensaje a enviar
    //tamano del mensaje
    //bandera msgconfirm para enviarle la respuesta a lo que me ha mandado el cliente
    //puntero a sockaddr que tiene la direccion destino
    //tamano del anterior sockaddr 
	n=sendto(sockfd, (const char *)bufferEnviarServidor, strlen(filename)+9, MSG_CONFIRM, (const struct sockaddr*)&servaddr,sizeof(servaddr));

	//si estoy en modo de depuracion muestro mensaje
	if(depuracion==1){
		if(opcion==0){
			printf("enviada solicitud de lectura de %s a servidor tftp en %s\n",filename,argv[1]);
		
		}else{
			printf("enviada solicitud de escritura de %s a servidor tftp en %s\n",filename,argv[1]);
		
		}
	
	}
	
	//comprobacion sendto se ha hecho correcto
	if(n<0) {
		perror("error en el sendto");
		exit(EXIT_FAILURE);

	}
	
	
	
	
	
	
	
	
	
	
	
	
	//modo lectura
	if(opcion==0){
		//abro el fichero donde almaceno lo que voy a leer del servidor
		resultadoLeido=fopen(resultadoLeidoFichero,"w");
		 if(resultadoLeido==NULL){ 
			perror("error al abrir para guardar lo leido");
			exit(EXIT_FAILURE);
		}
		
		
		
		
		
		
		
		
		//entro en bucle para recibir paquetes
		while(cierre){



		//recibo paquete del servidor

		//recvfrom recibo mensajes del socket.
    		//paso el file descriptor del socket, puntero al bufferRecibirServidor donde almaceno el mensaje
    		//tamano del bufferRecibirServidor
    		//la bandera waitall es para que se bloquee hasta que toda la peticion este completa
    		//sockaddr donde esta la direccion del que me esta mandando datos
    		//la longitud de esta direccion   
			n = recvfrom(sockfd, (char *)bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR, MSG_WAITALL, (struct sockaddr *)&servaddr, &server_addr_len);
			
			//compruebo tamano de lo recibido
			if(n<0){
				perror("error en el recvfrom");
				exit(EXIT_FAILURE);
			}
			
			
			//si estoy en dupracion muestro mensaje
			if(depuracion==1){
				printf("recibido bloque del servidor tftp. he recibido %d bytes y corresponde al bloque %d%d\n ",n,bufferRecibirServidor[2],bufferRecibirServidor[3]);
			
			}
			
			
			//compruebo si el servidor me manda error
			if(bufferRecibirServidor[1]==5){
				printf("error 40x en el servidor\n");
				for(indice=4;indice<n;indice++){
					printf("%c",bufferRecibirServidor[indice]);
				
				}
				printf("\n");
				exit(EXIT_FAILURE);
			
			}
			
			
			//del paquete recibido cojo solo la parte de los datos, es decir quito los 4 primeros numeros de la cabecera
			for(indice=4;indice<n;indice++){
				char actual=bufferRecibirServidor[indice];
				bufferSoloDatos[indice-4]=actual;
			
			}
			
			//escribo solo los datos del paquete, que tienen un tamano de los bytes recibidos-4 bytes de cabecera
			//en el fichero donde guardo lo que lea
			fwrite(bufferSoloDatos,1,n-4,resultadoLeido);
			
			//si los bytes recibidos son menos de los 512 de datos mas 4 de cabecera, sera ultimo paquete
			//terminaria bucle y cierro fichero
			if(n<516){
				cierre=0;
				if(depuracion==1){				
					printf("este bloque era el ultimo. cierro el fichero\n");
				}
				
				fclose(resultadoLeido);
			
			}
		







			//para limpiar el contenido del buffer
			// he tenido casos donde por ejemplo la primera vez escribes en todo el buffer y despues la segunda vez
			//escribes la mitad del buffer, entonces la mitad final como no la modificas se queda lo de la primera vez
			//el buffer, lo pongo con 0, todo su tamano
			memset(bufferEnviarServidor,0,TAMANO_ENVIAR_SERVIDOR);
			
			//ahora mandaria el ack del paquete recibido
			//primero pongo un 0
			numero=0;
			caracter=numero;
			
			bufferEnviarServidor[0]=caracter;

			//despues pongo un 4
			numero=4;
			caracter=numero;
			bufferEnviarServidor[1]=caracter;
			//despues el numero de bloque que ha llegado en el paquete
			bufferEnviarServidor[2]=bufferRecibirServidor[2]; 		
			bufferEnviarServidor[3]=bufferRecibirServidor[3];
		


			//le mando el paquete
			
			n=sendto(sockfd, (const char *) bufferEnviarServidor,4,MSG_CONFIRM,(const struct sockaddr *)&servaddr, sizeof(servaddr));
			
			//mensaje de depuracion
			if(depuracion==1){
				printf("enviado ack del bloque %d%d. este ack tenia %d bytes\n ",bufferRecibirServidor[2],bufferRecibirServidor[3],n);
			
			}
			

			//comprobacion cuantos bytes ha enviado
			if(n<0){
				perror("error en el sendto");
				exit(EXIT_FAILURE);
			}
			
			



	}//while cierre
	}//if opcion =0
	




	//modo escritura
	else{


		//recibo el primer ack si mando la peticion
		n = recvfrom(sockfd, (char *)bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR,MSG_WAITALL,(struct sockaddr *) &servaddr,
	&server_addr_len);
	
		// si depuracion muestro el mensaje de informacion
		if(depuracion==1){
			printf("recibido bloque del servidor tftp correspondiente al ack inicial de escritura. he recibido %d bytes y corresponde al bloque %d%d\n ",n,bufferRecibirServidor[2],bufferRecibirServidor[3]);
		
		}
		
		//comprobacion cuantos bytes he recibido
		if(n<0) {
			perror("error en el recvfrom");
			exit(EXIT_FAILURE);

		}
		
		
		//comprobacion si me manda error el servidor
		if(bufferRecibirServidor[1]==5){
				printf("error 40x en el servidor\n");
				for(indice=4;indice<n;indice++){
					printf("%c",bufferRecibirServidor[indice]);
				
				}
				printf("\n");
				exit(EXIT_FAILURE);
			
			}

		
		//abro el fichero que voy a mandar para que lo escriba el servidor
		fichero=fopen(filename, "r");
		if(fichero==NULL) {
			perror("error al abrir");
			exit(1);
		}
		
		
		
		
		
	



		//entro en bucle para enviar los paquetes del fichero a escribir
		while(seguir){
		
		
			//pongo a 0 el buffer donde prepara los datos que leo del fichero para el envio
			//aqui tuve un problema y el nombre del fichero se guardaba detras, y me hacia que los datos fuesen
			//mas de 512 bytes lo que no se puede. por eso tengo que poner a 0 un poco mas del tamano del buffer
			memset(bufferDatosEnvio,0,TAMANO_DATOS+20); 
			
			//leo del fichero los datos y los guardo en el buffer			
			leidos=fread(bufferDatosEnvio,sizeof *bufferDatosEnvio,TAMANO_DATOS, fichero);


			//si el tamano de lo que leo es menor de 512, entonces he llegado al final, saldria del bucle
			//ciero el fichero y muestro mensaje si es necesario
			if(leidos!=TAMANO_DATOS){
				seguir=0;
				if(depuracion==1){
				printf("este sera al ultimo bloque enviado\n");
				}
				
				fclose(fichero);
				
			}
			
			//limpio el buffer donde envio
			memset(bufferEnviarServidor,0,TAMANO_ENVIAR_SERVIDOR);
			//primero pongo un 0
			numero=0;
			caracter=numero;
			bufferEnviarServidor[0]=caracter;
			//despues pongo un 3
			numero=3;
			caracter=numero;
			bufferEnviarServidor[1]=caracter;
		
		
				//pongo 0 para el numero de bloque
				numero=0;
				caracter=numero;
				bufferEnviarServidor[2]=caracter;
				//pongo el numero de bloque en el que llego
				numero=contador;
				caracter=numero; 
				bufferEnviarServidor[3]=caracter;
			
			//despues pongo los datos a enviar en el buffer que enviare
			for(indice=0; indice<strlen(bufferDatosEnvio); indice++){
				char caracter=bufferDatosEnvio[indice]; 				
				bufferEnviarServidor[indice+4]=caracter;
			}


			//le mando el paquete
			n=sendto(sockfd, (const char *)bufferEnviarServidor, strlen(bufferDatosEnvio)+4, MSG_CONFIRM, (const struct sockaddr *)&servaddr,sizeof(servaddr));
			
			//mensaje de depuracion
			if(depuracion==1){
				printf("enviado el bloque  %d%d. este bloque tenia %d bytes\n ",bufferEnviarServidor[2],bufferEnviarServidor[3],n);
			
			
			}
			
			//comprobacion si ha mandado los bytes
			if(n<0){
				perror("error en el sendto"); 
				exit(EXIT_FAILURE);
			}
			
			
			
			
			
			
			
			
			
			


			//recibo ack del paquete mandado
			n = recvfrom(sockfd, (char *)bufferRecibirServidor, TAMANO_RECIBIR_SERVIDOR, MSG_WAITALL, (struct sockaddr *)&servaddr,&server_addr_len);

			//muestro mensaje
			if(depuracion==1){
				printf("recibido ack del servidor tftp de tamano %d correspondiente al bloque %d%d\n ",n,bufferRecibirServidor[2],bufferRecibirServidor[3]);
			}

			

			//comprobacion cuantos bytes he recibido
			if(n<0){
				perror("error en el recvfrom");
				exit(EXIT_FAILURE);
			}
			
			//si recibo error del servidor, salgo de la lectura
			if(bufferRecibirServidor[1]==5){
				printf("error 40x en el servidor\n");
				for(indice=4;indice<n;indice++){
					printf("%c",bufferRecibirServidor[indice]);
				
				}
				printf("\n");
				exit(EXIT_FAILURE);
			
			}

			

		//incremento el contador del bloque
		contador++;
}//while seguir
 }//modo escritura else
 
 		//cierro el socket
		if(close(sockfd)<0){
		perror("error al close");
		exit(EXIT_FAILURE);
		}
		
		return 0;
}






