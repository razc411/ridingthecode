#ifndef DEFINITIONS_INCLUDED
#define DEFINITIONS_INCLUDED


#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <list>
#include <sstream>
#include <cstdlib>
#include <string>
#include <memory>

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

struct packet_hdr
{
    int8_t ptype;
    int32_t ack_value;
    int32_t window_size;
    int32_t sequence_number;
    char dest_ip[IP_LEN];
    char data[DATA_LOAD];
};

#endif // DEFINITIONS_INCLUDED
