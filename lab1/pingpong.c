/*
Write a program that uses UNIX system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. 
The parent should send a byte to the child;
 the child should print "<pid>: received ping", where <pid> is its process ID, write the byte on the pipe to the parent, and exit; 
 the parent should read the byte from the child, print "<pid>: received pong", and exit. 
 Your solution should be in the file user/pingpong.c.
*/ 
// 引入声明类型的头文件
#include "kernel/types.h"
// 引入声明系统调用和 user/ulib.c 中其他函数的头文件
#include "user/user.h"

int main(int argc,char* argv[])
{
	int p_fd[2],c_fd[2];//创建父进程和子进程的文件描述符，2是因为pipe要接受整形数组，0是读取端，1是写入端 
	pipe(p_fd);
	pipe(c_fd);
	char buf[8];//通信数据
	//fork()创建进程，返回0为自进程，父进程返回pid，失败返回-1
	if(fork()==0)
	{
		//子进程
		read(p_fd[0],buf,4);//从父进程的管道读取4字节的数据（ping）存储在buf中
		printf("%d: received %s\n",gitpid(),buf); 
		write(c_fd[1],"pong",strlen("pong"));
	} 
	else
	{
		//父进程 
		write(p_fd[1], "ping", strlen("ping"));
        wait(NULL);//等待子进程结束 
        read(c_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
	}
	
	exit(0);
}
