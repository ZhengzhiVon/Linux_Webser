#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<fcntl.h>

int connect_ser();

void recv_file(int c,char* filename,char* cmd_str)
{
    if(filename==NULL||cmd_str==NULL)
    {
        return;
    }
    send(c,cmd_str,strlen(cmd_str),0);

    char buff[128]={0};
    int num=recv(c,buff,127,0);
    {
        printf("ser close\n");
    }

    if(strncmp(buff,"ok#",3)!=0)//"ok#2324sad"数字后面就会出现粘包
    {
        printf("文件不存在\n");
        return;
    }

    //文件过大时,int可能不够用
    int filesize=atoi(buff+3);
    printf("filesize=%d\n",filesize);

    int fd=open(filename,O_CREAT|O_WRONLY,0600);
    if(fd==-1)
    {
        send(c,"err",3,0);
        return;
    }

    if(filesize==0)
    {
        close(fd);
        return;
    }

    //没有让用户选择

    send(c,"ok",2,0);

    char data[512]={0};
    int curr_size=0;
    int n=0;
    while((n=recv(c,data,512,0))>0)
    {
        write(fd,data,n);
        curr_size+=n;
        float f=curr_size*100.0/filesize;
        printf("下载百分比:%.2f%%\r",f);
        fflush(stdout);
        if(curr_size>=filesize)
        {
            break;
        }
    }
    printf("\n下载成功\n");
    close(fd);
}

char* get_cmd(char buff[],char* myargv[])
{
    if(buff==NULL||myargv==NULL)
    {
        return NULL;
    }
    
    int i=0;
    char* s=strtok(buff," ");
    while(s!=NULL)
    {
        myargv[i++]=s;
        s=strtok(NULL," ");
    }
    return myargv[0];
}
int main()
{
    int c=connect_ser();
    if(c==-1)
    {
        exit(0);
    }

    while(1)
    {
        printf("connect~ >>  ");
        fflush(stdout);

        char buff[128]={0};
        fgets(buff,128,stdin);
        buff[strlen(buff)-1]=0;//删除"\n",存放获取的命令
        char cmd_buff[128]={0};
        strcpy(cmd_buff,buff);

        char* myargv[10]={0};
        char* cmd=get_cmd(buff,myargv);
        if(cmd==NULL)
        {
            continue;
        }
        if(strcmp(cmd,"exit")==0)
        {
            break;
        }
        else if(strcmp(cmd,"get")==0)
        {
            //下载
            recv_file(c,myargv[1],cmd_buff);
        }
        else if(strcmp(cmd,"up")==0)
        {
            //上传
        }
        else
        {
            //执行命令
            send(c,cmd_buff,strlen(cmd_buff),0);
            char recv_buff[1024]={0};
            if(recv(c,recv_buff,1023,0)<=0)//数据量较少无需循环
            {
                printf("ser close\n");
                break;
            }
            if(strncmp(recv_buff,"ok#",3)==0)
            {
                printf("%s\n",recv_buff+3);
            }
            else
            {
                printf("命令执行错误\n");
            }
        }
    }
    close(c);
    exit(0);
    
}

int connect_ser()
{
    int sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        printf("bind err\n");
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr,0,sizeof(saddr));
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(6000);
    saddr.sin_addr.s_addr=inet_addr("127.0.0.1");

    int res=connect(sockfd,(struct sockaddr*)&saddr,sizeof(saddr));
    if(res==-1)
    {   
        printf("connect ser failed\n");
        return -1;
    }
    
    return sockfd;
}
