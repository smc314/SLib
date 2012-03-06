#include <stdlib.h>
#include <stdio.h>
#include "EnEx.h"

void func0(void);
void func1(void);
void func2(void);
void func3(void);
void func4(void);
void func5(void);

int main(void)
{
	func0();
	EnEx printer("printer");
	printer.PrintHitMap();
}

void func0(void)
{
	EnEx ee("func0");

	for(int i = 0; i < 200000; i++){
		func1();
		func2();
		func3();
		func4();
		func5();
	}

}

void func1(void)
{
	EnEx ee("func1");
	int j = 0;

	for(int i = 0; i < 20000; i++){
		j = j + i;
	}

	func2();
}

void func2(void)
{
	EnEx ee("func2");
	int j = 0;

	for(int i = 0; i < 20; i++){
		j = j + i;
	}

	func3();
}

void func3(void)
{
	EnEx ee("func3");
	int j = 0;

	for(int i = 0; i < 20; i++){
		j = j + i;
	}

	func4();
}

void func4(void)
{
	EnEx ee("func4");
	int j = 0;

	for(int i = 0; i < 20; i++){
		j = j + i;
	}

	func5();
}

void func5(void)
{
	EnEx ee("func5");
	int j = 0;

	for(int i = 0; i < 20; i++){
		j = j + i;
	}

}

