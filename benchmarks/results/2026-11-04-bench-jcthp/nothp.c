// Runs a program with transparent huge pages disabled for it: PR_SET_THP_DISABLE survives execve.
#include <stdio.h>
#include <sys/prctl.h>
#include <unistd.h>

int main(int argc, char** argv)
{
  if (argc < 2) { fprintf(stderr, "usage: nothp program [args...]\n"); return 2; }
  if (prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0) != 0 || prctl(PR_GET_THP_DISABLE, 0, 0, 0, 0) != 1) { perror("nothp: prctl"); return 2; }
  execvp(argv[1], argv + 1);
  perror("nothp: execvp");
  return 127;
}
