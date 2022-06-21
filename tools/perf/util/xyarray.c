#include "xyarray.h"
#include "util.h"

struct xyarray *xyarray__new(int xlen, int ylen, size_t entry_size)
{
	size_t row_size = ylen * entry_size;
	struct xyarray *xy = zalloc(sizeof(*xy) + xlen * row_size);

	if (xy != NULL) {
		xy->entry_size = entry_size;
		xy->row_size   = row_size;
<<<<<<< HEAD
=======
		xy->entries    = xlen * ylen;
>>>>>>> v4.9.227
	}

	return xy;
}

<<<<<<< HEAD
=======
void xyarray__reset(struct xyarray *xy)
{
	size_t n = xy->entries * xy->entry_size;

	memset(xy->contents, 0, n);
}

>>>>>>> v4.9.227
void xyarray__delete(struct xyarray *xy)
{
	free(xy);
}
