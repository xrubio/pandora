
#include <iostream>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

int main(int argc, char *argv[])
{
	Engine::Rectangle<int> a(Engine::Size<int>(3,2), Engine::Point2D<int>(2,2));
	std::cout << "rectangle created: " << a << std::endl;

	std::cout << "c++11 style" << std::endl;
	for(auto foo : a)
	{
		std::cout << "next pos: " << foo << std::endl;
	}

	return 0;
}

