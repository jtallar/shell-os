// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/* sampleCodeModule.c */
#include <clib.h>
int main() {
	int i = 0;
    while (1) {
		i++;
		puts("\n########## pA ##########\n");
		sleep(200);
		if (i == 5) {
			setPriority(2, 0);
		}
		if (i == 10) {
			setPriority(2, 3);
		}
	}	
	return 0;
} 