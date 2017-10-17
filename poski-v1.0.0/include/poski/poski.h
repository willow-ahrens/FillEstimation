/************************************************************************
 * \file poski.h
 * **********************************************************************/

#ifndef _POSKI_H
#define _POSKI_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <sched.h>

/* For user interfaces */

// commond routines
#include "poski_common.h"
#include "poski_malloc.h"

// system routines
#include "poski_system.h"

// init & close handler
#include "poski_init.h"

// load matrix handler
#include "poski_loadmat.h"

// thread handler
#include "poski_thread.h"

// partition handler
#include "poski_partition.h"

// matrix handler
#include "poski_matrix.h"

// vector handler
#include "poski_vector.h"

// tune matrix handler
#include "poski_tunemat.h"

// kernel handler
#include "poski_kernel.h"

// error & message handler
#include "poski_print.h"

// test
#include "poski_test.h"

#endif //_POSKI_H
