//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
	
#define TRUE 1
#define FALSE 0
#define PORT 4008

int k,g=0;

struct client_data{
	int socket;
	int id;
};

struct group{
	int group_id;
	int members[10];
	int count;
	int active;
	int admin[5];
	int type;
	int pending;
	int status;
};


int find_client_id(struct client_data client_socket[],int sd){
	printf("finding client id of %d\n",sd);
	for(int i=0;i<k;i++){
		if(client_socket[i].socket == sd){
			printf("finding client id of %d\n",client_socket[i].id);

			return client_socket[i].id;
		}
	}
	return -1;
}


int is_group_admin(struct group groups[],int g,int id){
	if(id<=0)
		return -1;

	for(int i=0;i<5;i++){
		printf("%d\n",groups[g].admin[i]);
		if(id==groups[g].admin[i])
			return 1;
	}
	return -1;
}

int make_group_admin(struct group groups[],int g,int client_id){
	printf("check admin for %d",client_id);
	if(client_id<=0)
		return -1;

	for(int i=0;i<5;i++){
		if(groups[g].admin[i] == '\0' || groups[g].admin[i]==0 ){
			groups[g].admin[i] = client_id;
		}
	}

	return 1;
}

int remove_group_member(struct group groups[],int g,int client_id){
	printf("check membership for %d\n",client_id);
	if(client_id<=0)
		return -1;

	for(int i=0;i<5;i++){
		if(groups[g].admin[i] == client_id ){
			groups[g].admin[i] = 0;
		}
	}
	for(int i=0;i<10;i++){
		if(groups[g].members[i] == client_id ){
			printf("ffffff\n");
			groups[g].members[i] = 0;
			break;
			// groups[g].count-=1;
		}
	}
	return 1;
}


int remove_member(struct group groups[],int client_id){
	printf("remove member %d\n",client_id);
	if(client_id<=0)
		return -1;
	for(int h=0;h<g;h++){
		for(int i=0;i<5;i++){
			if(groups[h].admin[i] == client_id ){
				groups[h].admin[i] = 0;
			}
		}
		for(int i=0;i<10;i++){
			if(groups[h].members[i] == client_id ){
				groups[h].members[i] = 0;
				// groups[g].count-=1;
			}
		}
	}
	return 1;
}

int find_client_socket(struct client_data client_socket[],int id){
	for(int i=0;i<k;i++)
		if(client_socket[i].id == id)
			return client_socket[i].socket;
	
	return -1;
}

int find_group_no(struct group groups[],int g,int id){
	for(int i=0;i<g;i++){
		if(groups[i].group_id == id)
			return i;
	}
	return -1;
}

