
#ifndef _NOPSYS_UTIL_H_
#define _NOPSYS_UTIL_H_

int16_t repack(int16_t pixel);
int32_t swap32(int32_t pixel);

char* parse_string(char string[], int *variable, char separator_token);

#endif  /// _NOPSYS_UTIL_H_


