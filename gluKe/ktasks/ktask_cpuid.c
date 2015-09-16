#include <glinc.h>

#include <cpuid.h>

void ktentry_cpuid()
{
	int term;
	
	if((term=open("terminal"))<0)
	{
		kwr("'open' failed");
		task_kill_me();
	}

	cpuid_print(term);
	
	task_kill_me();
}

