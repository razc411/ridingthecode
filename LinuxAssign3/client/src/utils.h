
int write_pipe(int fd, const void *buf, size_t count);
int read_pipe(int fd, void *buf, size_t count);
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle);

int create_accept_socket();
int accept_new_client(int listen_sd);

int write_packet(int sockfd, int type, void * packet);
void serialize_cjoin(void* packet, int sockfd);
void serialize_smsg_skick(void* packet, int sockfd);
void serialize_cinfo(void* packet, int sockfd);

void* tcp_recieve(int sockfd, int * type);
void * recieve_smsg_skick(int sockfd);
void * recieve_cinfo(int sockfd);
void * recieve_cjoin(int sockfd);
void * recieve_cmsg(int sockfd);
void * recieve_cquit(int sockfd);
void rcv_variable(int sockfd, void * incoming, int size);