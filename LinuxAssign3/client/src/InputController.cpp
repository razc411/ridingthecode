#include "client_defs.h"

void* InputManager(void * indata)
{
    CMANAGERDATA * input_data = (CMANAGERDATA*) indata;

    uint32_t type;
    char *   temp = NULL;
    char     cmd[MAX_STRING];
    char *   channelname;
    char *   name;
    size_t   nbytes = MAX_STRING;

    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/create") == 0)
        {
            type = CHANNEL_CREATE;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
        else if(strcmp(cmd, "/kick") == 0)
        {
            type = CLIENT_KICK;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s %s", cmd, channelname, name);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
            write_pipe(input_data->write_pipe, name, MAX_USER_NAME);
        }
        else if(strcmp(cmd, "/exit") == 0)
        {
            type = SERVER_EXIT;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            break;
        }
        else if(strcmp(cmd, "/close") == 0)
        {
            type = CHANNEL_CLOSE;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
    }
}