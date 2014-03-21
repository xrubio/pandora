
#include <iostream>
#include <Rectangle.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <vector>

int main(int argc, char *argv[])
{
	const Engine::Rectangle<int> a(Engine::Size<int>(3,2), Engine::Point2D<int>(2,2));
	std::cout << "rectangle created: " << a << std::endl;

	for(const auto& foo : a ) { // This is equivalent to simply  for(auto& foo : a ), as there is only one const_iterator
		std::cout << "next : " << foo << std::endl;
// 		foo._x = 3;  // This would give an error because we have a const reference.
	}
	
	for(auto foo : a ) { // This calls a.cbegin() / a.cend(), and const_iterator::operator* returns a const reference to the internal 
		                 // _data point, but we copy it nevertheless into a new variable foo, so we will be able to modify it.
		                 // Of course this means that we need to pay the price for the copy.
		std::cout << "next : " << foo << std::endl;
		foo._x = 3; // We can actually modify it because we have a copy, not a const &
	}	


	std::cout << "begin/end: " << std::endl;
	for(auto foo = a.begin(); foo!=a.end(); foo++) {
		std::cout << "\tconst next pos: " << *foo << std::endl;
// 		foo->_x=3; // This would give an error because const_iterator::operator->() returns (and must return!) a const pointer.
		
		// But this is indeed equivalent to the second for-loop above, and of course works well.
		auto foo2(*foo);
		foo2._x=3;
	}
	return 0;
}

