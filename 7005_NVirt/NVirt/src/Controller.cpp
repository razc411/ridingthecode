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
Controller::Controller():
    c_buffer(new CircularBuffer(BUFLEN)), n_control(new NoiseController())
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
    ctrl_socket = create_udp_socket(6234);
    if((ctrl_socket2 = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror ("Can't create a socket\n");
        exit(1);
    }

    fd_set rcvset_t, rcvset, sndset, sndset_t;
    FD_ZERO(&rcvset);
    FD_ZERO(&sndset);

    FD_SET(STDIN, &rcvset);
    FD_SET(ctrl_socket, &rcvset);
    FD_SET(ctrl_socket, &sndset);

    maxfd = (STDIN > ctrl_socket) ? STDIN : ctrl_socket;

    while(true)
    {
        rcvset_t  = rcvset;
        sndset_t  = sndset;
        ready = select(maxfd + 1, &rcvset_t, &sndset_t, NULL, NULL);

        if(FD_ISSET(STDIN, &rcvset_t))
        {
            n_control->check_command();
        }

        if(FD_ISSET(ctrl_socket, &sndset_t))
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

    if((c_buffer->capacity() - c_buffer->size()) > P_SIZE)
    {
        char * buffer = (char*) malloc(P_SIZE);
        memset(buffer, 0, P_SIZE);

        while ((total_read += (n = read(ctrl_socket, buffer, bytes_to_read))) < P_SIZE)
        {
            buffer += n;
            bytes_to_read -= n;
        }

        memcpy(&packet, buffer, total_read);
        notify(RCV, packet);

        c_buffer->write((char*)&packet, total_read);
        free(buffer);
    }

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
    struct packet_hdr packet;
    struct sockaddr_in server;
    struct hostent *hp;

    if(!n_control->packet_drop_check())
    {
        return 2;
    }

    if(c_buffer->size() >= P_SIZE)
    {
        memset(&packet, 0, P_SIZE);

        c_buffer->read((char*)&packet, P_SIZE);

        bzero((char *)&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(6000);

        if ((hp = gethostbyname(packet.dest_ip)) == NULL)
        {
            perror("Can't get server's IP address\n");
            return -1;
        }
        bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);


        sleep(n_control->get_delay());

        if(sendto(ctrl_socket, (void*)&packet, P_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
        {
            perror("sendto failure");
            return -1;
        }

        notify(SND, packet);

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

    cout << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip
    << " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;

    n_control->log_descriptor << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip <<
    " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;
}
