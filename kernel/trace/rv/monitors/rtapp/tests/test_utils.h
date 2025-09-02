#ifndef _RV_RTAPP_TEST_UTILS_H
#define _RV_RTAPP_TEST_UTILS_H

#include <fcntl.h>
#include <stdbool.h>

static inline int rtapp_enable(void)
{
	int fd, ret;

	fd = open("/sys/kernel/tracing/rv/enabled_monitors", O_RDWR);
	if (fd == -1)
		return -1;

	ret = dprintf(fd, "rtapp\n");
	close(fd);
	if (ret < 0)
		return -1;

	fd = open("/sys/kernel/tracing/rv/monitors/rtapp/reactors", O_RDWR);
	if (fd == -1)
		return -1;

	ret = dprintf(fd, "sigtrap\n");
	close(fd);
	if (ret < 0)
		return -1;

	return 0;
}

static inline int rtapp_disable(void)
{
	/* FIXME */
	return 0;
}

#endif /* _RV_RTAPP_TEST_UTILS_H */
