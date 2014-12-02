#ifndef DEFINITIONS_INCLUDED
#define DEFINITIONS_INCLUDED


#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
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
#include <ctime>
#include <ifaddrs.h>

#define LISTEN      0
#define STDIN       0
#define SND         0
#define RCV         1
#define PORT        6000
#define BUFLEN      HEADER_SIZE + DATA_LOAD * 100
#define HEADER_SIZE 56
#define DATA_LOAD   2048
#define ACK         1
#define DATA        2
#define EOT         3
#define IP_LEN      20
#define P_SIZE      2104
#define TIMEOUT     CLOCKS_PER_SEC * 5
#define SERVER_PORT 6001

struct packet_hdr
{
    int ptype;
    int ack_value;
    int window_size;
    int sequence_number;
    char dest_ip[IP_LEN];
    char src_ip[IP_LEN];
    char data[DATA_LOAD];
};

#endif // DEFINITIONS_INCLUDED
