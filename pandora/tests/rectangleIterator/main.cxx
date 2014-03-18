
#include <iostream>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

int main(int argc, char *argv[])
{
	Engine::Rectangle<int> a(Engine::Size<int>(3,2), Engine::Point2D<int>(2,2));
	std::cout << "rectangle created: " << a << std::endl;

	const Engine::Rectangle<int> & constA = (const Engine::Rectangle<int> &)a;
	for(auto foo : constA )
	{
		std::cout << "next pos: " << foo << std::endl;
	}

	std::cout << "cbegin/cend: " << std::endl;
	for(auto foo = a.cbegin(); foo!=a.cend(); foo++)
	{
		std::cout << "\tconst next pos: " << *foo << std::endl;
	}
	return 0;
}

