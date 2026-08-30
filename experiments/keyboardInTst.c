#include<stdio.h>
#include<unistd.h>
#include<termios.h>

int main(){
struct termios old, new;
char c;
tcgetattr(STDIN_FILENO, &old);
new=old;
new.c_lflag &= ~(ICANON | ECHO);
tcsetattr(STDIN_FILENO, TCSANOW, &new);
read(STDIN_FILENO, &c,1);
tcsetattr(STDIN_FILENO, TCSANOW, &old);
printf("\nyou pressed: %c\n",c);
return 0;
}
