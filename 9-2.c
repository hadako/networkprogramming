#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

void client(int port, char *hostname)
{
	struct hostent *host;
	struct sockaddr_in serv;
	int s, x;
	char buf[512];
	host = gethostbyname(hostname);
	s = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *)&serv, sizeof(serv));
	serv.sin_family = PF_INET;
	serv.sin_port = htons(port);
	bcopy(host->h_addr, (char *)&serv.sin_addr, host->h_length);
	if (connect(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
		fprintf(stderr, "cannot connect\n");
		exit(1);
	}
	for(;;){
		printf("input a number?");
		scanf("%d",&x);
		if(x==0) break;
		x=htonl(x);
		write(s,&x,sizeof(int));
	}
	close(s);
}

void server(int port)
{
	char hostname[256];
	struct hostent *host;
	struct sockaddr_in me;
	int s0, s, n,x;
	char buf[512];
	s0 = socket(PF_INET, SOCK_STREAM, 0);
	gethostname(hostname, sizeof(hostname));
	host = gethostbyname(hostname);
	bzero((char *)&me, sizeof(me));
	me.sin_family = PF_INET;
	me.sin_port = htons(port);
	bcopy(host->h_addr, (char *)&me.sin_addr, host->h_length);
	if (bind(s0, (struct sockaddr *)&me, sizeof(me)) < 0) {
		fprintf(stderr, "cannot bind socket\n");
		exit(1);
	}
	listen(s0, 1);
	s = accept(s0, NULL, NULL);
	while ((n = read(s, &x, sizeof(int))) > 0) {
		x=ntohl(x);
		printf("%d\n",x);
	}
	close(s);
	close(s0);

	
}

int main(int argc, char *argv[])
{
	if (argc >= 3) client(atoi(argv[1]), argv[2]);
	else if (argc == 2) server(atoi(argv[1]));
	else {
		fprintf(stderr, "(server) %s port\n", argv[0]);
		fprintf(stderr, "(client) %s port hostname\n", argv[0]);
		exit(1);
	}
}
