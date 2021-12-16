#pragma once
#include <iostream>
#include <vector>
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
vector<Test*> v;
Test T();
Test* T2();
void T3();
int main();

