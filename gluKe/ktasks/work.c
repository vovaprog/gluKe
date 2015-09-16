#include <glinc.h>

#include <mtok.h>

#include <cpuid.h>

struct ktask_info{
	char name[MAX_NAME_LENGTH];
	char *descr;
	task_entry tentry;
};

//--------------------------------------------------------------------
//add task entries to this array
//task name, then short description, or 0, then task entry

struct ktask_info ktask_infos[]=
{
	{"counter",KTASK_COUNTER_DESCR,counter},
	{"reverse",KTASK_REVERSE_DESCR,reverse},
	{"cpuid",KTASK_CPUID_DESCR,ktentry_cpuid},
	{"life",KTASK_LIFE_DESCR,ktentry_life},
	{"",0,0}
};

//--------------------------------------------------------------------

#define HELP "\
work task commands:\n\
help          - show this message.\n\
tnames        - list names of tasks you may start using start command.\n\
tdescr arg    - print short description of task with name arg.\n\
start name    - start task called name. You may write 's' instead of 'start'.\n\
tasks         - print running tasks.\n\
kill arg      - kill task with id=arg. You may write 'k' instead of 'kill'.\n\
handles       - print opened handles.\n\
memory        - print allocated and free memory regions.\n\
\n\
use CTRL+F1..Fx to switch terminals.\n"

static int start_ktask(char *name)
{
	int i;

	for(i=0;ktask_infos[i].tentry!=0;i++)
		if(kstrcmp(ktask_infos[i].name,name)==0)
			return task_newk(ktask_infos[i].tentry,1);

	return -1;
}

static void print_ktask_names()
{
	int i;

	for(i=0;ktask_infos[i].tentry!=0;i++)
	{
		kwr(ktask_infos[i].name);
		kwr("\n");
	}
}

static int kill_task(char *sid)
{
	int task_id;

	if(sid==0) return -1;

	if(stoi(sid,&task_id)) return -1;

	if(task_id!=task_cur_id()) return task_kill(task_id);
	else return -1;
}

static int task_info(char *name)
{
	int i;

	for(i=0;ktask_infos[i].tentry;i++)
		if(kstrcmp(ktask_infos[i].name,name)==0)
		{
			if(ktask_infos[i].descr) kwr(ktask_infos[i].descr);
			else kwr("no info\n");
			return 0;
		}
	return -1;
}

void work()
{
	char buf[200];
	char words[3][MAX_NAME_LENGTH];
	int cur_pos,word_size;

	word_size=MAX_NAME_LENGTH;

	kwr("\n\
work task started.\n\
enter 'help' if you need help.\n");

    all();

loop:

	if(read_line(kfile,"go>",buf,sizeof(buf))<0) kwr("'read_line' failed\n");
	else
	{
		cur_pos=0;
		if(mtok(buf,&cur_pos,words[0],&word_size,0," \r\n")!=0)
		{
			words[0][0]='\0';word_size=MAX_NAME_LENGTH;
		}

		if(mtok(buf,&cur_pos,words[1],&word_size,0," \r\n")!=0)
		{
			words[1][0]='\0';word_size=MAX_NAME_LENGTH;
		}

		if(mtok(buf,&cur_pos,words[2],&word_size,0," \r\n")!=0)
		{
			words[2][0]='\0';word_size=MAX_NAME_LENGTH;
		}

		if(kstrcmp(words[0],"start")==0 || kstrcmp(words[0],"s")==0)
		{
			if(start_ktask(words[1])<0) kwr("can't start task\n");
		}
		else if(kstrcmp(words[0],"tnames")==0)
		{
			print_ktask_names();
		}
		else if(kstrcmp(words[0],"tasks")==0)
		{
			if(task_info_print_tasks()) kwr("can't print task info\n");
		}
		else if(kstrcmp(words[0],"kill")==0 || kstrcmp(words[0],"k")==0)
		{
			if(kill_task(words[1])) kwr("can't kill task\n");
		}
		else if(kstrcmp(words[0],"tdescr")==0)
		{
			if(task_info(words[1])) kwr("can't print task info\n");
		}
		else if(kstrcmp(words[0],"help")==0)
		{
			kwr(HELP);
		}
		else if(kstrcmp(words[0],"handles")==0)
		{
			if(handle_print_handles()) kwr("can't print handle info\n");
		}
		else if(kstrcmp(words[0],"memory")==0)
		{
			rgn_print_info();
		}
		else kwr("unknown command\n");
	}

goto loop;
}

