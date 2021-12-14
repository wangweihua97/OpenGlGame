#pragma once
#include <iostream>
using namespace std;

#include "Window.h"

class Test
{
public:
	int a;
	Test()
	{
		a = 0;
	}
	Test(int c)
	{
		a = c;
	}
	~Test()
	{
		cout << "11111" << endl;
	}
};
Test T();
Test* T2();
int main();

