#include <stdio.h>

#define NONBLANK '-'

main(){
	int c, lastChar;

	lastChar = NONBLANK;

	while((c = getchar()) != EOF){
		if( c == ' '){
			if(lastChar != ' ')
				putchar(c);
		} else
			putchar(c);
		lastChar = c;
	}

}
