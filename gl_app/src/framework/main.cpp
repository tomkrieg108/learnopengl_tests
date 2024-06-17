#include "pch.h"
#include "application.h"

int main()
{
	App::instance().Run();
	std::cout << alignof (int*);
	return 0;
}