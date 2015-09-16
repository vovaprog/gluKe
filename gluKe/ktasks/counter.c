#include <glinc.h>

void counter()
{
	char buf[ITOS_BSIZE];
	int counter=0;
	int term;

	if((term=open("terminal"))==-1)
	{
		kwr("'open' failed");
		task_kill_me();
	}

	while(1)
	{
		int i=0;
		for(i=0;i<5;i++)
		{
			write_string(term,itos_ud(counter++,buf));
			write_string(term," ");
		}
		sleep_mksec(5000);
	}
}
