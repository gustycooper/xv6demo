#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void)
{
  int n;
  argint(0, &n);
  myproc()->mask = n;
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo info;
  info.freemem = get_freemem();
  info.nproc = get_nproc();

  uint64 addr;
  argaddr(0, &addr);
  // kernel cannot directly access user space. use copyout()
  return copyout(myproc()->pagetable, addr, (char*)&info, sizeof(info));
}

int rseedv = 0;

uint64
sys_rseed(void)
{
  int n;
  argint(0, &n);
  rseedv = n;
  return 0;
}

#define MY_RAND_MAX ((1U << 31) - 1)

uint64
sys_rinter(void)
{
  int max;
  argint(0, &max);
  rseedv = ((rseedv * 1103515245 + 12345) & MY_RAND_MAX);
  return rseedv % max;
}

uint64
sys_ps(void)
{
  procdump();
  return 0;
}

uint64
sys_prochist(void)
{
  prochistory();
  return 0;
}

uint64
sys_sigalarm(void)
{
  struct proc* p = myproc();
  argint(0, &p->interval);
  argaddr(1, &p->handler);
  p->regs = p->trapframe + sizeof(struct trapframe);
  return 0;
}

uint64
sys_sigreturn(void)
{
  struct proc* p = myproc();
  memmove(p->trapframe, p->regs, sizeof(struct trapframe));
  p->ticks = 0;
  return p->trapframe->a0;
}

uint64
sys_getpriority(void)
{
  return myproc()->priority;
}

uint64
sys_setpriority(void) {
  int new_priority;
  argint(0, &new_priority);
  struct proc* p = myproc();
  p->priority = new_priority;
  return 0;
}

uint64
sys_spoon(void) {
  char procname[16];

  if(argstr(0, procname, 16) < 0)
    return -1;

  return spoon(procname);
}
