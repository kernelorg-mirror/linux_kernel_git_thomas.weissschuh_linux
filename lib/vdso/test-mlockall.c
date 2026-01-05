#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int ret;

	ret = mlockall(MCL_CURRENT | MCL_FUTURE);

	printf("pid=%d ret=%d\n", getpid(), ret);
}
