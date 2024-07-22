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
	//先判断路径是否存在 ，O_RDNOLY=0 
	if((fd=open(dir,0))<0)
	{
		fprintf(2,"find: cannot open %s\n",dir);
		return;
	}
	//使用fstat打开该路径
	if(fstat(fd,&st)<0) 
	{
		fprintf(2,"find: cannot fstat %s\n",dir);
		close(fd); 
		return;
	}
	//进而判断类型是文件还是目录
	//不是目录类型 
	if(st.type != T_DIR)
	{
		fprintf(2,"find: %s is not a directory\n",dir);
		close(fd);
		return; 
	} 
	// 如果路径过长放不入缓冲区，则报错提示
    if(strlen(dir) + 1 + DIRSIZ + 1 > sizeof buf)
    {
     	fprintf(2, "find: directory too long\n");
        // 关闭文件描述符 fd
        close(fd);
        return;
    }
    strcpy(buf,dir);
	p=buf+strlen(buf);
	*p++ = '/';
	//循环不断读取之前open()打开目录内部的文件夹和文件 
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
		//遇到了文件夹继续递归 
		if (st.type == T_DIR)
		{
			find(buf,file);
		}
	    else if (st.type == T_FILE && !strcmp(de.name, file))
        {
            // 打印缓冲区存放的路径
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
		//异常退出 
	}
	find(argv[1],argv[2]);
	exit(0);
}
