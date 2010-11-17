#include <psl1ght/lv2.h>

#include <stdio.h>
#include <malloc.h>
#include <string.h>

int main(int argc, const char* argv[])
{
	// stdout is redirected to the TTY_WRITE syscall. You can view this
	// message by using a payload that hooks TTY_WRITE (403) and displays
	// it somewhere (perhaps using PL3's send_eth network code? Kammy's
	// default loader also installs an ethernet printf hook into TTY_WRITE)
	printf("December %dth, %d. %s.\n", 25, 1986, "Van Nuys");

	char* buffer = (char*)malloc(0x100);
	strcpy(buffer, "This is just an act of kindness\n");
	strcat(buffer, "\tto let you know that your time is up.\n");
	fprintf(stdout, "0x%016llx: %s\n", buffer, buffer);
	free(buffer);

	return 0;
}
