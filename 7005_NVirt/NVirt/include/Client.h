#ifndef CLIENT_H
#define CLIENT_H


class Client
{
    public:
        Client(int socket);
        virtual ~Client();
        int get_socket(){return _socket;}
        string get_dest(){return _dest_ip_addr;}
        int get_status(){return _status;}

        CircularBuffer c_buffer;

    protected:
    private:
        int _socket;
        string _dest_ip_addr;
        int _status;
        int _last_sent

};

#endif // CLIENT_H
