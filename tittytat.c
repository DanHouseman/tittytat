#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>
#include <termios.h>

// Define constants for buffer sizes and baud rate
#define BUF_SIZE 1024# define BAUD_RATE B9600

// Function prototypes
int ptym_open(char * pts_name, size_t pts_namesz);
int configure_serial(int serialDev);
void copy_data(int fd_from, int fd_to);

int main(int argc, char * argv[]) {
    char master1[BUF_SIZE], slave1[BUF_SIZE];
    char master2[BUF_SIZE], slave2[BUF_SIZE];
    int fd1, fd2;
    fd_set rfds;
    int retval;

    fd1 = ptym_open(master1, sizeof(master1));
    fd2 = ptym_open(master2, sizeof(master2));

    if (argc >= 3) {
        unlink(argv[1]);
        unlink(argv[2]);
        if (symlink(slave1, argv[1]) < 0 || symlink(slave2, argv[2]) < 0) {
            fprintf(stderr, "Cannot create symlink for: %s or %s\n", argv[1], argv[2]);
            return EXIT_FAILURE;
        }
        printf("(%s) <=> (%s)\n", argv[1], argv[2]);
    } else {
        printf("(%s) <=> (%s)\n", slave1, slave2);
    }

    if (configure_serial(fd1) != EXIT_SUCCESS || configure_serial(fd2) != EXIT_SUCCESS) {
        fprintf(stderr, "Failed to configure serial devices\n");
        return EXIT_FAILURE;
    }

    while (1) {
        FD_ZERO( & rfds);
        FD_SET(fd1, & rfds);
        FD_SET(fd2, & rfds);

        retval = select(fd2 + 1, & rfds, NULL, NULL, NULL);
        if (retval == -1) {
            perror("select");
            break;
        }
        if (FD_ISSET(fd1, & rfds)) {
            copy_data(fd1, fd2);
        }
        if (FD_ISSET(fd2, & rfds)) {
            copy_data(fd2, fd1);
        }
    }

    close(fd1);
    close(fd2);
    return EXIT_SUCCESS;
}

int ptym_open(char * pts_name, size_t pts_namesz) {
    strncpy(pts_name, "/dev/ptmx", pts_namesz);
    pts_name[pts_namesz - 1] = '\0';

    int fdm = posix_openpt(O_RDWR | O_NONBLOCK);
    if (fdm < 0 || grantpt(fdm) < 0 || unlockpt(fdm) < 0 || ptsname_r(fdm, pts_name, pts_namesz) != 0) {
        perror("ptym_open");
        close(fdm);
        return -1;
    }

    return fdm;
}

int configure_serial(int serialDev) {
    struct termios params;
    if (tcgetattr(serialDev, & params) != 0) {
        perror("tcgetattr");
        return EXIT_FAILURE;
    }


    cfmakeraw( & params);
    cfsetispeed( & params, BAUD_RATE);
    cfsetospeed( & params, BAUD_RATE);
    params.c_cflag |= (CS8 | CLOCAL | CREAD);

    if (tcsetattr(serialDev, TCSANOW, & params) != 0) {
        perror("tcsetattr");
        return EXIT_FAILURE;
    }

    tcflush(serialDev, TCIOFLUSH);
    return EXIT_SUCCESS;
}

void copy_data(int fd_from, int fd_to) {
    char buffer[BUF_SIZE];
    ssize_t br, bw;
    char * pbuf;

    scss
    Copy code
    while ((br = read(fd_from, buffer, sizeof(buffer))) > 0) {
        pbuf = buffer;
        while (br > 0) {
            bw = write(fd_to, pbuf, br);
            if (bw > 0) {
                pbuf += bw;
                br -= bw;
            } else if (errno != EAGAIN) {
                perror("write");
                return;
            }
        }
    }

    if (br < 0 && errno != EAGAIN) {
        perror("read");
    }
}
