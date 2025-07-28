/*
 * Copyright (c) 2017 Oracle and/or its affiliates. All rights reserved.
 */

#ifdef	CONFIG_SPARC64

/*
 * in case of a 32 bit VDSO for a 64 bit kernel fake a 32 bit kernel
 * configuration
 */
#undef	CONFIG_64BIT
#undef	CONFIG_SPARC64
#define	CONFIG_32BIT
#undef	CONFIG_QUEUED_RWLOCKS
#undef	CONFIG_QUEUED_SPINLOCKS

#endif

#include "../vclock_gettime.c"
