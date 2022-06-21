/*
<<<<<<< HEAD
=======

 * Copyright (C) 2015 Thomas Meyer (thomas@m3y3r.de)
>>>>>>> v4.9.227
 * Copyright (C) 2005 Jeff Dike (jdike@karaya.com)
 * Licensed under the GPL
 */

#ifndef __STUB_DATA_H
#define __STUB_DATA_H

<<<<<<< HEAD
#include <sys/time.h>

struct stub_data {
	long offset;
	int fd;
	struct itimerval timer;
=======
#include <time.h>

struct stub_data {
	unsigned long offset;
	int fd;
>>>>>>> v4.9.227
	long err;
};

#endif
