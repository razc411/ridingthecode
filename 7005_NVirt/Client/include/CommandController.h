#ifndef COMMANDCONTROLLER_H
#define COMMANDCONTROLLER_H

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>

class NoiseController
{
    public:
        std::ofstream log_descriptor;
        queue<std::string> files;

        NoiseController();
        virtual ~NoiseController();

        int check_command();
        void set_descriptor(std::string line);

    protected:
    private:
        queue<std::string> files;
};

#endif // NOISECONTROLLER_H
