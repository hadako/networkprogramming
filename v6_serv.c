#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

int
main(int argc, char **argv){
	struct sockaddr_storage from; 
	/*
	sockaddr_storageはシステムがサポートする
	あらゆるプロトコルの情報保存バッファとして利用できる。*/
	socklen_t fromlen;
	char hbuf[NI_MAXHOST];

	fromlen=sizeof(from);
	/*sockaddr_storageで定義されたfrom変数を引数として与えている。*/
	if(getpeername(0,(struct sockaddr *)&from,&fromlen)<0){
		exit(1);
		/*not reached*/
	}

	/*つかている通信プロトコルに応じた形式で格納されている通信相手に
	関する情報をgetnameinfoを利用して文字列化する。*/
	if(getnameinfo((struct sockaddr *)&from,fromlen,hbuf,sizeof(hbuf),NULL,0,NI_NUMERICHOST) !=0){
		exit(1);
		/*not reached*/
	}

	write(0,"hello",6);
	write(0,hbuf,strlen(hbuf));
	write(0,"\n",1);
	exit(0);

}