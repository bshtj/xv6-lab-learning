#include "kernel/param.h"
#include "user/user.h"

#define MAX 512

int main(int argc,char *argv[])
{
	//在xargs后面至少有两个输入 
	if(argc<2)
	{
		fprintf("2,usage: xargs command\n");
		exit(1);//异常退出 
	}
	//使用cmd来存放argv中的命令，MAXARG定义了 `exec` 系统调用中参数的最大数量 32。 
	char *cmd[MAXARG];
	int index=0;//从cmd下标为0开始存 
	for(int i=1;i<argc;i++)
	{
		cmd[index++]=argv[i]; 
	} 
	//获取管道运算符|之前的结果作为新的输入
	char buf[MAX];//定义缓冲区存放|之前的结果
	int len=read(0,buf,sizeof(buf));//从标准输入流中读取
	char temp[MAX];//暂时存储buf中的内容，之后直接存到cmd中 
	int index_temp=0;
	for(int i=0;i<len;i++)
	{
		if(buf[i]!='\n')
		{
			temp[index_temp++]=buf[i];
		}
		else
		{
			//读到回车键
			temp[index_temp]=0;
			cmd[index]=temp;
			if(fork()==0)
			{
				exec(argv[1],cmd);
				exit(0);
			} 
			else
			{
				wait(0);
//			 	index_temp=0;
//			 	memset(temp,0,sizeof(temp));
			}
		}
	}
	 
	exit(0);
} 
