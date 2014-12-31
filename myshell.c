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

void pipe(argc){


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
    char line[512]; //入力されたコマンド　line=argv[]
    char *argv[16],*cmd;
    int argc;
    
    cmd=argv[0];
    
    for (;;){
        printf("#");
        fgets(line,512,stdin);
        argc=parse(line,argv);
        if (argc>0){
            if(strcmp(cmd,"cd")==0) cd(argc, argv[1]); //cd
            else if(strcmp(cmd,"exit")==0) exit(1); //exit
            else execute(argv);
            }
    }
    return 0;
}