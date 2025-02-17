
/************************************/
/* @auteur Mathieu Bahin            */
/* @date_création mars 2020         */
/* @version 1.0                     */
/* @email bahin.mathieu@gmail.com   */
/************************************/

#ifndef _UART_H
#define _UART_H


// Define Constants
const char *uart_target = "/dev/ttyTHS1";
#define     NSERIAL_CHAR   256
#define     VMINX          1
#define     BAUDRATE       B115200


class Uart {
private:
  /* data */
  int fid;
public:
  char serial_message[NSERIAL_CHAR];


  Uart ();
  void sendUart(char *msg);
  void readUart();
  void closeUart();

};
#endif

//begin::uart部分***************************************************************************************************
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>       // Used for UART
#include <sys/fcntl.h>    // Used for UART
#include <termios.h>      // Used for UART
#include <string.h>

using namespace std;

Uart :: Uart (){
  int ii, jj, kk;
  struct termios  port_options;   // Create the structure

  tcgetattr(fid, &port_options);	// Get the current attributes of the Serial port
  fid = open(uart_target, O_RDWR | O_NOCTTY );
  tcflush(fid, TCIFLUSH);
  tcflush(fid, TCIOFLUSH);
  if (fid == -1)
  {
    printf("**Error - Unable to open UART**.  \n=>Ensure it is not in use by another application\n=>Ensure proper privilages are granted to accsess /dev/.. by run as a sudo\n");
  }
  port_options.c_cflag &= ~PARENB;            // Disables the Parity Enable bit(PARENB),So No Parity
  port_options.c_cflag &= ~CSTOPB;            // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit
  port_options.c_cflag &= ~CSIZE;	            // Clears the mask for setting the data size
  port_options.c_cflag |=  CS8;               // Set the data bits = 8
  port_options.c_cflag &= ~CRTSCTS;           // No Hardware flow Control
  port_options.c_cflag |=  CREAD | CLOCAL;                  // Enable receiver,Ignore Modem Control lines
  port_options.c_iflag &= ~(IXON | IXOFF | IXANY);          // Disable XON/XOFF flow control both input & output
  port_options.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  // Non Cannonical mode
  port_options.c_oflag &= ~OPOST;                           // No Output Processing

  port_options.c_lflag = 0;               //  enable raw input instead of canonical,

  port_options.c_cc[VMIN]  = VMINX;       // Read at least 1 character
  port_options.c_cc[VTIME] = 0;           // Wait indefinetly

  cfsetispeed(&port_options,BAUDRATE);    // Set Read  Speed
  cfsetospeed(&port_options,BAUDRATE);    // Set Write Speed

  // Set the attributes to the termios structure
  int att = tcsetattr(fid, TCSANOW, &port_options);

  if (att != 0 )
  {
    printf("\nERROR in Setting port attributes");
  }
  else
  {
    printf("\nSERIAL Port Good to Go.\n");
  }

  // Flush Buffers
  tcflush(fid, TCIFLUSH);
  tcflush(fid, TCIOFLUSH);

}

void Uart :: sendUart(char *msg){
  //--------------------------------------------------------------
  // TRANSMITTING BYTES
  //--------------------------------------------------------------
  char tx_buffer[256]={0};

  for (int i = 0; msg[i]!='\0'; i++) {
    tx_buffer[i] = msg[i];
  }
  printf("%s\n",tx_buffer);

  if (fid != -1)
  {
    int count = write(fid, &tx_buffer[0], strlen((const char*)tx_buffer));		//Filestream, bytes to write, number of bytes to write

    printf("Count = %d\n", count);

    if (count < 0)  printf("UART TX error\n");
  }

}

void Uart :: readUart(){

  //--------------------------------------------------------------
  // RECEIVING BYTES - AND BUILD MESSAGE RECEIVED
  //--------------------------------------------------------------
  unsigned char rx_buffer[VMINX];
  bool          pickup = true;
  int ii;
  int           rx_length;
  int           nread = 0;

  //更新缓存
  tcflush(fid, TCIOFLUSH);
  //清空接收区
  for (ii=0; ii<NSERIAL_CHAR; ii++)  serial_message[ii]='\0';

  while (pickup && fid != -1)
  {
    rx_length = read(fid, (void*)rx_buffer, VMINX);   // Filestream, buffer to store in, number of bytes to read (max)

    if (rx_length>=0)
    {
      if (nread<=NSERIAL_CHAR-1){
        serial_message[nread] = rx_buffer[0];   // Build message 1 character at a time
      }

      if (rx_buffer[0]=='$')   pickup=false;    // # symbol is terminator 自定协议规定以$结尾
    }

    nread++;
  }

}

void Uart :: closeUart(){
  //-------------------------------------------
  //  CLOSE THE SERIAL PORT
  //-------------------------------------------
  close(fid);
}
//end::uart部分***************************************************************************************************