
#include "nopsys.h"
#include "utils.h"
#include "libc.h"


void fbreakpoint()
{
	return;
}

int16_t repack(int16_t pixel)
{
  return ((pixel & 0x7fe0) << 1)
       | ((pixel & 0x001f) << 0);
}

int32_t __swap32(int32_t pixel)
{
	int32_t answer;
	__asm("bswap %1" : "=r" (answer): "r" (pixel));
	return answer;
}


int32_t swap32(int32_t pixel)
{
	return (pixel >> 24) | 
	       (pixel << 24) |
	       ((pixel & 0xff0000) >> 8) |
	       ((pixel & 0xff00) << 8);
}


char* parse_string(char string[], int *variable, char separator_token)
{
	if (!string)
		return 0;
		
	char *nextChar = string;
	
	while (*nextChar && *nextChar != separator_token)
	{
		nextChar++;
	}

	char original = *nextChar;
	*nextChar = 0;
	*variable = _atoi(string);
	
	// if we got into the end of the string return 0, else return next position
	return original == 0 ? 0 : nextChar + 1;
}

