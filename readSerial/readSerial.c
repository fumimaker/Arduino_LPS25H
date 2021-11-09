#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#define SERIAL_PORT "/dev/ttyACM0"

int main(int argc, char *argv[])
{
    printf("Start\n");

    unsigned char msg[] = "serial port open...\n";
    unsigned char buf[255]; // バッファ
    int fd;                 // ファイルディスクリプタ
    struct termios tio;     // シリアル通信設定
    int baudRate = B115200;
    int i;
    int len;
    int ret;
    int size;

    fd = open(SERIAL_PORT, O_RDWR); // デバイスをオープンする
    if (fd < 0)
    {
        printf("open error\n");
        return -1;
    }else{
        printf("open ok\n");
    }

    tio.c_cflag += CREAD;  // 受信有効
    tio.c_cflag += CLOCAL; // ローカルライン（モデム制御なし）
    tio.c_cflag += CS8;    // データビット:8bit
    tio.c_cflag += 0;      // ストップビット:1bit
    tio.c_cflag += 0;      // パリティ:None

    cfsetispeed(&tio, baudRate);
    cfsetospeed(&tio, baudRate);

    cfmakeraw(&tio); // RAWモード

    tcsetattr(fd, TCSANOW, &tio); // デバイスに設定を行う

    ioctl(fd, TCSETS, &tio); // ポートの設定を有効にする
    
    char buffer[1];
    buffer[0]='C';
    
    write(fd, buffer, 1);
    printf("send ok\n");

    // 送受信処理ループ
    len = read(fd, buf, sizeof(buf));
    if(len==0){
        printf("zero!\n");
    }
    if(len<0){
        printf("read error...\n");
    }
    for(i = 0; i < len; i++) {
        printf("%02X", buf[i]);
    }
    printf("\n");

    close(fd); // デバイスのクローズ
    return 0;
}