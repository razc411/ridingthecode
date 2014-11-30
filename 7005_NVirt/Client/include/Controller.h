#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <cstring>
#include <fstream>
#include <queue>
#include <sstream>
#include "CommandController.h"
#include "TransferController.h"
#include "definitions.h"

struct packet_hdr
{
    int8_t ptype;
    char dest_ip[IP_LEN];
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
        void notify_terminal(int type, struct packet_hdr pkt);

        int send_ack(int seq, char * dest_ip);
    protected:
    private:
        int ctrl_socket;
        CommandController * cmd_control;
        std::queue<TransferController*> transfers;
};
#endif // CONTROLLER_H
