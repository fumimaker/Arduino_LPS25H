#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <unistd.h>

int main(){
    /* シリアルポートオープン */
    int fd = serialOpen("/dev/ttyAMA0",115200);    
    
    wiringPiSetup();
    fflush(stdout);
    
    if(fd<0){
        printf("can not open serialport");
    }

    serialPutchar(fd, 'C');
    while(serialDataAvail(fd)==0);
    printf("%d\n",serialDataAvail(fd));
    printf("recive : %x" , serialGetchar(fd) );
    fflush(stdout);
    return 0;
}