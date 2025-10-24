#include <stdio.h>

/*-
int foo(int x)
Addes 34 to the provided integer x.
-*/

int foo(int x)
{
	return x + 34;
}

int main()
{
	printf("foo: %d\n", foo(35));
	return 0;
}
