#include "convert.h"

void stringtohtml(char * from, char * to, int size){
	while(*from && size > 10){
		switch(*from){
			case '\r':
			case '\n':
				*(to++) = '<';size --;
				*(to++) = 'b';size --;
				*(to++) = 'r';size --;
				*(to++) = '/';size --;
				*(to++) = '>';size --;
				break;
			case '\"':
				*(to++) = '&';size --;
				*(to++) = 'q';size --;
				*(to++) = 'u';size --;
				*(to++) = 'o';size --;
				*(to++) = 't';size --;
				*(to++) = ';';size --;
				break;
			case '\'':
				*(to++) = '&';size --;
				*(to++) = 'a';size --;
				*(to++) = 'p';size --;
				*(to++) = 'o';size --;
				*(to++) = 's';size --;
				*(to++) = ';';size --;
				break;
			case '&':
				*(to++) = '&';size --;
				*(to++) = 'a';size --;
				*(to++) = 'm';size --;
				*(to++) = 'p';size --;
				*(to++) = ';';size --;
				break;
			case '<':
				*(to++) = '&';size --;
				*(to++) = 'l';size --;
				*(to++) = 't';size --;
				*(to++) = ';';size --;
				break;
			case '>':
				*(to++) = '&';size --;
				*(to++) = 'g';size --;
				*(to++) = 't';size --;
				*(to++) = ';';size --;
				break;
			case '\t':
				*(to++) = ' ';size --;
				break;
			case '\\':
				*(to++) = '\\';size --;
				*(to++) = '\\';size --;
				break;
			case ' ':
				while(*(from+1) == ' ') from++;
			default:
				*(to++) = *from;size --;
				break;
		}
		from++;
	}
	*to = 0;
}