
#include "nopsys.h"
#include "ints.h"
#include "stdio.h"
#include "stdbool.h"

computer_t computer;

computer_t* current_computer()
{
	return &computer;
}

uint64_t current_microseconds(void)
{
	//FIXME: These are milliseconds, not micro
	return nopsys_ticks * 1000.0 / TIMER_FREQ_HZ;
}


uint64_t current_seconds(void)
{
	return current_microseconds() / 1000000;
}


void nopsys_exit(void)
{ 
	console_set_debugging(true);
	printf("exited nopsys\n\n");
	serial_enter_debug_mode();
}

/*
void print_memory_state(){
	extern unsigned long endOfMemory,memory,memoryLimit,freeBlock,youngStart;
	printf("Memory: %d \n", memory);
	printf("Young start: %d \n", youngStart);
	printf("Free block: %d \n", freeBlock);
	printf("End of memory: %d \n", endOfMemory);
	printf("Memory limit: %d \n", memoryLimit);
}*/
