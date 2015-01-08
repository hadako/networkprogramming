#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*デュアルスタック対応クライアントプログラム*/
/*argv[1]=ipadress argv[2]=ポート*/


int 
main(int argc,char **argv){
	struct addrinfo hints, *res, *res0;
	ssize_t l;
	int s;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	char buf[1024];
	int error;


	/*check the number of argments*/
	if(argc != 3){
		fprintf(stderr, "usage:test host port\n");
		exit(1);
	}

	//resolve address/port into socketaddr 
	memset(&hints,0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	error=getaddrinfo(argv[1],argv[2],&hints,&res0); 
	/*デュアルスタックとするにはgetaddrinfoを利用して名前引きを行う。getaddrinfoはgethostbynameとgetservbynameの昨日を持っていて、
	ホスト名からひっけるアヅレスを一回の関数コードで全て取得する。不定数のアドレス情報がgetaddrinfo関数の中で確保されたメモリ空間に格納される。
	つまりres0の示す先のメモリオブジェクトは動的に確保されている。*/

	if(error){
		fprintf(stderr,"%s %s:%s\n",argv[1],argv[2],gai_strerror(error)); 
		exit(1);
		/*not reached*/
	}

	/*前記のgetaddrinfoの結果haresにリスト形式で保存されている。そのリストをたどりつつ先頭から順番に適切なソケットを生成して接続していく。
	このプログラムでは接続の前にされにgetnameinfoでホスト名を撮り直して表示するようにしている。*/

	/*try all the scokaddrs until connection goes successful*/
	for(res=res0;res;res=res->ai_next){
		error = getnameinfo(res->ai_addr, res->ai_addrlen,hbuf,sizeof(hbuf),sbuf,sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV); //アドレスから名前への変換
		if(error){
			fprintf(stderr,"%s %s:%s\n",argv[1],argv[2],gai_strerror(error));
			continue;
		}

		fprintf(stderr,"trying %s port %s\n",hbuf,sbuf);

		s=socket(res->ai_family,res->ai_socktype,res->ai_protocol);

		if(s<0){
			continue;
		}

		if(connect(s,res->ai_addr,res->ai_addrlen)<0){
			close(s);
			s=-1;
			continue;
		}

		while((l=read(s,buf,sizeof(buf)))>0){
			write(STDOUT_FILENO,buf,l);
			close(s);
		}


		exit(0);
	}

	fprintf(stderr,"test: no destination to connect to \n");
	exit(1);
	
}