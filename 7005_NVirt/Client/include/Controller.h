#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "CommandController.h"
#include "TransferController.h"
#include "definitions.h"

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
