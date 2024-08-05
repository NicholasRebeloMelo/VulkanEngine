#include "SceneManager.hpp"
#include "Window.hpp"

//std lib
#include <cstdlib>
#include <iostream>
#include <stdexcept>


int main()
{

	ve::SceneManager scene{};

	try 
	{
		scene.run();
	} 
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}