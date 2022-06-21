/*
 *    Copyright IBM Corp. 2007, 2012
 *    Author(s): Peter Oberparleiter <peter.oberparleiter@de.ibm.com>
 */

#ifndef S390_IDSET_H
<<<<<<< HEAD
#define S390_IDSET_H S390_IDSET_H
=======
#define S390_IDSET_H
>>>>>>> v4.9.227

#include <asm/schid.h>

struct idset;

void idset_free(struct idset *set);
<<<<<<< HEAD
void idset_clear(struct idset *set);
=======
>>>>>>> v4.9.227
void idset_fill(struct idset *set);

struct idset *idset_sch_new(void);
void idset_sch_add(struct idset *set, struct subchannel_id id);
void idset_sch_del(struct idset *set, struct subchannel_id id);
void idset_sch_del_subseq(struct idset *set, struct subchannel_id schid);
int idset_sch_contains(struct idset *set, struct subchannel_id id);
<<<<<<< HEAD
int idset_sch_get_first(struct idset *set, struct subchannel_id *id);
=======
>>>>>>> v4.9.227
int idset_is_empty(struct idset *set);
void idset_add_set(struct idset *to, struct idset *from);

#endif /* S390_IDSET_H */
