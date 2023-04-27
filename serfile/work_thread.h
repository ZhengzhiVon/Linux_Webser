#ifndef __WORK_PTHREAD_H
#define __WORK_PTHREAD_H

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<fcntl.h>
#include<pthread.h>

void start_thread(int c);
#endif
