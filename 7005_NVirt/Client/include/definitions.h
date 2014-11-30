#ifndef DEFINITIONS_INCLUDED
#define DEFINITIONS_INCLUDED

#define LISTEN      0
#define STDIN       1
#define SENDER      0
#define RECIEVER    1
#define SND         0
#define RCV         1
#define PORT        6323
#define BUFLEN      HEADER_SIZE + DATA_LOAD * 100
#define HEADER_SIZE 176
#define DATA_LOAD   2048
#define ACK         1
#define DATA        2
#define EOT         3
#define IP_LEN      20
#define P_DATA      2
#define P_SIZE      HEADER_SIZE + DATA_LOAD


#endif // DEFINITIONS_INCLUDED
