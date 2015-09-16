#include <glinc.h>

unsigned int timer_cur_frequency=TIMER_FREQUENCY;
unsigned int timer_cur_mksec_per_tick=1000000/TIMER_FREQUENCY;

struct task *last_planned_task;

void plan(int iret)
{
	struct task* t;

	//if called from interrupt - interrupts are already disabled (i hope :)
	//if called from some task - disable interrupts,
	//but do not increase block counter
	//interrupts will be enabled in task_switch function
	if(iret) one_noinc();

	t=first_task;
	do{
		t=ll_next_loop(&tasks,t);

		if(t->sleep_mksec!=0)
		{
			if(t->sleep_mksec <= timer_cur_mksec_per_tick) t->sleep_mksec=0;				
			else t->sleep_mksec-=timer_cur_mksec_per_tick;
		}
	}while(t!=first_task);


	if((t=last_planned_task)==0) t=first_task;

	do{
		t=ll_next_loop(&tasks,t);

		if(t->plan && t->sleep_mksec==0) break;
	}while(t!=last_planned_task);

	//no task to plan, so plan idle task
	if(!t->plan) t=first_task;

	last_planned_task=t;

	if(iret) task_switch(t->id);
	else
	{
		if(task_set_previous(t)) die("'task_set_previous' failed");
	}
}

void plan_timer_handler()
{
	plan(0);
}

int plan_init()
{
	if(int_init_timer_task(plan_timer_handler)==-1) return -1;

	last_planned_task=first_task;

	return 0;
}
