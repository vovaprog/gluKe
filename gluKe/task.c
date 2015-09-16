#include <glinc.h>

extern void *GDT;

struct ll_info tasks;

struct task *first_task=0;
int first_task_id=-1;

static int killer_task_id_to_kill;
static struct task *killer_task;
static int killer_task_id;

static void tentry_killer()
{
	one();
	
	while(1)
	{
		task_kill(killer_task_id_to_kill);

/*	plan should be really here, but it does not work. Why? */		
/*		plan(1); */

		task_switch(first_task_id);
	}
}

static int new_task_id_fun(void *p_item,void *arg)
{
	struct task *t=(struct task*)p_item;

	if(t->id>*(int*)arg) *(int*)arg=t->id;

	return 0;
}

//lock all before calling this
static int new_task_id()
{
	int max_task_id=0;

	ll_map_1arg(&tasks,new_task_id_fun,&max_task_id);

	return max_task_id+1;
}

static struct task* init_task_struct()
{
	struct task *new_task;
	int gdt_index;

	if((new_task=ll_push_back(&tasks))==0) return 0;

	kmemset(new_task,0,sizeof(struct task));

	for(gdt_index=1;gdt_index<GDT_MAX_DESCRS &&
		gdt_seg_limit(((char*)GDT)+gdt_index*GDT_DESCR_SIZE)!=0;
		gdt_index++);

	if(gdt_index==GDT_MAX_DESCRS)
	{
		ll_delete(&tasks,new_task);
		return 0;
	}

	new_task->gdt_tss_descr=(char*)GDT+GDT_DESCR_SIZE*gdt_index;

	gdt_fill_tss_descr(new_task->gdt_tss_descr,
                      &(new_task->tss),sizeof(struct TSS));

	new_task->selector=gdt_selector(gdt_index);

	//first set new task id to 0
	//because new_task_id will use ll_map
	//new struct is already added to list, but its id
	//is not inited
	new_task->id=0;

	new_task->id=new_task_id();

	return new_task;
}

int nof_used_descrs()
{
	int gdt_index;
	int count=0;

	for(gdt_index=1;gdt_index<GDT_MAX_DESCRS;gdt_index++)
		if(gdt_seg_limit(((char*)GDT)+gdt_index*GDT_DESCR_SIZE)!=0) count++;
	return count;
}

static void destroy_task_struct(struct task *t)
{
	kmemset(t->gdt_tss_descr,0,GDT_DESCR_SIZE);

	ll_delete(&tasks,t);
}

static int task_register_task0()
{
	struct task *new_task;

	if((new_task=init_task_struct())==0) return -1;

	new_task->tss.CR3=(unsigned int)k_page_cat;

	new_task->plan=1;

	asm("mov %0,%%ax   \n\
	     ltr %%ax        "::"m" (new_task->selector):"eax");

	return new_task->id;
}

int task_init()
{
	if(ll_init(&tasks,sizeof(struct task))==0) return -1;

	if((first_task_id=task_register_task0())<0) return -1;

	first_task=task_get_by_id(first_task_id);

	if((killer_task_id=task_newk(tentry_killer,0))<0) return -1;

	killer_task=task_get_by_id(killer_task_id);

	return first_task_id;
}

int task_newk(task_entry t_entry,int plan)
{
	struct task *new_task;
	char *stack;

	one();

	if((new_task=init_task_struct())==0)
	{
		all();
		return -1;
	}

	if((stack=rgn_alloc_t(KTASK_DEFAULT_STACK_SIZE,new_task->id))==0)
	{
		destroy_task_struct(new_task);
		all();
		return -1;
	}

	new_task->tss.CS=K_CS_SEL;
	new_task->tss.DS=K_DS_SEL;
	new_task->tss.SS=K_SS_SEL;
	new_task->tss.ES=K_DS_SEL;
	new_task->tss.FS=K_DS_SEL;
	new_task->tss.GS=K_DS_SEL;

	new_task->tss.ESP=(unsigned int)(stack+KTASK_DEFAULT_STACK_SIZE);

	new_task->tss.EIP=(unsigned int)t_entry;

	new_task->tss.CR3=(unsigned int)k_page_cat;

	new_task->tss.EFLAGS=0x00000200; /* set IF flag (permit interrupts) */

	new_task->plan=plan;

	all();
	return new_task->id;
}

