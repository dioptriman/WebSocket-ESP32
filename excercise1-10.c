#include <stdio.h>

main(){
	int c;

	while((c = getchar()) != EOF){
		if( c == '\t'){
			putchar('\\');
			putchar('t');
		}

		if( c == '\b'){
			putchar('\\');
			putchar('b');
		}
		if ( c == '\\'){
			putchar('\\');
			putchar('\\');
		} 

		if ( c != '\\' && c !=  '\b' && c != '\t')
			putchar(c);
	}

}