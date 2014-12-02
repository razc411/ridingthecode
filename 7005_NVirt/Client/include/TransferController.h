#ifndef TRANSFERCONTROLLER_H
#define TRANSFERCONTROLLER_H

#include "definitions.h"

class TransferController
{
    public:

        TransferController(std::string destip, int transfer_size, int win_size);
        virtual ~TransferController();
        TransferController(const TransferController &obj);

        int current_seq;
        int current_ack;
        int current_window;
        int window_size;
        int packet_size;
        int transfer_size;

        size_t size(){return strlen(buffer);}
        int readNextPacket(struct packet_hdr * packet);
        void readPacket(char &packet, int sequence_num);
        int verifyAck(struct packet_hdr * packet);
        void write_packet_buffer();
        std::string grab_ip();
        void create_EOT_packet(struct packet_hdr * packet);

    protected:
    private:
        std::string ipdest;
        std::string src_ip;
        char * buffer;
};

#endif // TRANSFERCONTROLLER_H
