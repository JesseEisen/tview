#include "common.h"
#include "debug.h"

char *
T_strdup(const char *str)
{
	size_t len;
	char *copy;

	len = strlen(str) + 1;
	if( !(copy = (char *)malloc(len)))  return 0;
	memcpy(copy, str,len);
	return copy;
}

char *
right_trim(char *str)
{
	int len = strlen(str);
	char *cur = str + len - 1; //move to last character
	BOOL is_space;

	while(cur != str && (isspace(*cur)||*cur == '\n')) --cur;
	is_space = (isspace(*cur) || *cur == '\n');
	cur[is_space ? 0 : 1 ] = '\0';

	return str;
}
