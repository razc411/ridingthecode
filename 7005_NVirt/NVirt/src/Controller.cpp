#include "../include/Controller.h"
/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		Controller.cpp
--
--	Functions: Controller()
--             ~Controller()
--             void execute()
--             size_t read(char *data, size_t bytes)
--             int recieve_data()
--             int transmit_data()
--             int create_udp_socket(int port)
--             void notify(int type, struct packet_hdr pkt)
--             const char * get_readable_type(int type)
--
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
--	Main controller for the network loop. Handles sending, recieving and user commands through select.
-------------------------------------------------------------------------------------------------------------------------*/
using namespace std;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: Controller
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: Controller()
--
--      RETURNS: Nothing, ctor.
--
--      NOTES:
--      Creates a new controller object. Instantiates a CircularBuffer of size BUFLEN and a NoiseController.
----------------------------------------------------------------------------------------------------------------------*/
Controller::Controller():
    c_buffer(new CircularBuffer(BUFLEN)), n_control(new NoiseController())
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
--      RETURNS: Nothing, dtor.
--
--      NOTES:
--      Called to destroy a Controller object. Closes any open sockets and deletes the circular buffer and noise controller.
----------------------------------------------------------------------------------------------------------------------*/
Controller::~Controller()
{
    delete c_buffer;
    delete n_control;
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
--      INTERFACE: void execute()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Main select loop for the network side of things. Watches stdin for input and acts on it. Also watches the ctrl_socket
--      for any incoming data or if data is ready to be written.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::execute()
{
    n_control->print_help();

    int maxfd = 0, ready = 0;
    ctrl_socket = create_udp_socket(PORT);

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
--      FUNCTION: recieve_data
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int recieve_data()
--
--      RETURNS: Returns an int, 1 on successful read.
--
--      NOTES:
--      If the circular buffer has room, reads the most recently recieved packet in and notifies the terminal.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::recieve_data()
{
    int total_read = 0, n = 0, bytes_to_read = P_SIZE;
    struct packet_hdr packet;

    if((c_buffer->capacity() - c_buffer->size()) > P_SIZE)
    {
        memset(&packet, 0, P_SIZE);

        while ((total_read += (n = read(ctrl_socket, &packet + (total_read), bytes_to_read))) < P_SIZE)
        {
            bytes_to_read -= n;
        }

        notify(RCV, packet);

        c_buffer->write((char*)&packet, total_read);
    }

    return 1;
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
--      INTERFACE: int transmit_data()
--
--      RETURNS: An int, -1 if the hostname is invalid and -2 if the destination is unreachable. Returns a 0 on successful transfer
--              and a 2 when the packet was dropped by the router.
--
--      NOTES:
--      Transmits data out of the router and determines if the next packet should be dropped or delayed. Notifies the terminal
--      on transmit.
----------------------------------------------------------------------------------------------------------------------*/
int Controller::transmit_data()
{
    struct packet_hdr packet;

    if(c_buffer->size() >= P_SIZE)
    {
        memset(&packet, 0, P_SIZE);
        c_buffer->read((char*)&packet, P_SIZE);

        if(!n_control->packet_drop_check())
        {
            cout << get_readable_type(packet.ptype) <<" Packet SEQ# " << packet.sequence_number <<
            " ACK# " << packet.ack_value << " lost!" << endl;
            return 2;
        }

        struct sockaddr_in server;
        struct hostent *hp;

        bzero((char *)&server, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(CLIENT_PORT);

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
            return -2;
        }

        notify(SND, packet);
    }

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
--                  port - port for the socket to be created on.
--
--      RETURNS: An int descriptor for the socket created.
--
--      NOTES:
--      Creates a UDP socket for both recieving and sending on the specified port. Exits the program if the socket cannot
--      be bound or created.
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
--      FUNCTION: notify
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void Controller::notify(int type, struct packet_hdr pkt)
--                  *pkt - a pointer to the packet_hdr struct to be notified on
--                  type - whether the transfer was a send or recieving transfer. RCV for recieve, SND for send.
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Prints out the data of the packet passed in as *pkt to the terminal.
----------------------------------------------------------------------------------------------------------------------*/
void Controller::notify(int type, struct packet_hdr pkt)
{
    string packet_type;
    string type_str = (type > 0) ? "RECV | " : "SND | ";

    packet_type = get_readable_type(pkt.ptype);

    cout << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip
    << " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;

    n_control->log_descriptor << type_str << "PKT: " << packet_type << " DEST: " << pkt.dest_ip <<
    " ACK# : " << pkt.ack_value << " SEQ# " << pkt.sequence_number << endl;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: get_readable_type
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: const char * get_readable_type(int type)
--                  type - the corresponding value for an EOT, ACK or DATA packet.
--
--      RETURNS: A const char * of the name of the type of packet found from the type.
--
--      NOTES:
--      Takes in a packet type # and returns the name of the packet.
----------------------------------------------------------------------------------------------------------------------*/
const char * Controller::get_readable_type(int type)
{
     switch(type)
    {
    case ACK:
        return "ACK";
    case EOT:
        return "EOT";
    case DATA:
        return"DATA";
    default:
        return "UNKNOWN";
    }
}
