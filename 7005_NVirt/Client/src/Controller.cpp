#include "../include/Controller.h"
/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		Controller.cpp
--
--	Functions: Controller()
--             ~Controller()
--             void execute()
--             void recieve_data()
--             void check_packet(struct packet_hdr * packet)
--             int send_ack(int seq, char * dest_ip)
--             int write_udp_socket(struct packet_hdr * packet)
--             int transmit_data()
--             int create_udp_socket(int port)
--             void notify_terminal(int type, struct packet_hdr * pkt)
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
--	The main network controller for the application. Starts a loop on select and waits for input from STDIN, the recieving
--  socket and the sending socket.
-------------------------------------------------------------------------------------------------------------------------*/
using namespace std;

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: Controller()
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: Controller()
--
--      RETURNS: Nothing, constructs a Controller object.
--
--      NOTES:
--      Constructs a Controller object. Creates a CommandController for use with the Controller.
----------------------------------------------------------------------------------------------------------------------*/
Controller::Controller(): cmd_control(new CommandController())
{

}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: ~Controller
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: ~Controller()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Destructor for the Controller object. Closes any open sockets on calling.
----------------------------------------------------------------------------------------------------------------------*/
Controller::~Controller()
{
    close(ctrl_socket);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: execute
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: execute()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Contains the controlling network loop for the application. Performs a select control for STDIN and receiving/sending
--      on the application's main socket.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::execute()
{
    int maxfd = 0, ready = 0;
    ctrl_socket = create_udp_socket(6000);

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
            cmd_control->check_command();
        }

        if(FD_ISSET(ctrl_socket, &sndset_t))
        {
            transmit_data();
        }

        if(FD_ISSET(ctrl_socket, &rcvset_t))
        {
            recieve_data();
        }

        if((clock() - timer == timeout) && timer_enabled){
            timer_enabled = false;
            cmd_control->transfers.pop_front();
            cout << "Transfer timed out, ending connection." << endl;
        }
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: recieve_data
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: recieve_data()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Called whenever data is waiting on the recieve socket. Reads in a packet, prints data on the packet to the terminal
--      and checks what kind of packet it is and performs the required action.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::recieve_data()
{
    int total_read = 0, n = 0, bytes_to_read = P_SIZE;
    struct packet_hdr packet;

    while ((total_read += (n = read(ctrl_socket, &packet + total_read, bytes_to_read))) < P_SIZE)
    {
        bytes_to_read -= n;
    }

    notify_terminal(RCV, &packet);

    check_packet(&packet);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:check_packet
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void check_packet(struct packet_hdr * packet)
--                  packet - the packet to be checked for type.
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Checks whether the passed in packet is either an ACK, EOT or DATA packet and performs the required actions.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::check_packet(struct packet_hdr * packet)
{
    if(packet->ptype == ACK){
        if(!cmd_control->transfers.front()->verifyAck(packet))
        {
            cmd_control->transfers.front()->current_seq = cmd_control->transfers.front()->current_ack + 2;
        }else{
            timer_enabled = false;
        }
    }
    else if(packet->ptype == EOT){
        cmd_control->transfers.pop_front();
        cout << "Transfer completed." << endl;
        timer_enabled = false;
    }
    else{
        send_ack(packet->sequence_number, packet->dest_ip);
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: send_ack
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int send_ack(int seq, char * dest_ip)
--                  dest_ip - a character array containing the destination IP
--                  seq - the sequence number of the ack to be sent
--
--      RETURNS: 0 if ack send was successful, -2 on invalid hostname and -1 if the packet destination is unreachable.
--
--      NOTES:
--      Crafts an ack with the requested sequence number and destination ip then sends it to the destination ip.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::send_ack(int seq, char * dest_ip)
{
    struct packet_hdr ack_packet;

    memcpy(ack_packet.dest_ip, dest_ip, strlen(dest_ip));
    ack_packet.ack_value = seq - 1;
    ack_packet.sequence_number = seq;
    ack_packet.window_size = 0;
    ack_packet.ptype = ACK;
    memset(&ack_packet.data, 'A', sizeof(ack_packet.data));

    return write_udp_socket(&ack_packet);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: write_udp_socket
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int Controller::write_udp_socket(struct packet_hdr * packet)
--                  *packet - a pointer to the packet structure to be written to the socket.
--
--      RETURNS: An int, 0 if transfer was successful, -1 if destination unreachable and -2 if the host was invalid.
--
--      NOTES:
--      Sends a packet over UDP to the specified host.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::write_udp_socket(struct packet_hdr * packet)
{
    struct sockaddr_in server;
    struct hostent *hp;

    bzero((char *)&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(6234);

    if((hp = gethostbyname("127.0.0.1")) == NULL)
    {
        return -2;
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    if(sendto(ctrl_socket, (void*)packet, P_SIZE, 0, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("sendto failure");
        return -1;
    }

    notify_terminal(SND, packet);
    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: transmit_data
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int Controller::transmit_data()
--
--      RETURNS: An int, -1 if a packet is ready to be sent, -2 if a transfer failed and 0 on success.
--
--      NOTES:
--      Retrieves the next packet to be sent and sends it over UDP. If the transfer fails the transfer is removed from
--      the list of ongoing transfers.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::transmit_data()
{
    if(cmd_control->transfers.size() == 0) {
            return 0;
    }

    struct packet_hdr packet;

    if(!(cmd_control->transfers.front()->readNextPacket(&packet))) {
        return -1;
    }

    int err;

    if((err = write_udp_socket(&packet)) <= -1)
    {
        delete cmd_control->transfers.front();
        cmd_control->transfers.pop_front();
        cout << "Invalid IP, abandoning transfer." << endl;
        return -2;
    }

    timer = clock();
    timer_enabled = true;

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: create_udp_socket
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int Controller::create_udp_socket(int port)
--                  port - the port for the udp socket to be created on.
--
--      RETURNS: An int for the socket descriptor created.
--
--      NOTES:
--      Creates the udp socket for the program on the specified port.
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

    if (::bind(udp_socket, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror ("Can't bind name to socket");
        exit(1);
    }

    cout << "Established UDP socket " << udp_socket << " on port " << port << endl;
    return udp_socket;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: notify_terminal
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void Controller::notify_terminal(int type, struct packet_hdr * pkt)
--                  *pkt - a pointer to the packet_hdr struct to be notified on
--                  type - whether the transfer was a send or recieving transfer. RCV for recieve, SND for send.
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Prints out the data of the packet passed in as *pkt to the terminal.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::notify_terminal(int type, struct packet_hdr * pkt)
{
    string packet_type;
    string type_str = (type > 0) ? "RECV | " : "SND | ";

    switch(pkt->ptype)
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

    cout << type_str << "PKT: " << packet_type << " DEST: " << pkt->dest_ip
    << " ACK# : " << pkt->ack_value << " SEQ# " << pkt->sequence_number << endl;

    cmd_control->log_descriptor << type_str << "PKT: " << packet_type << " DEST: " << pkt->dest_ip << " ACK# : "
    << pkt->ack_value << " SEQ# " << pkt->sequence_number << endl;
}
