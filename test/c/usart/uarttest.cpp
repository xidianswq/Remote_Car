#include "uart.h"
#include<iostream>
#include<cstring>
int main()
{
    Uart uart;
    char msg[]="hello";
    // const char *str="hello";
    // strcpy(msg,str);
    uart.sendUart((unsigned char*)msg);
}