#include <glinc.h>

#include <life.h>

int callback_life(char *arena,int size_w,int size_h,void *user_data)
{
	char pixel[4],key;
	int x,y,k,term;
	
	term=*((int*)user_data);

	if(read(term,&key,1)) return 1;

	pixel[2]=0xdb;

	for(y=0,k=0;y<25;y++)
	{
		pixel[1]=y;
		for(x=0;x<80;x++)
		{
			pixel[0]=x;
			if(arena[k++]) pixel[3]=0x0e;
			else pixel[3]=0x02;
			write(term,pixel,4);		
		}
	}
	
	ioctl(term,TERM_IOCTL_SHOW,0);
	
	return 0;
}

int start_life(int term,int sample_number)
{
	struct life_object *sample;
	
	switch(sample_number){
	case 0:sample=LIFE_SAMPLE_R_PENTOMINO;break;
	case 1:sample=LIFE_SAMPLE_GLIDERS;break;
	case 2:sample=LIFE_SAMPLE_OSCILLATORS;break;
	case 3:sample=LIFE_SAMPLE_QUEEN_BEE;break;
	default:return -1;
	}

	ioctl(term,TERM_IOCTL_MODE_PICTURE,0);

	life(80,25,sample,callback_life,&term);
		
	ioctl(term,TERM_IOCTL_MODE_TERMINAL,0);
	
	return 0;
}

void ktentry_life()
{
	int term,sample_number,len;
	char cmd[20];	
	
	if((term=open("terminal"))<0)
	{
		kwr("'open' failed\n");
		task_kill_me();
	}
	
	while(1)
	{
		write_string(term,"program shows conway's life algorithm\n\
enter numbers 0-3 to view samples, 'exit' or 'e' to exit\n");
		
		if(read_line(term,">",cmd,sizeof(cmd))<0) kwr("'read_line' failed\n");
		else
		{
			if((len=kstrlen(cmd))!=0) cmd[len-1]=0;

			if(kstrcmp(cmd,"exit")==0 || kstrcmp(cmd,"e")==0) task_kill_me();

			if(stoi(cmd,&sample_number) || sample_number<0 || sample_number>3)
			{
				write_string(term,"unknown command\n\n");
				continue;
			}
					
			if(start_life(term,sample_number))
			{
				write_string(term,"error occured\n\n");
			}
		}
	}
}


