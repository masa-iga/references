#include <cstdio>
#include <cstdlib>
#include <cstring> // memset()
#include <sys/un.h> // struct sockaddr_un
#include <sys/types.h>  // socket()
#include <sys/socket.h> // socket()
#include <unistd.h>
#include <errno.h> // errno

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


int listen_gate(const char* path)
{
    int gate;
    struct sockaddr_un gate_addr;

    unlink(path);

    if ((gate = socket(PF_UNIX, SOCK_DGRAM, 0)) < 0)
    {
        VPERROR("socket error\n");
        exit(-1);
    }

    memset(&gate_addr, 0, sizeof(gate_addr));

    gate_addr.sun_family = AF_UNIX;
    strlcpy(gate_addr.sun_path, path, sizeof(gate_addr.sun_path));

    if (bind(gate, reinterpret_cast<struct sockaddr*>(&gate_addr), sizeof(gate_addr)) < 0)
    {
        VPERROR("bind error\n");
        exit(-1);
    }

    return gate;
}

int recvfd(int gate, void *message, size_t message_len)
{
    struct iovec iov;

    iov.iov_base = message;
    iov.iov_len = message_len;

    struct msghdr msg;
    char cmsgbuf[CMSG_SPACE(sizeof(int))];

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsgbuf;
    msg.msg_controllen = sizeof(cmsgbuf);
    msg.msg_flags = MSG_WAITALL;

    if (recvmsg(gate, &msg, 0) < 0)
    {
        VPERROR("recvmsg() failed\n");
        exit(-1);
    }

    struct cmsghdr *cmsg = reinterpret_cast<cmsghdr*>(cmsgbuf);

    return *((int*)CMSG_DATA(cmsg));
}

int main()
{
    const char* path = "/tmp/local.sock";

    int gate = listen_gate(path);

    VPRINTF("gate %d\n", gate);

    pid_t msg;
    int fd = recvfd(gate, &msg, sizeof(msg));

    VPRINTF("file descriptor %d from pid %d\n", fd, msg);

    const size_t bufsize = 1024;
    ssize_t len;
    char buf[bufsize];

    while ((len = read(0, buf, sizeof(buf))) > 0)
    {
        const char* p = buf;
        const char* const endp = p + len;

        do {
            const ssize_t nbytes = write(fd, p, endp - p);

            if (nbytes < 0)
            {
                if (errno == EINTR || errno == EAGAIN)
                {
                    continue;
                }
                else
                {
                    goto end;
                }
            }

            p += nbytes;
        } while (p < endp);
    }

end:

    VPERROR("end\n");
    unlink(path);

    return 0;
}
