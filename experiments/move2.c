#include<stdio.h>
#include<unistd.h>
void arrrev(int *a,int len){
	int i,j,t;
	for(i=0,j=len-1;i<j;i++,j--){
		t=a[i];
		a[i]=a[j];
		a[j]=t;
	}
}
int main(){
	int i,j,len=5,f=0,x,y,running=1;
	//char blocks[5]={'<','>','-',' '};
	int body[5] = {11, 9, 7, 5, 3};
	printf("\033[2J");
	printf("\033[?25l");
	for(i=1;i<=40;i++)
		for(j=1;j<=79;j+=2)
			if(i==1||j==1||i==40||j==79)
				printf("\033[%d;%dH--",i,j);
	fflush(stdout);
	x=2,y=11;
	while(running){
		for(i=0;i<len;i++)
			if(!i)
				printf("\033[%d;%dH<>",x,body[i]);
			else
				printf("\033[%d;%dH--",x,body[i]);
		if (!f && body[4] > 3 )
			printf("\033[%d;%dH  ", x,body[4]-2);
		else if(f && body[4] < 77)
			printf("\033[%d;%dH  ", x,body[4]+2);
		fflush(stdout);
		usleep(160000);
		if(body[0]==3){
			arrrev(body,len);
			f=0;
		}
		if(body[0]==77){
			arrrev(body,len);
			//for(i=0;i<len;i++)
			//printf("\033[%d;%dH%d",x+i,90+i,body[i]);
			f=1;
		}
		if (f)
			for(i=0;i<len;i++)
				body[i]-=2;
		else
			for(i=0;i<len;i++)
				body[i]+=2;
	}
	return 0;
}
