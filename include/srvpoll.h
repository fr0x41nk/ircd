#define PORT 6668
#define MAX_CLIENTS 256
#define BUFF_SIZE 4096



typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_MSG,
    STATE_GOODBYE,
    STATE_DISCONNECTED,
} state_e;

typedef struct {
    int fd;
    state_e state;
    char buffer[4096];
} clientstate_t;

void init_clients(clientstate_t* states);
