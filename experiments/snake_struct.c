#include<stdio.h>
#include<unistd.h>
#define right 0
#define left 1
#define up 2
#define down 3
struct Segment{
	int row;
	int col;
};

void r_shift(struct Segment *a,int len){
	for(int i=len-1;i>0;i--)
		a[i]=a[i-1];
}

int main(){
	int i,j,len=5,direction=right,clrow=100,clcol=100,running=1;
	struct Segment snake[5] = {
		{2, 11},
		{2, 9},
		{2, 7},
		{2, 5},
		{2, 3}
	};
	printf("\033[2J");
	printf("\033[?25l");
	
	for(i=1;i<=40;i++)
		for(j=1;j<=79;j+=2)
			if(i==1||j==1||i==40||j==79)
				printf("\033[%d;%dH--",i,j);

	fflush(stdout);
	while(running){
		printf("\033[%d;%dH♦",snake[0].row,snake[0].col);
		for(i=1;i<len;i++)
				//if(direction == right || direction == left)
				//printf("\033[%d;%dH◇",bodyx[i],bodyy[i]);
				//else
				printf("\033[%d;%dH♢",snake[i].row,snake[i].col);
		printf("\033[%d;%dH  ", clrow,clcol);
		fflush(stdout);
		usleep(160000);
		if(snake[0].col==3 && snake[0].row==2){
			direction  = right;
		}
		else if(snake[0].col==77 && snake[0].row==2){
			direction = down;
		}
		else if(snake[0].col==77 && snake[0].row==39){
			direction = left;
		}
		else if(snake[0].col==3 && snake[0].row==39)
			direction = up;
			
		clrow=snake[len-1].row;
		clcol=snake[len-1].col;
		r_shift(snake,len);
		if (direction == right)
			snake[0].col += 2;
		else if(direction == left)
			snake[0].col -= 2;
		else if(direction  == up)
			snake[0].row--;
		else if(direction == down)
			snake[0].row++;
	}
	return 0;
}
