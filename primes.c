/*
Your goal is to use pipe and fork to set up the pipeline. 
The first process feeds the numbers 2 through 35 into the pipeline.
For each prime number, you will arrange to create one process that reads from its left neighbor over a pipe and writes to its right neighbor over another pipe. 
Since xv6 has limited number of file descriptors and processes, the first process can stop at 35.
*/
// �ļ��������ض���(����ӳ��ȽϺ�)
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
	//�ӹܵ��ж�ȡ����(�����������) 
	if(read(0,&tmp_old,sizeof(int)))
	{
		//��һ��һֱ��������
		printf("prime %d\n",tmp_old);
		//������ɸѡ����������(���ǵ�һ�����ı���)
		pipe(fd);//����һ���ܵ�
		if(fork()==0)
		{
			mapping(1,fd);
			//�ӽ���
			while(read(0,&tmp_new,sizeof(int))){
				//���ǵ�һ�������ı������������� 
				if(tmp_new%tmp_old!=0)
				{
					write(1,&tmp_new,sizeof(int)); 
				}
			} 
		} 
		else
		{
	        //������ 
	        //�ȴ��ӽ������ 
	        wait((int*)0); 
	        mapping(0,fd);
	        //���� 
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
		//�ӽ���
		mapping(1,fd);
		for(int i-2;i<=35;i++)
		{
			write(1,&i,sizeof(&i));
		}
	}
	else
	{
		//������
		wait((int*)0);
		mapping(0,fd);
		isprime();
		
	}
	
	exit(0);
 } 

