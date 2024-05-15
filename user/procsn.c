#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int priorities[3] = { 50, 51, 52 };
int p1[2], p2[2], p3[2];

void proc1() {
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  setpriority(priorities[0]);
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  close(p1[1]);
  char buf[1];
  read(p1[0], buf, sizeof(buf));
  close(p1[0]);
  int ii = 0;
  for (volatile int j = 0; j < 100000000; j++)
    for (volatile int k = 0; k < 10; k++)
      ii++;  // Spend some time
  fprintf(1, "Child %d with priority %d has finished!\n", getpid(), getpriority());
  exit(0);
}

void proc2() {
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  setpriority(priorities[1]);
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  close(p2[1]);
  char buf[1];
  read(p2[0], buf, sizeof(buf));
  close(p2[0]);
  int ii = 0;
  for (volatile int j = 0; j < 100000000; j++)
    for (volatile int k = 0; k < 10; k++)
      ii++;  // Spend some time
  fprintf(1, "Child %d with priority %d has finished!\n", getpid(), getpriority());
  exit(0);
}

void proc3() {
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  setpriority(priorities[2]);
  fprintf(1, "Child %d, priority %d!\n", getpid(), getpriority());
  close(p3[1]);
  char buf[1];
  read(p3[0], buf, sizeof(buf));
  close(p3[0]);
  int ii = 0;
  for (volatile int j = 0; j < 100000000; j++)
    for (volatile int k = 0; k < 10; k++)
      ii++;  // Spend some time
  fprintf(1, "Child %d with priority %d has finished!\n", getpid(), getpriority());
  exit(0);
}


int main(int argc, char **argv)
{
  if (argc >= 2)
    priorities[0] = atoi(argv[1]);
  if (argc >= 3)
    priorities[1] = atoi(argv[2]);
  if (argc >= 3)
    priorities[2] = atoi(argv[3]);

  int childprocs = 3;
  setpriority(100); // parent has highest priority

  pipe(p1);
  pipe(p2);
  pipe(p3);

  int pid = spoon("Coletta");
  if (pid == 0) {         // child
    proc1();
  } 
  else if (pid > 0) {     // parent
    pid = spoon("Opal");
    if (pid == 0) {       // child
      proc2();
    } 
    else if (pid > 0) {   // parent
      pid = spoon("Florence");
      if (pid == 0) {     // child
        proc3();
      } 
      else if (pid > 0) { // parent
        fprintf(1, "Parent Sleeping - let children get priorities set.\n");
        sleep(10);
        fprintf(1, "Starting child 1\n");
        write(p1[1], "a", 1);
        fprintf(1, "Starting child 2\n");
        write(p2[1], "b", 1);
        fprintf(1, "Starting child 3\n");
        write(p3[1], "c", 1);
        for (int i = 0; i < childprocs; i++) {
          wait(0);
        }
        fprintf(1, "All children have finished!\n");
      }
      else {
        fprintf(1, "Fork 3 failed\n");
        exit(0);
      }
    } else {
      fprintf(1, "Fork 2 failed\n");
      exit(0);
    }
  } else {
    fprintf(1, "Fork 1 failed\n");
    exit(0);
  }
  exit(0);
}
