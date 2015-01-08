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
	int s, x,y;
	char kachimake[3][16]={"勝ち","負け","あいこ"};

	host = gethostbyname(hostname);
	s = socket(AF_INET, SOCK_STREAM, 0); //ソケット立てる
	bzero((char *)&serv, sizeof(serv));
	serv.sin_family = PF_INET;
	serv.sin_port = htons(port);
	bcopy(host->h_addr, (char *)&serv.sin_addr, host->h_length);

	if (connect(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) { //コネクトする
		fprintf(stderr, "cannot connect\n");
		exit(1);
	}
	for(;;){
		x=3;
		while(x<0||x>2){
			printf("じゃんけん グー(0),チョキ(1),パー(2)");
			scanf("%d",&x);
		}

		x=htonl(x);
		write(s,&x,sizeof(int));

		read(s,&y,sizeof(int));
		y=ntohl(y);
		printf("%s\n",kachimake[y]);
	}
	close(s);
}

void server(int port)
{
	char hostname[256];
	struct hostent *host;
	struct sockaddr_in me;
	int s0, s[2], n,x,kekka,kekka2,i,j,client,te[2];


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

	listen(s0, 2);
	for(i=0;i<2;i++){
		s [i]= accept(s0, NULL, NULL);
		client++;
		printf("client %d accepted\n",client);
	}

	while(1){
		for(j=0;j<2;j++){
			read(s[j],&x,sizeof(int));
			te[j]=ntohl(x);
		}

		//te 0:ぐー 1:ちょき　2:ぱー
		//kakka kachi:0 make:1 aiko:2 
		//client1勝ち負け判定
		if(te[0]==0){
			if(te[1]==0) kekka=2;
			else if(te[1]==1) kekka=0;
			else kekka=1;
		}else if(te[0]==1){
			if(te[1]==0) kekka=1;
			else if(te[1]==1) kekka=2;
			else kekka=0;
		}else{
			if(te[1]==0) kekka=0;
			else if(te[1]==1) kekka=1;
			else kekka=2;
		}

		//client2勝ち負け判定
		if(kekka==0){
			kekka2=1;
		}else if(kekka==1){
			kekka2=0;
		}else{
			kekka2=2;
		}

		kekka=htonl(kekka);
		write(s[0],&kekka,sizeof(int));
		kekka2=htonl(kekka2);
		write(s[1],&kekka2,sizeof(int));
	}

	close(s[0]);
	close(s[1]);
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
