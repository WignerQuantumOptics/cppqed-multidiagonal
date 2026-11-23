// LD_PRELOAD shim, a stand-in for a padding allocator: every malloc of at least 64 KiB gets 1 KiB more, at its end, so that
// arrays allocated back to back no longer lie a multiple of 1 MiB apart when their size is one (cppqed-multidiagonal#26).
// Allocations made while dlsym resolves the real malloc come from a static buffer.
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stddef.h>

enum { THRESHOLD = 1 << 16, PAD = 1 << 10 };

static void* (*realMalloc)(size_t);
static void (*realFree)(void*);
static int resolving;
static _Alignas(16) char boot[8192];
static size_t bootUsed;

static void resolve(void)
{
  resolving = 1;
  realMalloc = (void* (*)(size_t))dlsym(RTLD_NEXT, "malloc");
  realFree = (void (*)(void*))dlsym(RTLD_NEXT, "free");
  resolving = 0;
}

void* malloc(size_t n)
{
  if (!realMalloc) {
    if (resolving) { size_t a = (n + 15) & ~(size_t)15; if (bootUsed + a > sizeof boot) return NULL; void* p = boot + bootUsed; bootUsed += a; return p; }
    resolve();
  }
  return realMalloc(n >= THRESHOLD ? n + PAD : n);
}

void free(void* p)
{
  if (!p || ((char*)p >= boot && (char*)p < boot + sizeof boot)) return;
  if (!realFree) resolve();
  realFree(p);
}
