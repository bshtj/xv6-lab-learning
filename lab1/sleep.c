// Lab Xv6 and Unix utilities
// sleep.c

// �����������͵�ͷ�ļ�
#include "kernel/types.h"
// ��������ϵͳ���ú� user/ulib.c ������������ͷ�ļ�
#include "user/user.h"

int main(int argc, char *argv[])
{
    // ��������в���������2�������ӡ������Ϣ
    if (argc != 2)
    {
        // ϵͳ���� write(int fd, char *buf, int n) �������������Ϣ
        // ���� fd ���ļ���������0 ��ʾ��׼���룬1 ��ʾ��׼�����2 ��ʾ��׼����
        // ���� buf �ǳ����д��д�����ݵ��ַ�����
        // ���� n ��Ҫ������ֽ���
        // ����������� user/ulib.c �� strlen() ������ȡ�ַ��������ֽ���
        write(2, "Usage: sleep time\n", strlen("Usage: sleep time\n"));
        exit(1);
    }
    // ���ַ����Ͳ���ת��Ϊ����
    int time = atoi(argv[1]);
    // ����ϵͳ���� sleep �������������Ͳ���
    sleep(time);
    // �����˳�����
    exit(0);
}

