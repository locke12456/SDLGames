#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int Rand[10]={0};
int tempRand[10]={0};
int a[10]={0};
int arraw=0,new_rand=rand()%10,next=0,amount=0;
void RandInit()
{
	srand(rand()%0xffff);
	Rand[0]=rand()%10;
	for(int i=1;i<10;i++)
	{
		Rand[i]=(Rand[i-1]+3)%10;
		tempRand[i]=Rand[i];
	}
}
void TenRand()
{
	amount++;
	if(Rand[(++arraw)%10]==new_rand)
	{
		a[next]=amount;
		amount=0;
		if(next>9){
			RandInit();
			next=0;
		}
		Rand[0]=tempRand[(++next)%10];
		for(int i=1;i<10;i++)
		{
			Rand[i]=(Rand[i-1]+3)%10;
			//tempRand[i]=Rand[i];
		}
		srand(rand()%0xffff);
		new_rand=rand()%10;
	}
}