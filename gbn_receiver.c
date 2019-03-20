#include<sys/socket.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdio.h>
#define BUFFERSIZE 1024
#define PORT 5000

int main ()
{
	int sock ;
	int bind_status , bytes_received , bytes_sent, expected_seqnum=1,i, recv_seqnum ;
	char send_buf [ BUFFERSIZE ] , recv_buf [ BUFFERSIZE ],data[BUFFERSIZE];
	struct sockaddr_in s_server , s_client ;
	int si_len = sizeof ( s_client ) ;
	int flag=0,temp;
	// Creating the socket
	sock = socket ( AF_INET , SOCK_DGRAM , IPPROTO_UDP ) ;
	
	if ( sock < 0)
	{
		printf ( " Socket creation failed .\n " ) ;
	}

	else
	{
		printf ( " Socket created with descriptor %d \n " , sock ) ;
	}

	// Initializing and binding the socket
	s_server.sin_family = AF_INET ;
	s_server.sin_port = htons ( PORT ) ;
	s_server.sin_addr.s_addr = htonl ( INADDR_ANY ) ;
	bind_status = bind ( sock , ( struct sockaddr *) &s_server , \
	sizeof ( s_server ) ) ;

	if ( bind_status < 0)
	{
		printf ( " Binding socket failed .\n " ) ;
	}

	else
	{
		printf ( " Binding socket successful .\n " ) ;
	}

	while(1)
	{
		sleep(1);
		//  Wait for a packet
		bytes_received=recvfrom(sock,recv_buf,sizeof(recv_buf),\
		0,(struct sockaddr*)&s_client,&si_len);

		
		if (expected_seqnum > 10)
		{
			expected_seqnum = expected_seqnum - 10 ;
		}	

		/*if ((recv_buf[0]%3==0) &&(flag!=2))
		{
			flag=flag+1;
					
		}	
		else*/ if (recv_buf[0]==recv_buf[1])
		{		
			flag=0;			
			recv_buf[bytes_received]='\0';
			//recv_seqnum=atoi(recv_buf[0]);
			recv_seqnum=recv_buf[0];
			//process packet
			if(recv_seqnum==expected_seqnum)
			{
				for(i=0;recv_buf[i+1]!='\0';i++)
				data[i]=recv_buf[i+1];	
				data[i]='\0';	
				expected_seqnum=expected_seqnum+1;
				printf ( " %d bytes of data received : %s \n " , bytes_received , data ) ;
				//make ack
				//sprintf(&send_buf[0],"%d",recv_seqnum);
				send_buf[0]=(char *)recv_seqnum;
				send_buf[1]=send_buf[0];
				bytes_sent = sendto ( sock , send_buf , sizeof ( send_buf ) , 0 , \
				( struct sockaddr*) &s_client , si_len ) ;
				temp=(send_buf[0]);
				printf ( " %d bytes sent : %d \n " , bytes_sent , temp ) ;	
			}
	
			else
			{
				bytes_sent = sendto ( sock , send_buf , sizeof ( send_buf ) , 0 , \
				( struct sockaddr*) &s_client , si_len ) ;
				
				printf ( " %d bytes sent : %s \n " , bytes_sent , send_buf ) ;		
			}
		
		}
	

	}

	return 0;
}
