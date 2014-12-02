#ifndef DEFINITIONS_H_INCLUDED
#define DEFINITIONS_H_INCLUDED

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <cstring>

#define STDIN       0
#define SND         0
#define RCV         1
#define PORT        6001
#define BUFLEN      HEADER_SIZE + DATA_LOAD * 100
#define HEADER_SIZE 36
#define DATA_LOAD   2048
#define ACK         1
#define DATA        2
#define EOT         3
#define IP_LEN      20
#define P_SIZE      HEADER_SIZE + DATA_LOAD
#define TIMEOUT     CLOCKS_PER_SEC * 5
#define CLIENT_PORT 6000

struct packet_hdr
{
    int ptype;
    int ack_value;
    int window_size;
    int sequence_number;
    char dest_ip[IP_LEN];
    char data[DATA_LOAD];
};

#endif // DEFINITIONS_H_INCLUDED
