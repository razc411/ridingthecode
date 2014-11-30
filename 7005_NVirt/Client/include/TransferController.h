#ifndef TRANSFERCONTROLLER_H
#define TRANSFERCONTROLLER_H

#include <cstring>

class TransferController
{
    public:

        TransferController(char * destip, size_t transfer_size, size_t win_size);
        virtual ~TransferController();

        int current_seq;
        int last_acked_seq;
        int current_ack;
        int current_window;
        size_t window_size;
        size_t packet_size;
        size_t transfer_size;

        size_t size(){return strlen(buffer);}
        int readNextPacket(struct packet_hdr * packet);
        void readPacket(char &packet, int sequence_num);
        bool verifyAck(struct packet_hdr packet);
        void write_packet_buffer(const char ipdest[20]);

    protected:
    private:
        char * ipdest;
        char * buffer;
};

#endif // TRANSFERCONTROLLER_H
