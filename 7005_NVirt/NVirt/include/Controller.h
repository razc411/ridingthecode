#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "CircularBuffer.h"
#include "NoiseController.h"
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
        void notify(int type, struct packet_hdr pkt);

    protected:
    private:
        int ctrl_socket;
        int ctrl_socket2;
        CircularBuffer * c_buffer;
        NoiseController * n_control;
};
#endif // CONTROLLER_H
