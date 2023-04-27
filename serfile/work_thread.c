#include"work_thread.h"
#include<sys/wait.h>

#define ARGC_MAX 10
#define ERR "err"
struct ArgNode
{
    int c;
};

void send_err(int c)
{
    send(c,ERR,strlen(ERR),0);
}

char *get_cmd(char buff[],char* myargv[])
{
    if(buff==NULL||myargv==NULL)
    {
        return NULL;
    }

    char *ptr=NULL;
    int i=0;
    char *s=strtok_r(buff," ",&ptr);
    while(s!=NULL)
    {
       myargv[i++]=s;
       s=strtok_r(NULL," ",&ptr);
    }

    return myargv[0];
}

static int pipefd[2];//新建管道

void send_file(int c,char* filename)
{
    
}

void *work_fun(void*arg)
{
    //int c=(int)arg;
    struct ArgNode*p=(struct ArgNode*)arg;
    if(p==NULL)
        return NULL;
    int c=p->c;
    free(p);
    while(1)
    {
        char buff[128]={0};
        int num=recv(c,buff,127,0);//收到数据"ls""rm"
        if(num<=0)
        {
            break;
        }

        char* myargv[ARGC_MAX]={0}; 
        char* cmd=get_cmd(buff,myargv);
        if(cmd==NULL)
        {
            send_err(c);
            continue;
        }
        else if(strcmp(cmd,"get")==0)
        {
            //下载
        }
        else if(strcmp(cmd,"up")==0)
        {
            //上传
        }
        else
        {
            //fork+exec
            int pipefd[2];
            if(pipe(pipefd)==-1)
            {
                send_err(c);
                continue;
            }

            pid_t pid=fork();
            if(pid==-1)
            {
                send_err(c);
                continue;
            }
        
            if(pid==0)
            {
                close(pipefd[0]);
                //用管道的写端覆盖标准输出和标准错误输出
                dup2(pipefd[1],1);
                dup2(pipefd[1],2);
                execvp(cmd,myargv);
                perror("exec err\n");
                exit(0);
            }
            close(pipefd[1]);
            wait(NULL);
            char send_buff[1024]={"ok#"};
            read(pipefd[0],send_buff+3,1020);
            send(c,send_buff,strlen(send_buff),0);
        }
        
    }
    printf("close\n");

}

    

void start_thread(int c)
{
    //if(pipe(pipefd)==-1)
    //{
    //    printf("管道创建失败\n");
    //    return ;
    //}
    
    //write(pipefd[1],&c,sizeof(c));

    pthread_t id;
    struct ArgNode* p=(struct ArgNode*)malloc(sizeof(struct ArgNode));
    if(p==NULL)
    {
        return;
    }
    p->c=c;
    if(pthread_create(&id,NULL,work_fun,(void*)p)!=0)
    {
        close(c);
        printf("启动线程失败\n");
        return ;
    }
    //close(pipefd[1]);
}
