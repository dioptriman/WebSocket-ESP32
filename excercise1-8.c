#include <stdio.h>

main(){
	int c;
	int bl = 0;
	int tb = 0;
	int nl = 0;

	while((c = getchar()) != EOF){
		if(c == '\n'){
			++nl;
		}

		if (c == ' '){
			++bl;
		}

		if(c == '\t'){
			++tb;
		}
	}

	printf("Total New Lines: %d\n", nl);
	printf("Total Blank Spaces : %d\n", bl);
	printf("Total New Tabs : %d\n", tb);

}
