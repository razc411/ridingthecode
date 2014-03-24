#include "client_defs.h"

void* InputManager(void * indata)
{
    THREAD_DATA * input_data = (THREAD_DATA*) indata;

    int type;
    char *   temp = NULL;
    char     cmd[MAX_STRING];
    size_t   nbytes = MAX_STRING;
    
    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/join") == 0)
        {
            C_JOIN_PKT * cjoin = (C_JOIN_PKT*) malloc(sizeof(C_JOIN_PKT));
            if(sscanf(temp, "%s %s", cmd, cjoin->channel_name) == 2)
            {
                write_packet(input_data->write_pipe, CLIENT_JOIN_PKT, cjoin);
            }
            else
            {
                printf("Improper syntax, expected: /join <channel name>\n");
            }
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
           C_MSG_PKT * c_msg = (C_MSG_PKT*) malloc(sizeof(C_MSG_PKT));
           memcpy(c_msg->msg, temp, MAX_STRING);
           write_packet(input_data->write_pipe, CLIENT_MSG_PKT, c_msg);
        }
    }

    return NULL;
}