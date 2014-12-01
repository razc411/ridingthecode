/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		TransferController.cpp
--
--	Functions: size_t size() const { return size_; }
--             size_t capacity() const { return capacity_; }
--             size_t write(const char *data, size_t bytes);
--             size_t read(char *data, size_t bytes);
--
--	Date:			November 24 2014
--
--	Revisions:
--
--
--	DESIGNERS:		Ramzi Chennafi
--
--
--	PROGRAMMER:		Ramzi Chennafi
--
--	NOTES:
--	A circular buffer for the storing of connection data.
--
-------------------------------------------------------------------------------------------------------------------------*/
using namespace std;
#include "TransferController.h"
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: TransferController
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: TransferController::TransferController(std::string ip_dest, size_t t_size, size_t win_size)
--                  ip_dest - destination ip for the transfer
--                  t_size  - total size of the transfer.
--                  win_size - size of the sliding window in # of packets.
--
--      RETURNS: Nothing. Constructor.
--
--      NOTES:
--      Constructs a TransferController object. Sets default values of variables.
----------------------------------------------------------------------------------------------------------------------*/
TransferController::TransferController(std::string ip_dest, int num_packets, int win_size):
    current_ack(-1), current_seq(1), current_window(1), ipdest(ip_dest),
    transfer_size(num_packets * P_SIZE), window_size(win_size), packet_size(P_SIZE)
{
    buffer = (char*)malloc(transfer_size + P_SIZE);
    memset(buffer, 0, transfer_size + P_SIZE);
    write_packet_buffer();
}

TransferController::TransferController(const TransferController &obj)
{
    current_ack = obj.current_ack;
    current_seq = obj.current_seq;
    current_window = obj.current_window;
    transfer_size = obj.transfer_size;
    window_size = obj.window_size;
    packet_size = obj.packet_size;
    ipdest = obj.ipdest;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: ~TransferController
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: TransferController::~TransferController()
--
--      RETURNS: Nothing, constructs a Controller object.
--
--      NOTES:
--      Destructor for a TransferController object. Frees the data buffer on calling/
----------------------------------------------------------------------------------------------------------------------*/
TransferController::~TransferController()
{
    free(buffer);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: readNextPacket
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int TransferController::readNextPacket(struct packet_hdr * packet)
--                  packet - a reference to an empty packet_hdr to be filled.
--
--      RETURNS: An int, 1 if a packet is read to be read and 0 if there is none.
--
--      NOTES:
--      Constructs a Controller object. Creates a CommandController for use with the Controller.
----------------------------------------------------------------------------------------------------------------------*/
int TransferController::readNextPacket(struct packet_hdr * packet)
{
    if(current_seq <= window_size * current_window){
        memcpy((void*)packet, buffer + ((current_seq - 1) * packet_size), packet_size);
        current_seq++;
        return 1;
    }
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: verifyAck
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int TransferController::verifyAck(struct packet_hdr packet)
--                  packet - the ACK packet to be verified.
--
--      RETURNS: An int, 2 if the window has slided due to the ack being valid, 1 if the ack was valid
--                  and 0 if the ack was invalid.
--
--      NOTES:
--      Checks the validity of the passed in ACK. If the ack is valid, increments the current_ack value. If the ack was
--      the last for the window, also increments the current_window.
----------------------------------------------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: write_packet_buffer
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void TransferController::write_packet_buffer()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Writes the dummy packets to the buffer for the transfer. Once the data is written adds an EOT to the end of the buffer.
----------------------------------------------------------------------------------------------------------------------*/
void TransferController::write_packet_buffer()
{
    for(int i = 0; i < transfer_size/packet_size; i++)
    {
        struct packet_hdr packet_data;
        memcpy(packet_data.dest_ip, ipdest.c_str(), strlen(ipdest.c_str()));
        packet_data.ack_value = i;
        packet_data.sequence_number = i + 1;
        packet_data.window_size = window_size;
        packet_data.ptype = DATA;
        memset(&packet_data.data, 'A', sizeof(packet_data.data));
        memcpy(buffer + (packet_size * i), &packet_data, packet_size);

        if(i == (transfer_size/packet_size - 1))
        {
            memset(&packet_data, 0, sizeof(struct packet_hdr));
            memcpy(packet_data.dest_ip, ipdest.c_str(), strlen(ipdest.c_str()));
            memset(&packet_data.data, 'A', sizeof(packet_data.data));
            packet_data.ptype = EOT;
            memcpy(buffer + (packet_size * (i + 1)), &packet_data, packet_size);

        }
    }

}
