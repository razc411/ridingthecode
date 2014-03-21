#include "client_defs.h"

void* InputManager(void * indata)
{
    THREAD_DATA * input_data = (THREAD_DATA*) indata;

    int type;
    char *   temp = NULL;
    char     cmd[MAX_STRING];
    char     channelname[MAX_CHANNEL_NAME];
    size_t   nbytes = MAX_STRING;

    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/join") == 0)
        {
            type = JOIN_CHANNEL;
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
        else if(strcmp(cmd, "/leave") == 0)
        {
            type = QUIT_CHANNEL;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
        }
        else if(strcmp(cmd, "/exit") == 0)
        {
            type = EXIT;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            break;
        }
        else
        {
           C_MSG_PKT c_msg;
           c_msg.msg = cmd;
           write_packet(input_data->write_pipe, CLIENT_MSG_PKT, &c_msg);
        }
    }

    return NULL;
}