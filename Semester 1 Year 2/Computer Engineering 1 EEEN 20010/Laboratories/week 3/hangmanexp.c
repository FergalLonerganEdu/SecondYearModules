#include<stdio.h>
#include<string.h>
main(){
	char word[30],getit[30],*found;
	int score,loop,len,num,lup,om,luptimes,times,fitimes,ondtimes,antimes;
	char gname[70];
	char h;

	luptimes=0;
	fitimes=0;
	ondtimes=0;

	printf("\n ENTER THE WORD: ");
	gets(word);
	score=0;
	num=0;
	om=0;
	len=strlen(word);

	printf("\n ENTER YOUR NAME: ");
	scanf("%s",gname);
	printf("\n LET's START: ");
	h='_';
	printf("\n LOOK HERE: ");
	for(loop=0;loop!=len;loop++)
	{
		printf("\n %c ",h);
	}

	for(loop=0;loop!=len;loop++)
	{
		om=1;
		num=num+1;
		fitimes=0;
		ondtimes=0;
		printf("\n\n %d LETTER: ",num);
		scanf("%s",&getit[num]);
		found=strchr(word,getit[num]);
		if(found)        {
			for(luptimes=0,antimes=1; luptimes!=len; luptimes++,antimes++)
			{
				if(getit[num]==word[luptimes])
				{
					fitimes++;
				}
				else
				{
					fitimes=fitimes;
				}
			}
			for(luptimes=num;luptimes!=0;luptimes--)
			{
				if(getit[num]==getit[luptimes])
				{
					ondtimes++;
				}
				else
				{
					ondtimes=ondtimes;
				}
			}
			if(fitimes>=ondtimes)
			{
				score=score+1;
				printf("\n OK\n");
				for(lup=0;getit[num]!=word[lup];lup++)
				{
					om++;
				}
				printf("\n POSITION: %d\n",om);
				printf("\n ===================\n");
			}
			else
			{
				score=score;
				printf("\n NOT AGAIN!\n");
			}
		}
		else
		{
			score=score;
			printf("\n NO\n");
		}
	}
	printf("\n THE WORD IS '%s'\n",word);
	printf("\n YOUR SCORE IS %d OUT OF %d",score,len);

	return 0;
}


