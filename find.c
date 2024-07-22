/*
Write a simple version of the UNIX find program: 
find all the files in a directory tree with a specific name. Your solution should be in the file user/find.c.
*/ 
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void find(char * dir,char *file)
{
	char buf[512],*p;
	int fd;
	struct dirent de;
	struct stat st;
	//���ж�·���Ƿ���� ��O_RDNOLY=0 
	if((fd=open(dir,0))<0)
	{
		fprintf(2,"find: cannot open %s\n",dir);
		return;
	}
	//ʹ��fstat�򿪸�·��
	if(fstat(fd,&st)<0) 
	{
		fprintf(2,"find: cannot fstat %s\n",dir);
		close(fd); 
		return;
	}
	//�����ж��������ļ�����Ŀ¼
	//����Ŀ¼���� 
	if(st.type != T_DIR)
	{
		fprintf(2,"find: %s is not a directory\n",dir);
		close(fd);
		return; 
	} 
	// ���·�������Ų��뻺�������򱨴���ʾ
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
    {
     	fprintf(2, "find: directory too long\n");
        // �ر��ļ������� fd
        close(fd);
        return;
    }
    strcpy(buf,dir);
	p=buf+strlen(buf);
	*p++ = '/';
	//ѭ�����϶�ȡ֮ǰopen()��Ŀ¼�ڲ����ļ��к��ļ� 
	while(read(fd,&de,sizeof(de))==sizeof(de))
	{
		if(de.inum == 0)
		   continue;
		if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
		memmove(p,de.name,DIRSIZ);
		p[DIRSIZ]=0;
		if(stat(buf,&st)<0)
		{
			fprintf(2, "find: cannot stat %s\n", buf);
            continue;
		}
		//�������ļ��м����ݹ� 
		if (st.type == T_DIR)
		{
			find(buf,file);
		}
	    else if (st.type == T_FILE && !strcmp(de.name, file))
        {
            // ��ӡ��������ŵ�·��
            printf("%s\n", buf);
        } 
	}
}

int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		fprintf(2,"usage: find dirName fileNmae\n!");
		exit(1);
		//�쳣�˳� 
	}
	find(argv[1],argv[2]);
	exit(0);
}
