# Logging with pandora

1. Presentation
The logging system of Pandora allows the developer of new simulations to verify that the written source code is working correctly. It has been designed to be as simple and efficient as possible, without cluttering your program with additional calls.
These are some additional features provided by the system:
	- Three logging levels: Info, Debug and Extreme Debug.
	- Possibility to use several different log files.
	- Message generation is built on STL streams.

In this tutorial we will explore in turns the multiple ways to log informations within pandora. We start by a very simple program that does not log any information. You will notice that this program does not contain any informations specific to pandora (World, Config, Agent, Raster ...). This is because in this tutorial we will only explore the aspects relative to the logging of information. 

2. Preparation
Create a file called main.cxx and type in the following content:

```cpp
#include <Exception.hxx>
#include <iostream>

int main(int argc, char *argv[])
{
	try
	{
		std::cout << "end of main" << std::endl;
	}
	catch( std::exception & exceptionThrown )
	{
		std::cout << "exception thrown: " << exceptionThrown.what() << std::endl;
	}
	return 0;
}
```

We can try now to compile this program to verify that our working base is sane. 
Copy the file 03_src/SConstuct in the same folder as your main.cxx.
Compile and execute the code:
```bash
$ scons
$ ./logging
```

The output should be something like:
```bash
$ ./logging
```
end of main

3. First logs
To log information there are three possible calls, depending on the level at which you want to receive the log:
	- log_INFO -> This level of messages will always be written
	- log_DEBUG -> Debug logs are written. A compiling option will have to be use to activate this function. This will be explain in the following.
	- log_EDEBUG -> Extreme Debug logs are useful if you want to verify extensively a particular section of your code. Similarly to debug a compiling option will be used.

All the calls receive two parameters: the name of the file where the message will be stored, at the content of the it. You can combine different levels at the same file. For example:

```cpp
log_INFO("test", "This is a info log");
log_INFO("test2", "This is a info log for log file test2.log");
log_DEBUG("test", "This is a debug log");
log_EDEBUG("test", "This is a extreme debug log");
```

To use the system, you will have to include the file 'Logger.hxx' in your source code:

```cpp
#include <Logger.hxx>
```

You will also have to add the following lines in the main function:

```cpp
		log_INFO("test", "This is a info log");
		log_INFO("test2", "This is a info log for log file test2.log");
		log_DEBUG("test", "This is a debug log");		
		log_EDEBUG("test", "This is a extreme debug log");
```

You can now recompile the main program and start it:
```bash
$ scons
$ ./logging
```

Pandora has now create the files 'test.log' and 'test2.log' inside the directory 'logs'. If you are interested on changing the name of this directory it can be done creating a Configuration for your app (see TUTORIAL CONFIGURATION).


4. Levels of logging
You can notice that only the log of level info is present in the file test.log. To include the debug informations an option has to be given at compilation time:
```bash
$ scons debug=true
$ ./logging
```

Now the test.log file contains also the debug message.

To include the extrem debug messages the procedure is similar:
```bash
$ scons debug=true edebug=true
$ ./logging
```

Now the test.log file contains both the debug and extrem debug messages. Notice that in order to activate the extrem debug messages you have to first activate the debug messages. Therefore both options are passed to scons.


5. Logging of variables
In addition you can log the values of variables. To test this function, add the following lines to the file main.cxx

```cpp
int variable1 = 5;
float variable2 = 0.3f;

log_INFO("test", "The value of variable1 is: " << variable1 << " and variable2: " << variable2);
```

And as always, recompile and start your program:
```bash
$ scons
$ ./logging
```

You can now observe in test.log that the first info message is present as well as the log of the value of the variables.

[Next - Analysis your results (python)](04_pyanalysis.md)
Or [Next - Analysis your results (C++)](05_analysis.md)
p
