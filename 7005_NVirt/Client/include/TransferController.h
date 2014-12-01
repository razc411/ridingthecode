#ifndef TRANSFERCONTROLLER_H
#define TRANSFERCONTROLLER_H

#include "definitions.h"

class TransferController
{
    public:

        TransferController(std::string destip, size_t transfer_size, size_t win_size);
        virtual ~TransferController();
        TransferController(const TransferController &obj);

        int current_seq;
        int current_ack;
        int current_window;
        size_t window_size;
        size_t packet_size;
        size_t transfer_size;

        size_t size(){return strlen(buffer);}
        int readNextPacket(struct packet_hdr * packet);
        void readPacket(char &packet, int sequence_num);
        int verifyAck(struct packet_hdr packet);
        void write_packet_buffer();

    protected:
    private:
        const char * ipdest;
        char * buffer;
};

#endif // TRANSFERCONTROLLER_H
