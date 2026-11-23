// LD_PRELOAD shim: logs every malloc of at least 512 KiB, and its free, to stderr as "+ size address" / "- 0 address".
// Single-threaded use only (jc_qjmc). Allocations made while dlsym resolves the real functions come from a static buffer.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

enum { BIG = 1 << 19, MAXLIVE = 1024 };

static void* (*realMalloc)(size_t);
static void (*realFree)(void*);
static int resolving;
static _Alignas(16) char boot[8192];
static size_t bootUsed;
static void* live[MAXLIVE];
static int nLive;

static void resolve(void)
{
  resolving = 1;
  realMalloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
  realFree = (void (*)(void*))dlsym(RTLD_NEXT, "free");
  resolving = 0;
}

static void say(char sign, size_t n, void* p)
{
  char b[64];
  int k = snprintf(b, sizeof b, "%c %zu %p\n", sign, n, p);
  if (k > 0) (void)!write(2, b, (size_t)k);
}

void* malloc(size_t n)
{
  if (!realMalloc) {
    if (resolving) { size_t a = (n + 15) & ~(size_t)15; if (bootUsed + a > sizeof boot) return NULL; void* p = boot + bootUsed; bootUsed += a; return p; }
    resolve();
  }
  void* p = realMalloc(n);
  if (p && n >= BIG && nLive < MAXLIVE) { live[nLive++] = p; say('+', n, p); }
  return p;
}

void free(void* p)
{
  if (!p || ((char*)p >= boot && (char*)p < boot + sizeof boot)) return;
  if (!realFree) resolve();
  for (int i = 0; i < nLive; ++i)
    if (live[i] == p) { live[i] = live[--nLive]; say('-', 0, p); break; }
  realFree(p);
}