static int task_get_by_id_predicate(void *p_item,void *arg)
{
	struct task *t=(struct task*)p_item;
	int task_id=((int*)arg)[0];

	if(t->id==task_id) return 1;
	return 0;
}
struct task* task_get_by_id(int task_id)
{
	return ll_map_1arg(&tasks,task_get_by_id_predicate,&task_id);
}


int task_get_by_sel_predicate(void *p_item,void *arg)
{
	struct task *t=(struct task*)p_item;
	unsigned short int sel=*(int*)arg;

	if(t->selector==sel) return 1;
	return 0;
}
struct task* task_get_by_sel(unsigned short int sel)
{
	return ll_map_1arg(&tasks,task_get_by_sel_predicate,&sel);
}

int task_switch(int task_id)
{
	struct task* t;

	int_off();

	if((t=task_get_by_id(task_id))==0) return -1;

	task_set_previous(t);

	/* set NT flag */
	asm("pushf\n\
	     orl $(0x00004000),(%esp) \n\
	     popf");

	int_on_iret();

	return 0;
}

static struct task* task_cur()
{
	unsigned short int task_sel;
	struct task *t;

	asm("str %0"::"m" (task_sel));

	if((t=task_get_by_sel(task_sel))) return t;

	return 0;
}

int task_cur_id()
{
	struct task *t;

	if((t=task_cur())==0) return -1;

	return t->id;
}


#define SET_TASK_BUSY_FLAG(p_task) (((char*)(p_task)->gdt_tss_descr)[5]|=0x02)
#define CLEAR_TASK_BUSY_FLAG(p_task) (((char*)(p_task)->gdt_tss_descr)[5]&=~0x02)

int task_set_previous(struct task *target_task)
{
	struct task *cur_task,*prev_task;

	cur_task=task_cur();

	if((prev_task=task_get_by_sel(cur_task->tss.prev_task))!=0)
		CLEAR_TASK_BUSY_FLAG(prev_task);

	SET_TASK_BUSY_FLAG(target_task);

	cur_task->tss.prev_task=target_task->selector;

	return 0;
}

int task_fill_idt_task_gate(int task_id,void *idt_descr)
{
	struct task *task;

	if((task=task_get_by_id(task_id))==0) return -1;

	((int*)idt_descr)[0]=0x0;
	((int*)idt_descr)[1]=0x0;

	((unsigned char*)idt_descr)[5]=0x85;

	((unsigned short int*)idt_descr)[1]=task->selector;

	return 0;
}

int sleep_mksec(unsigned int mksec)
{
	struct task *t;

	one();
	
	if((t=task_get_by_id(task_cur_id()))==0) return -1;
	t->sleep_mksec=mksec;

	all();

	task_switch(first_task_id);

	return 0;
}

int task_set_planned(int task_id,int plan)
{
	struct task *t=task_get_by_id(task_id);

	t->plan=plan;

	return 0;
}

int task_kill(int task_id)
{
	struct task *t;

	//don't let to kill first task
	if(task_id==first_task_id) return -1;

	one();

	if((t=task_get_by_id(task_id))==0)
	{
		all();
		return -1;
	}

	if(handle_close_task_handles(task_id))
		die("'close_task_files' failed");

	destroy_task_struct(t);

	if(rgn_free_task_memory(task_id))
		die("'rgn_free_task_memory' failed");

	//plan will start from first_task
	last_planned_task=first_task;

	all();

	return 0;
}

void task_kill_me()
{
	int cur_id;
	struct task* cur_task;

	one();

	cur_id=task_cur_id();
	cur_task=task_get_by_id(cur_id);	
	cur_task->plan=0;

	killer_task_id_to_kill=cur_id;
	
	task_switch(killer_task_id);
}

//------------------------------------------------------------------------
//task info utils
//may be this should be put to separate file

static int task_info_print_tasks_callback(void *p_item)
{
	struct task *t=(struct task*)p_item;
	char itos_buf[ITOS_BSIZE];

	kwr("id:");
	kwr(itos_ud(t->id,itos_buf));
	kwr(" planned:");
	kwr(t->plan?"yes":"no");
	kwr("\n");

	return 0;
}

int task_info_print_tasks()
{
	ll_map(&tasks,task_info_print_tasks_callback);
	return 0;
}
