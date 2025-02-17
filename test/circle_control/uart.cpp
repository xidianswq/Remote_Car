/************************************/
/* @auteur Mathieu Bahin            */
/* @date_création mars 2020         */
/* @version 1.0                     */
/* @email bahin.mathieu@gmail.com   */
/************************************/

#include "uart.h"
#include <stdio.h>
#include <unistd.h>       // Used for UART
#include <sys/fcntl.h>    // Used for UART
#include <termios.h>      // Used for UART
#include <string.h>

using namespace std;

Uart :: Uart (){
  int ii, jj, kk;

  // SETUP SERIAL WORLD

  struct termios  port_options;   // Create the structure

  tcgetattr(fid, &port_options);	// Get the current attributes of the Serial port


  //------------------------------------------------
  //  OPEN THE UART
  //------------------------------------------------
  // The flags (defined in fcntl.h):
  //	Access modes (use 1 of these):
  //		O_RDONLY - Open for reading only.
  //		O_RDWR   - Open for reading and writing.
  //		O_WRONLY - Open for writing only.
  //	    O_NDELAY / O_NONBLOCK (same function)
  //               - Enables nonblocking mode. When set read requests on the file can return immediately with a failure status
  //                 if there is no input immediately available (instead of blocking). Likewise, write requests can also return
  //				   immediately with a failure status if the output can't be written immediately.
  //                 Caution: VMIN and VTIME flags are ignored if O_NONBLOCK flag is set.
  //	    O_NOCTTY - When set and path identifies a terminal device, open() shall not cause the terminal device to become the controlling terminal for the process.fid = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode

  fid = open(uart_target, O_RDWR | O_NOCTTY );

  tcflush(fid, TCIFLUSH);
  tcflush(fid, TCIOFLUSH);

  if (fid == -1)
  {
    printf("**Error - Unable to open UART**.  \n=>Ensure it is not in use by another application\n=>Ensure proper privilages are granted to accsess /dev/.. by run as a sudo\n");
  }

  //------------------------------------------------
  // CONFIGURE THE UART
  //------------------------------------------------
  // flags defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html
  //	Baud rate:
  //         - B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200,
  //           B230400, B460800, B500000, B576000, B921600, B1000000, B1152000,
  //           B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
  //	CSIZE: - CS5, CS6, CS7, CS8
  //	CLOCAL - Ignore modem status lines
  //	CREAD  - Enable receiver
  //	IGNPAR = Ignore characters with parity errors
  //	ICRNL  - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
  //	PARENB - Parity enable
  //	PARODD - Odd parity (else even)

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

int main(int argc, char *argv[]) {
  Uart u;
  int i;
  char m[256]="#hello world!$";
  u.sendUart(m);
   
  while (1)
  {
    u.readUart();
    if(strlen(u.serial_message)!=0)
    {
      for(i=0;u.serial_message[i]!='$';i++)    //自定协议规定以$结尾
      {
        printf("%c ",u.serial_message[i]);
      }
    }
    printf("\n");    
  }
  u.closeUart();

  return 0;
}
