// LD_PRELOAD shim: logs every malloc (and C++ operator new) of at least 512 KiB, and its free, to stderr as "+ size address" / "- 0 address".
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

// C++ operator new/delete (Itanium ABI names), so that allocators that override them (jemalloc, tcmalloc) are logged too:
// they forward to malloc/free above, which forward to the allocator's. Diagnostic only: a failed allocation returns NULL.
void* _Znwm(size_t n) { return malloc(n); }                 // operator new(size_t)
void* _Znam(size_t n) { return malloc(n); }                 // operator new[](size_t)
void _ZdlPv(void* p) { free(p); }                           // operator delete(void*)
void _ZdaPv(void* p) { free(p); }                           // operator delete[](void*)
void _ZdlPvm(void* p, size_t n) { (void)n; free(p); }       // operator delete(void*, size_t)
void _ZdaPvm(void* p, size_t n) { (void)n; free(p); }       // operator delete[](void*, size_t)
