#include <cstdio>
#include <cstdlib> // exit()
#include <cstring> // memset()
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h> // struct sockaddr_un
#include <netdb.h>

#define VPERROR(...) \
    do { \
        fprintf(stderr, "%s() L%d: ", __func__, __LINE__); \
        perror(__VA_ARGS__); \
    } while (false)

#define VPRINTF(...) \
    do { \
        printf("%s() L%d: ", __func__, __LINE__); \
        printf(__VA_ARGS__); \
    } while (false)


int connect_gate(const char* path)
{

    int gate;

    if ( (gate = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        VPERROR("socket error\n");
        exit(-1);
    }

    struct sockaddr_un gate_addr;
    memset(&gate_addr, 0, sizeof(gate_addr));

    gate_addr.sun_family = AF_UNIX;
    
    /* strlcpy() will be safer than strnpcy() 
       because strlcpy() copies (size-1) bytes at most,
       and guarantee that the destination terminates with NULL
     */
    strlcpy(gate_addr.sun_path, path, sizeof(gate_addr.sun_path));

    if (connect(gate, reinterpret_cast<struct sockaddr*>(&gate_addr), sizeof(gate_addr)))
    {
        VPERROR("connect error\n");
        exit(-1);
    }

    return gate;
}


int sendfd(int gate, int fd, void* message, int message_len)
{
    struct iovec iov;

    iov.iov_base = message;
    iov.iov_len = message_len;

    char cmsgbuf[CMSG_SPACE(sizeof(int))];

    struct cmsghdr *cmsg = reinterpret_cast<struct cmsghdr*>(cmsgbuf);
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *((int*)CMSG_DATA(cmsg)) = fd;

    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    msg.msg_flags = 0;

    if (sendmsg(gate, &msg, 0) < 0)
    {
        VPERROR("sendmsg() error\n");
        exit(-1);
    }

    return 0;
}


int main()
{
    const char* path = "/tmp/local.sock";

    int gate = connect_gate(path);

    VPRINTF("gate %d\n", gate);
 
    pid_t msg = getpid();
    sendfd(gate, 1, &msg, sizeof(msg));
    perror("end");

    return 0;
}
