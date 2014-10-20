#include <Exception.hxx>
#include <iostream>
#include <Logger.hxx>

int main(int argc, char *argv[])
{
	try
	{
		log_INFO("test", "This is a info log");
		log_INFO("test2", "This is a info log for log file test2.log");
		log_DEBUG("test", "This is a debug log");		
		log_EDEBUG("test", "This is a extreme debug log");

		int variable1 = 5;
		float variable2 = 0.3f;

		log_INFO("test", "The value of variable1 is: " << variable1 << " and variable2: " << variable2);

		std::cout << "end of main" << std::endl;
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}

