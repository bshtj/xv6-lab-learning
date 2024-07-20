// Lab Xv6 and Unix utilities
// sleep.c

// 引入声明类型的头文件
#include "kernel/types.h"
// 引入声明系统调用和 user/ulib.c 中其他函数的头文件
#include "user/user.h"

int main(int argc, char *argv[])
{
    // 如果命令行参数不等于2个，则打印错误信息
    if (argc != 2)
    {
        // 系统调用 write(int fd, char *buf, int n) 函数输出错误信息
        // 参数 fd 是文件描述符，0 表示标准输入，1 表示标准输出，2 表示标准错误
        // 参数 buf 是程序中存放写的数据的字符数组
        // 参数 n 是要传输的字节数
        // 所以这里调用 user/ulib.c 的 strlen() 函数获取字符串长度字节数
        write(2, "Usage: sleep time\n", strlen("Usage: sleep time\n"));
        exit(1);
    }
    // 把字符串型参数转换为整型
    int time = atoi(argv[1]);
    // 调用系统调用 sleep 函数，传入整型参数
    sleep(time);
    // 正常退出程序
    exit(0);
}

