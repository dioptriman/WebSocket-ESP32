#include <stdio.h>

main(){
	float celcius, fahr;
	int lower, upper, step;

	lower = 0;
	upper = 300;
	step = 20;

	celcius = lower;

	printf("%3s %6s\n", "Cel", "Fahr");

	while(celcius <= upper){
		fahr = ((9.0/5.0) * fahr) + 32.0;
		printf("%3.0f %6.1f\n", celcius, fahr);
		celcius += step;
	}
}
