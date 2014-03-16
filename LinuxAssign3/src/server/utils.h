
int write_pipe(int fd, const void *buf, size_t count);
int read_pipe(int fd, void *buf, size_t count);
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle);

int create_accept_socket();
int accept_new_client(int listen_sd);
int tcp_recieve(int sockfd, int bytes_to_read, char * packet);