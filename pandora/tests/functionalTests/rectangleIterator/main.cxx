
#include <iostream>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

int main(int argc, char *argv[])
{
	const Engine::Rectangle<int> a(Engine::Size<int>(3,2), Engine::Point2D<int>(2,2));
	std::cout << "rectangle created: " << a << std::endl;

	std::cout << "ranged for" << std::endl;
	for(auto foo : a )
	{
		std::cout << "\tnext : " << foo << std::endl;
	}	

	std::cout << "begin/end: " << std::endl;
	for(auto foo = a.begin(); foo!=a.end(); foo++)
	{
		std::cout << "\tnext: " << *foo << std::endl;
	}
	return 0;
}

