#ifndef NOISECONTROLLER_H
#define NOISECONTROLLER_H

#include "definitions.h"

class NoiseController
{
    public:
        std::ofstream log_descriptor;

        NoiseController();
        virtual ~NoiseController();

        void check_command();
        void set_ploss(std::string line);
        void set_delay(std::string line);
        void set_descriptor(std::string line);
        bool packet_drop_check();

        int get_delay(){return packet_delay;}

    protected:
    private:
        bool set_logging;
        double data_loss;
        int packet_delay;
};

#endif // NOISECONTROLLER_H
