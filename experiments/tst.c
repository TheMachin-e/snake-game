#include<stdio.h>
int main(){
int i,j;
printf("\033[2J");
for(i=1;i<=40;i++)
for(j=1;j<=80;j+=2)
if(i==1 && j==1||i==0&&j==79||i==40&&j==79||i==40&&j==79)
printf("\033[%d;%dH+",i,j);
else if(i==1&&j<=80||i==40&&j<=80){
printf("\033[%d;%dH-",i,j);
printf("-");
}
else if(j<=40&&j==1||i<=40&&j==79)
printf("\033[%d;%dH|",i,j);
return 0;
}
