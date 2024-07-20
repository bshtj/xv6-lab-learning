#include "kernel/param.h"
#include "user/user.h"

#define MAX 512

int main(int argc,char *argv[])
{
	//��xargs������������������ 
	if(argc<2)
	{
		fprintf("2,usage: xargs command\n");
		exit(1);//�쳣�˳� 
	}
	//ʹ��cmd�����argv�е����MAXARG������ `exec` ϵͳ�����в������������ 32�� 
	char *cmd[MAXARG];
	int index=0;//��cmd�±�Ϊ0��ʼ�� 
	for(int i=1;i<argc;i++)
	{
		cmd[index++]=argv[i]; 
	} 
	//��ȡ�ܵ������|֮ǰ�Ľ����Ϊ�µ�����
	char buf[MAX];//���建�������|֮ǰ�Ľ��
	int len=read(0,buf,sizeof(buf));//�ӱ�׼�������ж�ȡ
	char temp[MAX];//��ʱ�洢buf�е����ݣ�֮��ֱ�Ӵ浽cmd�� 
	int index_temp=0;
	for(int i=0;i<len;i++)
	{
		if(buf[i]!='\n')
		{
			temp[index_temp++]=buf[i];
		}
		else
		{
			//�����س���
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
