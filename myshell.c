#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 外部/cd/
 
 */

int parse(char *line,char *argv[]){
    int argc;
    int n;
    
    argc=0;
    while(*line){
        argv[argc++] = line;
        while (*line && *line >' ') {
            line++;
        } //スペースより大きかったら
        while (*line && *line <= ' ') {
            n++;
            *line++ = '\0';
        } //スペース以下、つまりアスキーコードの文字じゃない奴
    }
    return argc;
    
}


void cd(int argc, char *path){
    if(argc==1) path=getenv("HOME");
    if(chdir(path)==-1){
        printf("\"cd\" comannd error\n");
        exit(-1);
    }
}

void pipecmd(char *argv[],int i){
	int pid1,pid2,status;
	int fds[2];

	pid1=fork(); //子プロセス
	if(pid1<0){
		printf("prosess1 fork error\n");
		exit(1);
	}
	else if(pid1==0){
		pipe(fds);
		pid2=fork(); //孫プロセス
		if(pid2<0){
			printf("prosess2 fork error\n");
			exit(1);
		}
		else if(pid2==0){
			dup2(fds[1],1);
			close(fds[1]);
			execvp(argv[1],&argv[i+1]);
		}
		else{
			dup2(fds[0],0);
			close(fds[0]);
			execvp(argv[0],argv);
		}
	}
	while (wait(&status)!=pid1);
}


    /*外部*/
void execute (char *argv[]){
    int pid,status;
    int i,n;
    char *cmd;
    
    cmd=argv[0];
    
    pid=fork();
    if(pid<0){
        printf("fork error\n");
        exit(1);
    }else if(pid==0){ //子プロセスの動き
        execvp(cmd,argv);
        printf("%s command error\n",argv[0]);
        exit(1);   
    }
    //親プロセスの動き
    else {
        while (wait(&status) !=pid);
    }

}

int main(){
    char line[512]; //入力されたコマンド　
    char *argv[16],*cmd;
    int argc,i,pipe=0;
    
    
    for (;;){
        printf("#");
        fgets(line,512,stdin);
        argc=parse(line,argv);
        if (argc>0){
        	for(i=0;i<argc;i++){
        		if(strcmp(argv[i],"|")==0){
        			argv[i]=NULL;
        			pipe=1;
        			break;
        		}
        	}
        	//pipeがない
        	if(pipe==0){
         	    if(strcmp(argv[0],"cd")==0) cd(argc, argv[1]); //cd
        	    else if(strcmp(argv[0],"exit")==0) exit(1); //exit
        	    else execute(argv);//組み込み以外
        	}else { //パイプあり
        		pipecmd(argv,i);
        	}
        }
    }
    return 0;
}