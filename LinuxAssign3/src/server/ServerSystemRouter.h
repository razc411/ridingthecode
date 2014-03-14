#define TCP_PORT 			43534
#define MAX_CHANNELS 		50
#define READ 				0
#define WRITE 				1
#define MAX_CHANNEL_NAME 	20

typedef struct{
	int **client_list;
	char **channel_name_list;
	int channel_num;
	sem_t channel_name_sem;
	sem_t client_list_sem;
	int write_pipe;
	int read_pipe;
} CHANNEL_DATA;

typedef struct{
	int **client_list;
	sem_t client_list_sem;
	int write_pipe;
	int read_pipe;
} CMANAGERDATA;