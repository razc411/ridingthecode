#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "CircularBuffer.h"
#include "NoiseController.h"
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

#define LISTEN      0
#define STDIN       1
#define SENDER      0
#define RECIEVER    1
#define SND         0
#define RCV         1
#define PORT       6323
#define BUFLEN      HEADER_SIZE + DATA_LOAD * 100
#define HEADER_SIZE 144
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
    char dest_ip[IP_LEN];
    char src_ip[IP_LEN];
    int32_t ack_value;
    int32_t window_size;
    int32_t sequence_number;
    char data[DATA_LOAD];
};

class Controller
{
    public:
        Controller();
        virtual ~Controller();

        void execute();
        int recieve_data();
        int transmit_data();
        int create_udp_socket(int port);
        void notify(int type, struct packet_hdr pkt);

    protected:
    private:
        int ctrl_socket;
        CircularBuffer * c_buffer;
        NoiseController * n_control;
};
#endif // CONTROLLER_H
