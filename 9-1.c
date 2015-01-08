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
	int s, n;
	char buf[512];

	host = gethostbyname(hostname);
	s = socket(AF_INET, SOCK_STREAM, 0); //ソケットを作る
	bzero((char *)&serv, sizeof(serv));
	serv.sin_family = PF_INET;
	serv.sin_port = htons(port);
	bcopy(host->h_addr, (char *)&serv.sin_addr, host->h_length);

	if (connect(s, (struct sockaddr *)&serv, sizeof(serv)) < 0) { //servに接続する。
		fprintf(stderr, "cannot connect\n"); //エラーを返す
		exit(1);
	}
	
	while ((n = read(0, buf, 512)) > 0) {
		write(s, buf, n);
	}
	close(s);
}

void server(int port)
{
	char hostname[256];
	struct hostent *host;
	struct sockaddr_in me;
	int s0, s, n;
	char buf[512];

	s0 = socket(PF_INET, SOCK_STREAM, 0); //ソケットを作る

	gethostname(hostname, sizeof(hostname)); //ホストネーム(IPアドレス)を取ってくる
	host = gethostbyname(hostname); //ホストネームからホストをたてる
	bzero((char *)&me, sizeof(me)); //meを初期化
	me.sin_family = PF_INET; //meのsinfamilyをPF_INETに
	me.sin_port = htons(port); //meのsin_portをhtonsポートに

	bcopy(host->h_addr, (char *)&me.sin_addr, host->h_length);

	if (bind(s0, (struct sockaddr *)&me, sizeof(me)) < 0) { //ソケットをディスクリプタにバインと
		fprintf(stderr, "cannot bind socket\n"); //失敗したらエラーを返す
		exit(1);
	}

	listen(s0, 1); //ソケットをリッスンにする
	s = accept(s0, NULL, NULL); //ソケット接続待機、要求をうけたら

	while ((n = read(s, buf, 512)) > 0) {
		write(1, buf, n);
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
