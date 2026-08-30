#include<stdio.h>
#include<unistd.h>
#define right 0
#define left 1
#define up 2
#define down 3
void arrrev(int *a,int len){
	int i,j,t;
	for(i=0,j=len-1;i<j;i++,j--){
		t=a[i];
		a[i]=a[j];
		a[j]=t;
	}
}
void r_shift(int *a,int len){
	for(int i=len-1;i>0;i--)
		a[i]=a[i-1];
}

int main(){
	int i,j,len=5,direction=right,clx=0,cly=0,running=1;
	//char blocks[5]={'<','>','-',' '};
	int bodyy[5] = {11, 9, 7, 5, 3},bodyx[5]={2, 2, 2, 2, 2};
	printf("\033[2J");
	printf("\033[?25l");
	
	for(i=1;i<=40;i++)
		for(j=1;j<=79;j+=2)
			if(i==1||j==1||i==40||j==79)
				printf("\033[%d;%dH--",i,j);

	fflush(stdout);
	while(running){
		printf("\033[%d;%dH♦",bodyx[0],bodyy[0]);
		for(i=1;i<len;i++)
				//if(direction == right || direction == left)
				//printf("\033[%d;%dH◇",bodyx[i],bodyy[i]);
				//else
				printf("\033[%d;%dH♢",bodyx[i],bodyy[i]);
		printf("\033[%d;%dH  ", clx,cly);
		fflush(stdout);
		usleep(160000);
		if(bodyy[0]==3 && bodyx[0]==2){
			direction  = right;
		}
		else if(bodyy[0]==77 && bodyx[0]==2){
			direction = down;
		}
		else if(bodyy[0]==77 && bodyx[0]==39){
			direction = left;
		}
		else if(bodyy[0]==3 && bodyx[0]==39)
			direction = up;
			
		clx=bodyx[len-1];
		cly=bodyy[len-1];
		r_shift(bodyy,len);
		r_shift(bodyx,len);
		if (direction == right)
			bodyy[0] += 2;
		else if(direction == left)
			bodyy[0] -= 2;
		else if(direction  == up)
			bodyx[0]--;
		else if(direction == down)
			bodyx[0]++;
	}
	return 0;
}
