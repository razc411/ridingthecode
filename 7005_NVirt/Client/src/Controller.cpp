#include "../include/Controller.h"
/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		CircularBuffer.cpp
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
Controller::Controller(): data_ready(0),
    c_buffer(new CircularBuffer(BUFLEN)), cmd_control(new CommandController())
{
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
Controller::~Controller()
{
    delete c_buffer;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::execute()
{
    int maxfd = 0, ready = 0;
    ctrl_socket = create_udp_socket(PORT);

    fd_set rcvset_t, rcvset, sndset;
    FD_ZERO(&rcvset);
    FD_ZERO(&sndset);

    FD_SET(STDIN, &rcvset);
    FD_SET(ctrl_socket, &rcvset);
    FD_SET(ctrl_socket, &sndset);

    maxfd = (STDIN > ctrl_socket) ? STDIN : ctrl_socket;

    while(true)
    {
        rcvset_t  = rcvset;
        ready = select(maxfd + 1, &rcvset_t, &sndset, NULL, NULL);

        if(FD_ISSET(STDIN, &rcvset_t))
        {
            data_ready = n_control->check_command();
            //temporary testing with dummy packets
            if(data_ready)
            {
                write_dummy_buffer();
            }
        }

        if(FD_ISSET(ctrl_socket, &sndset))
        {
            transmit_data();
        }

        if(FD_ISSET(ctrl_socket, &rcvset_t))
        {
            recieve_data();
        }
    }
}

void write_dummy_buffer()
{
    for(int i = 0; i < 50; i++)
    {
        struct packet_hdr dummy_packet;
        dummy_packet->dest_ip = 192.168.0.1;
        dummy_packet->src_ip = 192.168.0.2;
        dummy_packet->ack_value = i;
        dummy_packet->sequence_number = i + 1;
        dummy_packet->window_size = 50;
        dummy_packet->ptype = DATA;
        memset(&dummy_packet->data, 'A', sizeof(dummy_packet->data));
        c_buffer->write(&dummy_packet, sizeof(dummy_packet));
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::recieve_data()
{
    int total_read = 0, n = 0, bytes_to_read = P_SIZE;
    struct packet_hdr packet;

    char * buffer = (char*) malloc(sizeof(P_SIZE));
    memset(buffer, 0, P_SIZE);

    while ((n = read(ctrl_socket, buffer, bytes_to_read)) < P_SIZE)
    {
        buffer += n;
        bytes_to_read -= n;
        total_read += n;
    }

    memcpy(buffer, &packet, P_SIZE);
    notify(RCV, packet);

    free(buffer);

    return 1;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::transmit_data()
{
    //if(cmd_control->files.size() == 0){return 0;}
    if(!data_ready){return 0;}
    if(last_ack != current_ack){return 3;}

    struct packet_hdr packet;
    struct sockaddr_in server;
    struct hostent *hp;

    char * buffer = (char*) malloc(sizeof(P_SIZE));
    memset(buffer, 0, P_SIZE);

    c_buffer->read(buffer, P_SIZE);
    memcpy(&packet, buffer, P_SIZE);

    current_ack = packet.ack_value;

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if ((hp = gethostbyname(packet.dest_ip)) == NULL)
    {
        perror("Can't get server's IP address\n");
        return -1;
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    do{
        if(sendto(ctrl_socket, (void*)&packet, P_SIZE, 0, (struct sockaddr *)&server, sizeof(server)))
        {
            perror("sendto failure");
        }
    }   while(!recieve_ack(packet.ack_value, packet.sequence_number));

    notify(SND, packet);

    free(buffer);

    return 0;
}

int recieve_ack(int ack_value, int sequence_number)
{
    int total_read = 0, n = 0, bytes_to_read = P_SIZE;
    struct packet_hdr packet;

    char * buffer = (char*) malloc(sizeof(P_SIZE));
    memset(buffer, 0, P_SIZE);

    while ((n = read(ctrl_socket, buffer, bytes_to_read)) < P_SIZE)
    {
        buffer += n;
        bytes_to_read -= n;
        total_read += n;
    }

    memcpy(buffer, &packet, P_SIZE);
    notify(RCV, packet);

    free(buffer);

    if(ack_value == packet.ack_value && packet.ptype == ACK){
        return 1;
    }

    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::create_udp_socket(int port)
{
    int udp_socket;
    struct sockaddr_in server;

    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror ("Can't create a socket\n");
        exit(1);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(udp_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror ("Can't bind name to socket");
        exit(1);
    }

    cout << "Established UDP socket " << udp_socket << " on port " << port << endl;
    return udp_socket;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::notify(int type, struct packet_hdr pkt)
{
    string packet_type;
    string type_str = (type > 0) ? "RECV | " : "SND | ";

    switch(pkt.ptype)
    {
    case ACK:
        packet_type = "ACK";
        break;
    case EOT:
        packet_type = "EOT";
        break;
    case DATA:
        packet_type = "DATA";
        break;
    }

    cout << type_str << "PKT: " << packet_type << " SRC: " << pkt.src_ip << " DEST: " << pkt.dest_ip
    << " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;

    n_control->log_descriptor << type_str << "PKT: " << packet_type << " SRC: " << pkt.src_ip
    << " DEST: " << pkt.dest_ip << " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;
}
