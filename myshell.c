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

/*
void pipecmd(char *argv[],int i){
	int fd[2];
	int pid1,pid2,status;


	pipe(fd); //パイプ作成
	pid1=fork();
	if(pid1<0){
		printf("fork error\n");
        exit(1);
	}else if(pid1==0){ //Kind(一つ目のコマンド)
		dup2(fd[1],1); //
		close(fd[1]);
		execvp(argv[0],argv);	
		fprintf(stderr,"erorr");
		close(fd[0]);
		exit(1);
	}else{
		close(fd[1]);
		wait(&status);
		if(status!=0){
			close(fd[0]);
		}
		pid2=fork(); //もう一回フォーク
		if(pid2<0){
			printf("fork error\n");
       		exit(1);
		}else if(pid2==0){ //Kind(２つ目のコマンド)
			dup2(fd[0],0);
			close(fd[0]);
			execvp(argv[i+1],&argv[i+1]);
			fprintf(stderr,"erorr");
			exit(1);
		}else{
			close(fd[0]);
			wait(&status);
		}
	}

}
*/


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
        		printf("pass:%d\n",__LINE__);
        		if(strcmp(argv[i],"|")==0){
        			printf("pass:%d\n",__LINE__);
        			argv[i]=NULL;
        			pipe=1;
        			break;
        		}
        	}
        	//pipeがない
        	if(pipe==0){
        		printf("pass:%d\n",__LINE__);
         	    if(strcmp(argv[0],"cd")==0) cd(argc, argv[1]); //cd
        	    else if(strcmp(argv[0],"exit")==0) exit(1); //exit
        	    else execute(argv);//組み込み以外
        	}else {
        		execute(argv);
        		execute(&argv[i+1]);
        	}
        }
    }
    return 0;
}