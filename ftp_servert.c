#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>

void open_img(int ls) {
    FILE    *fp;
    char    buf[1024];
    int    i, size;
    fp = fopen("img.jpg", "rb");
    if(fp != NULL) {
        while((size = fread( buf, sizeof(unsigned char), 1024, fp ))>0) {
                write(ls,buf,sizeof(size));
        }
        fclose(fp);
    }
}

int main(int argc, char **argv)
{
	struct addrinfo hints, *res;
	int error;
	struct sockaddr_storage from;
	socklen_t fromlen;
	int ls;
	int s;
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int ch;
	int af = AF_INET6;
	#ifdef IPV6_V6ONLY
		const int on = 1;
	#endif
		while ((ch = getopt(argc, argv, "46")) != -1) {
	switch (ch) {
		case '4':
			af = AF_INET;
			break;
		case '6':
			af = AF_INET6;
			break;
		default:
			fprintf(stderr, "usage: test [-46] port\n");
			exit(1);
			/*NOTREACHED*/
		}
	}

	argc -= optind;
	argv += optind;
	if (argc != 1) {
		fprintf(stderr, "usage: test port\n");
		exit(1);
		/*NOTREACHED*/
	}

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = af;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	error = getaddrinfo(NULL, argv[0], &hints, &res);
	if (error) {
		fprintf(stderr, "%s: %s\n", argv[0], gai_strerror(error));
		exit(1);
		/*NOTREACHED*/
	}
	if (res->ai_next) {
		fprintf(stderr, "%s: multiple address returned\n", argv[0]);
		exit(1);
		/*NOTREACHED*/
	}
	s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (s < 0) {
		perror("socket");
		exit(1);
		/*NOTREACHED*/
	}

	#ifdef IPV6_V6ONLY
		if (res->ai_family == AF_INET6 &&
 			setsockopt(s, IPPROTO_IPV6, IPV6_V6ONLY, &on, sizeof(on)) < 0) {
			perror("bind");
			exit(1);
			/*NOTREACHED*/
		}
	#endif

	if (bind(s, res->ai_addr, res->ai_addrlen) < 0) {
		perror("bind");
		exit(1);
		/*NOTREACHED*/
	}
	if (listen(s, 5) < 0) {
		perror("listen");
		exit(1);
		/*NOTREACHED*/
	}
	error = getnameinfo(res->ai_addr, res->ai_addrlen, hbuf,
 	sizeof(hbuf), sbuf, sizeof(sbuf),
 	NI_NUMERICHOST | NI_NUMERICSERV);
	if (error) {
		fprintf(stderr, "test: %s\n", gai_strerror(error));
		exit(1);
		/*NOTREACHED*/
	}
	fprintf(stderr, "listen to %s %s\n", hbuf, sbuf);
	while (1) {
		fromlen = sizeof(from);
		ls = accept(s, (struct sockaddr *)&from, &fromlen);
		if (ls < 0) continue;
		write(ls, "hello\n", 6);
		open_img(ls);
 		close(ls);
	}
		/*NOTREACHED*/
}




