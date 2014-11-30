#ifndef COMMANDCONTROLLER_H
#define COMMANDCONTROLLER_H

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>
#include <queue>

class CommandController
{
    public:
        std::ofstream log_descriptor;
        std::queue<std::string> files;

        CommandController();
        virtual ~CommandController();

        int check_command();
        void set_descriptor(std::string line);
        bool file_exists(const std::string& name);

    protected:
    private:
};

#endif // NOISECONTROLLER_H
