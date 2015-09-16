#ifndef PLAN_H_INCLUDED
#define PLAN_H_INCLUDED

extern struct task *last_planned_task;

int plan_init();
void plan(int iret);

#endif

