#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include <time.h>
#include <pthread.h>
#include <sys/poll.h>
#define BUFFERSIZE 1024
#define SERVERADDR "127.0.0.1"

pthread_t tids [3];

struct sockaddr_in s_server,s_client ;
int sock,bytes_received, si_len = sizeof(s_server),bytes_sent;
char recv_buf[BUFFERSIZE],send_buf[BUFFERSIZE],seqno='1',data[BUFFERSIZE],packet[40][BUFFERSIZE];
int seqnum=1,seqnotill=1,base=1,N=4,ackno=0,status, rv, bind_status ;
time_t x,start;
struct pollfd ufds[1];


void* rdt_send(void* args)
{
char chksum,temp[BUFFERSIZE];
int i;

while(1)
{	
	printf(" Message to send : " ) ;
	scanf("%s",data) ;
	
	if(seqnum<base+N)
	{	
		strcpy(temp,data);
		chksum=(char *)seqnum;
		for(i=0;temp[i]!='\0';i++)
		data[i+2]=temp[i];
		data[0]=(char *)seqnum;
		data[1]=chksum;
		data[i+2]='\0';
		strcpy(packet[seqnum],data);		
		strcpy(send_buf,packet[seqnum]);

		bytes_sent = sendto(sock,send_buf,sizeof(send_buf),0,\
		( struct sockaddr*)&s_server,si_len);
		printf ( " %d bytes sent : %s \n " , bytes_sent , temp ) ;


		
		seqnum=seqnum+1;
		if(seqnum>10)
		seqnum=seqnum-10;
		if(seqnum==base)
		start=time(NULL);	


		
	}
	else
	{
		printf("\n Window full\n Retry later");
	}


}

}

void* timeout(void* args)
{
int ackno;
ufds[0].fd=sock;
ufds[0].events=POLLIN;
while(1)
{

	if((time(NULL)-start)>=3)
	{
		seqnotill=seqnum;
		seqnum=base;
		while(seqnum<seqnotill)
		{	
			strcpy(send_buf,packet[seqnum]);
			bytes_sent = sendto(sock,send_buf,sizeof(send_buf),0,\
			( struct sockaddr*)&s_server,si_len);
			seqnum++;
			if(seqnum>10)
			seqnum=seqnum-10;
			printf ( " %d bytes sent : %s \n " , bytes_sent , packet[seqnum] ) ;

		}
	start=time(NULL);
	}
}
}


void* rdt_rcv(void* args)
{
while(1)
{

	rv = poll(&ufds,1,100);

	if(rv == -1)
	perror("poll");
	else if (rv==0);

	else 
	{//printf("receiving");
	bytes_received = recvfrom ( sock , recv_buf , sizeof ( recv_buf ) , \
	0 , ( struct sockaddr*)&s_server , &si_len ) ;
	if(recv_buf[0]==recv_buf[1])
	{
		ackno=(recv_buf[0]);
		base=ackno+1;
		if(base!=seqnum)
		start=time(NULL);
		if(base>10)
		{
			base=base-10;
		}
		printf ( "\n %d bytes received : %d \n " , bytes_received , ackno ) ;
	}
	}
}
}





int main()
{
// Creating the client socket
sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);

if(sock<0)
{
	printf("Socket creation failed .\n " ) ;
	return 1;
}
else
{
	printf ( " Socket creation successful at %d \n " , sock ) ;
}

// Assigning server IP and port for sending
s_server.sin_family = AF_INET ;
s_server.sin_port = htons (5000) ;

if(!inet_aton(SERVERADDR,&s_server.sin_addr))
{
	printf( " IP network format conversion failed.\n " ) ;
	return 1;
}

else
{
	printf("IP network format conversion successful .\n" ) ;
}

// Initializing and binding the socket
s_client.sin_family = AF_INET ;
s_client.sin_port = htons ( 6000 ) ;
s_client.sin_addr.s_addr = htonl ( INADDR_ANY ) ;
bind_status = bind ( sock , ( struct sockaddr *) &s_client , \
sizeof ( s_client ) ) ;

if ( bind_status < 0)
{
	printf ( " Binding socket failed .\n " ) ;
}

else
{
	printf ( " Binding socket successful .\n " ) ;
}


status = pthread_create(&tids[0],NULL,&rdt_send,(void *)&sock) ;

status = pthread_create(&tids[1],NULL,&timeout,(void *)&sock) ;

status = pthread_create(&tids[2],NULL,&rdt_rcv,(void *)&sock) ;
// Waiting for threads to complete
pthread_join ( tids [0] , NULL ) ;
pthread_join ( tids [1] , NULL ) ;
pthread_join ( tids [2] , NULL ) ;

return 0;

}
