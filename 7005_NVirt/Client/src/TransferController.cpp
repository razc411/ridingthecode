#include "TransferController.h"

TransferController::TransferController(std::string ip_dest, size_t t_size, size_t win_size):
    current_ack(-1), current_seq(1), current_window(1), ipdest(ip_dest.c_str()),
    transfer_size(t_size), window_size(win_size), packet_size(P_SIZE)
{
    buffer = (char*)malloc(t_size);
    memset(buffer, 0, sizeof(t_size));
    write_packet_buffer();
}

TransferController::~TransferController()
{
    free(buffer);
}

int TransferController::readNextPacket(struct packet_hdr * packet)
{
    if(current_seq <= window_size * current_window){
        memcpy((void*)packet, &buffer[(current_seq - 1) * packet_size], packet_size);
        current_seq++;
        return 1;
    }
    return 0;
}

void TransferController::readPacket(char &packet, int sequence_num)
{
    memcpy((void*)packet, &buffer[(sequence_num - 1) * packet_size], packet_size);
}

int TransferController::verifyAck(struct packet_hdr packet)
{
    if(packet.ack_value == current_ack + 1)
    {
        current_ack++;

        if(((current_window * window_size)) == (current_ack + 1))
        {
            current_window++;
            return 2;
        }
        return 1;
    }
    return 0;
}

void TransferController::write_packet_buffer()
{
    for(int i = 0; i < transfer_size/packet_size; i++)
    {
        struct packet_hdr packet_data;
        memcpy(packet_data.dest_ip, &ipdest, sizeof(ipdest));
        packet_data.ack_value = i;
        packet_data.sequence_number = i + 1;
        packet_data.window_size = window_size;
        packet_data.ptype = DATA;
        memset(&packet_data.data, 'A', sizeof(packet_data.data));
        memcpy(&buffer[packet_size * i], &packet_data, packet_size);

        if(i == (transfer_size/packet_size - 1))
        {
            memset(&packet_data, 0, sizeof(struct packet_hdr));
            memcpy(packet_data.dest_ip, &ipdest, sizeof(ipdest));
            packet_data.ptype = EOT;
            memcpy(&buffer[packet_size * (i + 1)], &packet_data, packet_size);
        }
    }

}
