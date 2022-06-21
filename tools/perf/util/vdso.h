#ifndef __PERF_VDSO__
#define __PERF_VDSO__

#include <linux/types.h>
#include <string.h>
#include <stdbool.h>

#define VDSO__MAP_NAME "[vdso]"

<<<<<<< HEAD
#define DSO__NAME_VDSO "[vdso]"
=======
#define DSO__NAME_VDSO    "[vdso]"
#define DSO__NAME_VDSO32  "[vdso32]"
#define DSO__NAME_VDSOX32 "[vdsox32]"
>>>>>>> v4.9.227

static inline bool is_vdso_map(const char *filename)
{
	return !strcmp(filename, VDSO__MAP_NAME);
}

struct dso;

bool dso__is_vdso(struct dso *dso);

struct machine;
struct thread;

<<<<<<< HEAD
struct dso *vdso__dso_findnew(struct machine *machine, struct thread *thread);
void vdso__exit(struct machine *machine);
=======
struct dso *machine__findnew_vdso(struct machine *machine, struct thread *thread);
void machine__exit_vdso(struct machine *machine);
>>>>>>> v4.9.227

#endif /* __PERF_VDSO__ */
