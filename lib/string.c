// library for string manipulation

void 
strcpy(char *dst, char *src)
{
	char c;
	while ((c = *(src++)) != '\0') {
		*(dst++) = c;
	}
	*dst = '\0';
}
