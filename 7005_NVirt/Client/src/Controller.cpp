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
Controller::Controller(): cmd_control(new CommandController())
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
    close(ctrl_socket);
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
            cmd_control->check_command(transfers);
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
    notify_terminal(RCV, packet);

    if(packet.ptype == ACK){
        if(!transfers.front()->verifyAck(packet))
        {

        }
    }
    else if(packet.ptype == EOT){

    }
    else{
        send_ack(packet.sequence_number, packet.dest_ip);
    }

    free(buffer);

    return 1;
}

int Controller::send_ack(int seq, char * dest_ip)
{
    struct packet_hdr dummy_packet;
    struct sockaddr_in server;
    struct hostent *hp;

    memcpy(&dest_ip, dummy_packet.dest_ip, sizeof(dest_ip));
    dummy_packet.ack_value = seq - 1;
    dummy_packet.sequence_number = seq;
    dummy_packet.window_size = 0;
    dummy_packet.ptype = ACK;
    memset(&dummy_packet.data, 'A', sizeof(dummy_packet.data));

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if ((hp = gethostbyname(dummy_packet.dest_ip)) == NULL)
    {
        perror("Can't get server's IP address\n");
        return -1;
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    if(sendto(ctrl_socket, (void*)&dummy_packet, P_SIZE, 0, (struct sockaddr *)&server, sizeof(server)))
    {
        perror("sendto failure");
    }

    notify_terminal(SND, dummy_packet);
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
    if(transfers.size() == 0) {return 0;}

    struct packet_hdr packet;
    struct sockaddr_in server;
    struct hostent *hp;

    if(!(transfers.front()->readNextPacket(&packet))) {return 0;}

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if ((hp = gethostbyname(packet.dest_ip)) == NULL)
    {
        perror("Can't get server's IP address\n");
        return -1;
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    if(sendto(ctrl_socket, (void*)&packet, P_SIZE, 0, (struct sockaddr *)&server, sizeof(server)))
    {
        perror("sendto failure");
    }

    notify_terminal(SND, packet);

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
void Controller::notify_terminal(int type, struct packet_hdr pkt)
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

    cout << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip
    << " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;

    cmd_control->log_descriptor << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip << " ACK# : "
    << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;
}
