/*
Your goal is to use pipe and fork to set up the pipeline. 
The first process feeds the numbers 2 through 35 into the pipeline.
For each prime number, you will arrange to create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. 
Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.
*/
// 文件描述符重定向(讲成映射比较好)
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
void mapping(int n,int*pd)
{
	close(n);
	dup(pd[n]);
	close(pd[0]);
	close(pd[1]);
}

void isprime()
{
	int tmp_old;
	int tmp_new;
	int fd[2];
	//从管道中读取数据(数据如果存在) 
	if(read(0,&tmp_old,sizeof(int)))
	{
		//第一个一直都是素数
		printf("prime %d\n",tmp_old);
		//接下来筛选符合条件数(不是第一个数的倍数)
		pipe(fd);//创建一个管道
		if(fork()==0)
		{
			mapping(1,fd);
			//子进程
			while(read(0,&tmp_new,sizeof(int))){
				//不是第一个素数的倍数，符合条件 
				if(tmp_new%tmp_old!=0)
				{
					write(1,&tmp_new,sizeof(int)); 
				}
			} 
		} 
		else
		{
	        //父进程 
	        //等待子进程完成 
	        wait((int*)0); 
	        mapping(0,fd);
	        //迭代 
	        isprime();
		}
		 
	}
}

int main(int argc,cahr *argv[])
{
	int fd[2];
	pipe(fd);
	if(fork()==0)
	{
		//子进程
		mapping(1,fd);
		for(int i-2;i<=35;i++)
		{
			write(1,&i,sizeof(&i));
		}
	}
	else
	{
		//父进程
		wait((int*)0);
		mapping(0,fd);
		isprime();
		
	}
	
	exit(0);
 } 

