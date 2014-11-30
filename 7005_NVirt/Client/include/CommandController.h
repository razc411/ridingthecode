#ifndef COMMANDCONTROLLER_H
#define COMMANDCONTROLLER_H

#include "TransferController.h"
#include "definitions.h"


class CommandController
{
    public:
        std::ofstream log_descriptor;
        std::queue<std::string> files;

        CommandController();
        virtual ~CommandController();

        int check_command(std::queue<TransferController*> &qTransfers);
        void set_descriptor(std::string line);
        bool file_exists(const std::string& name);

    protected:
    private:
        int num_packets;
        int window_size;

};

#endif // NOISECONTROLLER_H
