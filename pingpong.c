/*
Write a program that uses UNIX system calls to ''ping-pong'' a byte between two processes over a pair of pipes, one for each direction. 
The parent should send a byte to the child;
 the child should print "<pid>: received ping", where <pid> is its process ID, write the byte on the pipe to the parent, and exit; 
 the parent should read the byte from the child, print "<pid>: received pong", and exit. 
 Your solution should be in the file user/pingpong.c.
*/ 
// �����������͵�ͷ�ļ�
#include "kernel/types.h"
// ��������ϵͳ���ú� user/ulib.c ������������ͷ�ļ�
#include "user/user.h"

int main(int argc,char* argv[])
{
	int p_fd[2],c_fd[2];//���������̺��ӽ��̵��ļ���������2����ΪpipeҪ�����������飬0�Ƕ�ȡ�ˣ�1��д��� 
	pipe(p_fd);
	pipe(c_fd);
	char buf[8];//ͨ������
	//fork()�������̣�����0Ϊ�Խ��̣������̷���pid��ʧ�ܷ���-1
	if(fork()==0)
	{
		//�ӽ���
		read(p_fd[0],buf,4);//�Ӹ����̵Ĺܵ���ȡ4�ֽڵ����ݣ�ping���洢��buf��
		printf("%d: received %s\n",gitpid(),buf); 
		write(c_fd[1],"pong",strlen("pong"));
	} 
	else
	{
		//������ 
		write(p_fd[1], "ping", strlen("ping"));
        wait(NULL);//�ȴ��ӽ��̽��� 
        read(c_fd[0], buf, 4);
        printf("%d: received %s\n", getpid(), buf);
	}
	
	exit(0);
}
