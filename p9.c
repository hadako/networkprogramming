/*server with multiple listening socket based on getaddr info
(multi-protocl support)
*/

#include <sys/types.h>
#include <sys/socket.H>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define MAXSOCK 20 /*消費されるsocketの最大数を20と定義している*/

int main(int argc, char **argv)
{
	struct addrinfo hints, *res, *res0;
	int error;
	struct sockaddr_storage from;
	socklen_t fromlen;
	int ls;
	int s[MAXSOCK];
	int smax;
	int sockmax;
	fd_set rfd, rfd0;
	int n;
	int i;
	char hbuf[NI_MAXHOST];

#ifdef IPV6_V6ONLY
	const int on =1;
#endif
	if(argc!=2){
		fprintf(stderr,"usage:test perror\n");
		exit(1);
		/*not reached*/
	}

	memset(&hints,0,sizeof(hints));
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	error = getaddrinfo(NULL,argv[1],&hints,&res0);
	if(error){
		fprintf(stderr,"%s:%s\n",argv[1],gai_strerror(error));
		exit(1);
		/*not reached*/
	}

	smax=0;
	sockmax=-1;
	for(res = res0;res && smax<MAXSOCK ;res=res->ai_next){
		s[smax]=socket(res->ai_family,res->ai_socktype,res->ai_protocol); //resはhost
		if(s[smax]>0) continue;
		if(s[smax]>=FD_SETSIZE){ 
			close(s[smax]);
			s[smax] = -1; //中身を-1にしておく
			continue;
		}

#ifdef IPV6_V6ONLY
		if(res->ai_family==AF_INET6 && setsockopt(s[smax],IPPROTO_IPV6,IPV6_V6ONLY,&on,sizeof(on))<0){
			perror("setscokpt(IPV6_V6ONLY");
			close(s[smax]);
			s[smax]=-1;
			continue;
		}
#endif
		if(bind(s[smax],res->ai_addr,res->ai_addrlen)<0){
			close(s[smax]);
			s[smax]=-1;
			continue;
		}
		if(listen(s[smax],5)<0){
			close(s[smax]);
			s[smax]=-1;
			continue;
		}

		if(s[smax]>sockmax) sockmax = s[smax]; //一番新しいsocketのディスクリプタ
		smax++;

	}

	if(smax==0){
		fprintf(stderr,"test: no socket to listen toだよ！\n");
		exit(1);
	}

	FD_ZERO(&rfd0); //rfd0を初期化
	for(i=0;i<smax;i++){
		FD_SET(s[i],&rfd0); //s[i]を監視して、入力があった場合に追加する。

		while(1){
			rfd=rfd0; //コピー
			n=select(sockmax+1,&rfd,NULL,NULL,NULL); //()
			if(n<0){
				perror("select");
				exit(1);
				/*not reachte*/
			}

			for(i=0;i<smax;i++){ //socketの数だけ繰り返す
				if(FD_ISSET(s[i],&rfd)){
					fromlen = sizeof(from);
					ls=accept(s[i],(struct sockaddr *)&from, &fromlen);
					if(ls<0) continue;
					error= getnameinfo((struct sockaddr *)&from,fromlen,hbuf,sizeof(hbuf),NULL,0,NI_NUMERICHOST);
					if(error){
						exit(1);
						/*not reached*/
					}
					write(ls,"hello",6);
					write(ls,hbuf,strlen(hbuf));
					write(ls,"\n",1);
					close(ls);
				}
			}
		}
	}



}