#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>

int main(void)
{
    struct termios old, new;
    char ch;

    tcgetattr(STDIN_FILENO, &old);

    new = old;
    new.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &new);

    while (1)
    {
        fd_set set;
        struct timeval timeout;

        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        if (select(STDIN_FILENO + 1,
                   &set,
                   NULL,
                   NULL,
                   &timeout) > 0)
        {
            read(STDIN_FILENO, &ch, 1);

            if (ch == 'q')
                break;

            printf("You pressed: %c\n", ch);
        }

        usleep(10000);
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old);

    return 0;
}
