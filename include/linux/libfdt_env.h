#ifndef _LIBFDT_ENV_H
#define _LIBFDT_ENV_H

<<<<<<< HEAD
#include <linux/kernel.h>
=======
#include <linux/kernel.h>	/* For INT_MAX */
>>>>>>> v4.9.227
#include <linux/string.h>

#include <asm/byteorder.h>

<<<<<<< HEAD
=======
#define INT32_MAX	S32_MAX
#define UINT32_MAX	U32_MAX

typedef __be16 fdt16_t;
typedef __be32 fdt32_t;
typedef __be64 fdt64_t;

>>>>>>> v4.9.227
#define fdt32_to_cpu(x) be32_to_cpu(x)
#define cpu_to_fdt32(x) cpu_to_be32(x)
#define fdt64_to_cpu(x) be64_to_cpu(x)
#define cpu_to_fdt64(x) cpu_to_be64(x)

#endif /* _LIBFDT_ENV_H */
