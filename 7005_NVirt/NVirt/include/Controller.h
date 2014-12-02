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
        const char * get_readable_type(int type);

    protected:
    private:
        int ctrl_socket;
        CircularBuffer * c_buffer;
        NoiseController * n_control;
};
#endif // CONTROLLER_H
