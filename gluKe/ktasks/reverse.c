#include <glinc.h>

void reverse()
{
	char buf[200],temp;
	int i,l,term;
	
	if((term=open("terminal"))<0)
	{
		kwr("'open' failed");
		task_kill_me();
	}

loop:
	write_string(term,"\
enter string and program will print it from tail to head.\n\
enter 'exit' or 'e' to stop program.\n");
	
	if(read_line(term,"enter word>",buf,sizeof(buf))<0) write_string(term,"'read_line' failed\n");
	else
	{
		if((l=kstrlen(buf))!=0) 
		{
			buf[l-1]=0;
			l--;			
		}

		if(kstrcmp(buf,"e")==0 || kstrcmp(buf,"exit")==0) task_kill_me();

		for(i=0;i<l/2;i++)
		{
			temp=buf[i];
			buf[i]=buf[l-1-i];
			buf[l-1-i]=temp;
		}

		write_string(term,buf);write_string(term,"\n\n");
	}
	goto loop;
}