int get_token(char insert_data[10][1024],char buffer[1024]){
		char *pch;
		int m=0;
		char string[1024];
		memset(string,'\0',strlen(string));
		strncpy(string, buffer, strlen(buffer));
		printf("get token inp: %s\n",string);
		pch = strtok(string," ");
		while (pch != NULL)
		{
			memset(insert_data[m],'\0',strlen(insert_data[m]));
			strcpy(insert_data[m], pch);
			insert_data[m][strcspn(insert_data[m],"\n")]='\0';
			printf("yooo %s\n",insert_data[m]);
			m+=1;
			pch = strtok(NULL, " ");
		}
		return m;
}
int main(int argc , char *argv[])
{

	struct client_data client_socket[30];
	struct group groups[10];
	
	int opt = TRUE;
	int master_socket , addrlen , new_socket ,
		max_clients = 30 , activity, i , valread , sd;
	int max_sd;
	struct sockaddr_in address;
		
	char buffer[1024]; //data buffer of 1K
		
	//set of socket descriptors
	fd_set readfds;
		
	//a message
	char *message = "SUCCESSFUL\r\n";
	char *message2 = "UNSUCCESSFUL\r\n";
	char *message3 = "client has disconected\r\n";
	char *message4 = "group is full\r\n";
	char *message5 = "group joined !!\r\n";
	char *message8 = "group invite declined !!\r\n";
	char *message6 = "you have been made admin of group : ";
	char *message7 = "you have been removed from group : ";
	char *err_message1 = "you are not an admin\r\n";
	char *err_message2 = "group is not active\r\n";
	//initialise all client_socket[] to 0 so not checked
	for (i = 0; i < max_clients; i++)
	{
		client_socket[i].id = 0;
		client_socket[i].socket=0;
	}
		
	//create a master socket
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
	//set master socket to allow multiple connections ,
	//this is just a good habit, it will work without this
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0 )
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	//type of socket created
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( atoi(argv[1]) );
		
	//bind the socket to localhost port 8888
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	printf("Listener on port %d \n", atoi(argv[1]));
		
	//try to specify maximum of 3 pending connections for the master socket
	if (listen(master_socket, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
		
	//accept the incoming connection
	addrlen = sizeof(address);
	puts("Waiting for connections ...");
	

	while(TRUE)
	{
		//clear the socket set
		FD_ZERO(&readfds);
	
		//add master socket to set
		FD_SET(master_socket, &readfds);
		max_sd = master_socket;
			
		//add child sockets to set
		for ( i = 0 ; i < max_clients ; i++)
		{
			//socket descriptor
			sd = client_socket[i].socket;
				
			//if valid socket descriptor then add to read list
			if(sd > 0)
				FD_SET( sd , &readfds);
				
			//highest file descriptor number, need it for the select function
			if(sd > max_sd)
				max_sd = sd;
		}
	
		//wait for an activity on one of the sockets , timeout is NULL ,
		//so wait indefinitely
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
	
		if ((activity < 0) && (errno!=EINTR))
		{
			printf("select error");
		}
			
		//If something happened on the master socket ,
		//then its an incoming connection
		if (FD_ISSET(master_socket, &readfds))
		{	k+=1;
			if ((new_socket = accept(master_socket,
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
			{
				perror("accept");
				exit(EXIT_FAILURE);
			}
			
			//inform user of socket number - used in send and receive commands
			printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
				(address.sin_port));
		
			//send new connection greeting message
			if(k<10){
				if( send(new_socket, message, strlen(message), 0) != strlen(message) ){
					perror("send");
				}
			}
			else{
					if( send(new_socket, message2, strlen(message2), 0) != strlen(message2) )
						perror("send");
				}
			puts("Welcome message sent successfully");
				
			//add new socket to array of sockets
			for (i = 0; i < max_clients; i++)
			{
				//if position is empty
				if( client_socket[i].id == 0 )
				{
					client_socket[i].socket = new_socket;
					client_socket[i].id = rand()%99999;
					printf("Adding to list of sockets as %d\n" , i);
						
					break;
				}
			}
		}
			
		//else its some IO operation on some other socket
		for (i = 0; i < max_clients; i++)
		{
			sd = client_socket[i].socket;
				
			if (FD_ISSET( sd , &readfds))
			{
				//Check if it was for closing , and also read the
				//incoming message
				bzero(buffer,sizeof(buffer));
				
				valread = read( sd , buffer, 1024);
				if (valread == 0)
				{
					k-=1;
					//Somebody disconnected , get his details and print
					getpeername(sd , (struct sockaddr*)&address , \
						(socklen_t*)&addrlen);
					printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						
					//Close the socket and mark as 0 in list for reuse
					close( sd );
					client_socket[i].socket = 0;
				}
					
				//Echo back the message that came in
				else
				{
					//set the string terminating NULL byte on the end
					//of the data read
					// buffer[valread] = '\0';
					// send(sd , buffer , strlen(buffer) , 0 );
					char temp[1000];
					memset(temp, '\0', sizeof(temp));

					strcpy(temp,buffer);
					printf("%s \n",buffer);

					printf("temp");


					if (strncmp(temp, "/broadcast", 10) == 0)
					{
						char msg[1024];
						strncpy(msg,temp+11,strlen(temp)-11);
						printf("hola %s %d\n",msg,max_clients);
						for(int j=0;j<max_clients;j++){
							if(client_socket[i].id!=client_socket[j].id){
								printf("j %d",client_socket[j].socket);
								send(client_socket[j].socket, msg , strlen(msg) , 0 );
							}
						}
						bzero(msg, 1000);
					}


					else if (strncmp(temp, "/sendgroup", 10) == 0)
					{
						printf("yoooo\n");
						char msg[1024];

						char string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						char *pch;
						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));


						int m = get_token(insert_data,string);						 
						memset(msg, '\0', sizeof(msg));

						strncpy(msg,temp+16,strlen(temp)-16);
						int group_id  = atoi(insert_data[1]);

						printf("group id: %d, message: %s\n",group_id,msg);
						int flag=0;
	

						int grp_no = find_group_no(groups,g,group_id);
						if(groups[grp_no].status==1)
							if(groups[grp_no].type==0)
								for(int i=0;i<10;i++){
									if(find_client_socket(client_socket,groups[grp_no].members[i])>0)
										send(find_client_socket(client_socket,groups[grp_no].members[i]),msg,strlen(msg),0);								
								}
							else{
								if(is_group_admin(groups,grp_no,find_client_id(client_socket,sd))){
									for(int i=0;i<10;i++){
										if(find_client_socket(client_socket,groups[grp_no].members[i])>0)
											send(find_client_socket(client_socket,groups[grp_no].members[i]),msg,strlen(msg),0);		
									}
								}
								else
									send(sd,err_message1,sizeof(err_message1),0);
							}

						else
							send(sd,err_message2,sizeof(err_message2),0);

						bzero(msg,1024);
					}

					else if (strncmp(temp, "/send", 5) == 0)
					{
						printf("yoooo\n");
						char msg[1024];

						char string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						// tokenizer start
						char *pch;
						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));

						int m = get_token(insert_data,string);
						memset(msg, '\0', sizeof(msg));

						strncpy(msg,temp+11,strlen(temp)-11);
						int client_sock  = atoi(insert_data[1]);

						printf("client id: %d, message: %s\n",client_sock,msg);
						int flag=0;
	
						for(int h=0;h<max_clients;h++){
							if(client_socket[h].id==client_sock){
								send(client_socket[h].socket,msg,strlen(msg),0);
								printf("send\n");
								flag =1;
							}
						}
						
						if(!flag)
							send(sd,message3,strlen(message3),0);

						bzero(msg,1024);
					}



					else if (strncmp(temp, "/active", 7) == 0)
					{
						char msg[100];
						char tmp[100];
						strcpy(msg,"active clients : \n");
						// bzero(tmp,100);
						for(int j=0;j<max_clients;j++){

							if(client_socket[i].id!=client_socket[j].id && client_socket[j].id!=0){
								
                        		sprintf(tmp, "%d\n", client_socket[j].id);
								strcat(msg,tmp);
								bzero(tmp,100);
							}
						}
						send(sd, msg , strlen(msg) , 0 );
						bzero(tmp,100);
					}

					if (strncmp(temp, "/quit", 5) == 0) {
						
						getpeername(sd , (struct sockaddr*)&address ,(socklen_t*)&addrlen);
						
						printf("Host disconnected , ip %s , port %d \n" ,
						inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
						k-=1;
						close( sd );
						remove_member(groups,client_socket[i].id);
						client_socket[i].socket = 0;
						client_socket[i].id=0;
					}

					else if (strncmp(temp, "/makegroupbroadcast", 19) == 0) {
						char string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);
						char insert_data[10][1024];
						int m = get_token(insert_data,string);
						int g_no = find_group_no(groups,g,atoi(insert_data[1]));

						if(is_group_admin(groups,g_no,find_client_id(client_socket,sd))>0){
							char msg[100];
							sprintf(msg,"converted grp: %d to boradcast only\n",atoi(insert_data[1]));
							groups[g_no].type=1;
							send(sd,msg,strlen(msg),0);
						}
						else
							send(sd,err_message1,strlen(err_message1),0);

					}

					else if (strncmp(temp, "/makegroupreq", 13) == 0) {
					
						printf("received %s\n",temp);

						char msg[1024];

						char string[1024];
						
						strncpy(string, buffer, strlen(buffer));

						// tokenizer start
						char *pch;
						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));

						int m= get_token(insert_data,buffer);

						groups[g].pending = m-1;
						groups[g].status = 0;

						for(int j=0;j<max_clients;j++){
							if(client_socket[j].socket == sd){
								groups[g].group_id = rand()%999;
								groups[g].members[0] = client_socket[j].id;
								groups[g].count+=1;
								groups[g].admin[0] = client_socket[j].id;
								printf("added first admin %d %d\n",groups[g].group_id,groups[g].admin[0]);
								break;
							}
						}

						printf("ffffff \n");
						for(int j=1;j<=m;j++){
							for(int p=0;p<max_clients;p++){

								if(client_socket[p].id==atoi(insert_data[j])){									
									char joingroupmsg[500];
									sprintf(joingroupmsg, "you have been invited to join group: %d type /joingroup followed by group id to join else type /declinegroup",groups[g].group_id);
									send(client_socket[p].socket, joingroupmsg , strlen(joingroupmsg) , 0 );
								}
							}

						}

						g+=1;

						
						printf("group req sent\n");
					}

					// bzero(buffer,1000);

					else if (strncmp(temp, "/makegroup", 10) == 0) {
					
						printf("received %s\n",temp);

						char msg[1024];

						char string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));
						int m  = get_token(insert_data,string);

						for(int j=0;j<max_clients;j++){
							if(client_socket[j].socket == sd){
								groups[g].group_id = rand()%999;
								groups[g].members[0] = client_socket[j].id;
								groups[g].count+=1;
								groups[g].admin[0] = client_socket[j].id;
								printf("added first admin %d %d\n",groups[g].group_id,groups[g].admin[0]);
								break;
							}
						}

						printf("eeeee \n");

						for(int j=1;j<=m;j++){
							
							groups[g].members[j] = atoi(insert_data[j]);
							groups[g].count+=1;

							for(int p=0;p<max_clients;p++){

								if(client_socket[p].id==groups[g].members[j]){									
									char joingroupmsg[100];
									sprintf(joingroupmsg, "you have been added to group %d\n",groups[g].group_id);
									send(client_socket[p].socket, joingroupmsg , strlen(joingroupmsg) , 0 );
								}
							}

						}
						
						char text[100];
						sprintf(text,"group created, id: %d",groups[g].group_id);
						send(sd,text,strlen(text),0);
						g+=1;

						printf("group made\n");
					}
					else if (strncmp(temp, "/addtogroup", 11) == 0) {
						printf("received %s\n",temp);

						char msg[1024];

						char string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));

						int m = get_token(insert_data,string);
						int group_id = atoi(insert_data[1]);
						int g_no = find_group_no(groups,g,group_id);
						char str[500];
						if(is_group_admin(groups,g_no,find_client_id(client_socket,sd))>0){
							for(int j=2;j<=m;j++){
								for(int h = 0;h<10;h++){
									if(groups[g_no].members[h]==0 || groups[g_no].members[h]=='\0'){
										groups[g_no].members[h]=atoi(insert_data[j]);
										groups[g_no].count+=1;
										sprintf(str,"you have been added to group %d\n",group_id);
										send(find_client_socket(client_socket,atoi(insert_data[j])),str,strlen(str),0);
										break;
									}
								}
							}
						}
						else
							send(sd,err_message1,strlen(err_message1),0);

					}
					else if (strncmp(temp, "/joingroup", 10) == 0) {
						printf("yoooo\n");
						char msg[1024],string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));


						int m = get_token(insert_data,string);

						int g_id = atoi(insert_data[1]);
						
						printf("joining group id %d\n",g_id);
						for(int j=0;j<g;j++){
							if(groups[j].group_id==g_id ){
								if(groups[j].count < 5){
									groups[j].members[groups[j].count] = find_client_id(client_socket,sd);
									printf("er ewrw %d\n",groups[j].members[groups[j].count]);
									groups[j].count+=1;
									groups[j].pending-=1;
									send(sd,message5,strlen(message5),0);
								}

								else
									send(sd,message4,strlen(message4),0);

								break;
							}
						}
					}

					else if (strncmp(temp, "/declinegroup", 13) == 0) {
						printf("yoooo\n");
						char msg[1024],string[1024];
						
						strncpy(string, buffer, strlen(buffer) - 1);

						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));


						int m = get_token(insert_data,string);

						int g_id = atoi(insert_data[1]);
						
						printf("joining group id %d\n",g_id);
						for(int j=0;j<g;j++){
							if(groups[j].group_id==g_id ){
								groups[j].pending-=1;
								send(sd,message8,strlen(message8),0);
								break;
							}
						}
					}


					else if (strncmp(temp, "/activegroups", 12) == 0) {
						
						char msg[1000];
						char tmp[1000];
						strcpy(msg,"active groups : \n");


						for(int h =0;h<g;h++){
							sprintf(tmp, "group id: %d\n members: \n",groups[h].group_id);
							strcat(msg,tmp);
							bzero(tmp,1000);
							if(groups[h].status==1)
								for(int j=0;j<groups[h].count;j++){
									if(groups[h].members[j]>0)
									sprintf(tmp, "%d\n",groups[h].members[j]);
									strcat(msg,tmp);
									bzero(tmp,1000);
								}
						}

						send(sd, msg , strlen(msg) , 0 );
						bzero(tmp,1000);
					}



					else if (strncmp(temp, "/makeadmin", 10) == 0) {
						printf("yoooo\n");
						char msg[1024];
						char string[1024];
						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));

						int m = get_token(insert_data,buffer);
						printf("uckkkkkk!\n");

						int group_id = atoi(insert_data[1]);
						int client_id = atoi(insert_data[2]);
						printf("group_id %d\nclient_id %d\n",group_id,client_id);
						if(is_group_admin(groups,find_group_no(groups,g,group_id), find_client_id(client_socket,sd))>0 && groups[find_group_no(groups,g,group_id)].status==1){
							if(make_group_admin(groups,find_group_no(groups,g,group_id),client_id)){
								char msg[1024];
								sprintf(msg,"%s %d",message6,group_id);
								send(find_client_socket(client_socket,client_id),msg,strlen(msg),0);
							}
							else{
								printf("error making %d group admin of %d\n",client_id,group_id);
							}

						}	
						else
							send(sd,err_message1,strlen(err_message1),0);
					}

					else if (strncmp(temp, "/removefromgroup", 16) == 0) {
						printf("yoooo\n");
						char msg[1024];
						char string[1024];
						char insert_data[10][1024];

						memset(insert_data, '\0', sizeof(insert_data));

						int m = get_token(insert_data,buffer);
						printf("uckkkkkk!\n");

						int group_id = atoi(insert_data[1]);
						if(is_group_admin(groups,find_group_no(groups,g,group_id), find_client_id(client_socket,sd))>0 && groups[find_group_no(groups,g,group_id)].status==1){
						for(int j=2;j<=m;j++){
						
							int client_id = atoi(insert_data[j]);
							printf("group_id %d\nclient_id %d\n",group_id,client_id);
							
								if(remove_group_member(groups,find_group_no(groups,g,group_id),client_id)){
									char msg[1024];
									sprintf(msg,"%s %d",message7,group_id);
									send(find_client_socket(client_socket,client_id),msg,strlen(msg),0);
								}
								else{
									printf("error removing %d from group %d\n",client_id,group_id);
								}

							}	
						}
						else
							send(sd,err_message1,strlen(err_message1),0);
						
					}

					for(int i=0;i<g;i++){
						
						int flag =0;

						for(int j=0;j<5;j++){
							if(groups[i].admin[j]!=0){
								flag=1;
								break;
							}
						}

						if(flag==1)
							if(groups[i].pending==0){
								groups[i].status =1;
						}
						else
							groups[i].status = 0;
					}
				}


				bzero(buffer,1000);

			}
		}
	}	
	return 0;
}
