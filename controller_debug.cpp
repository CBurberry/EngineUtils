#include "controller_debug.h"

void queryButtons(sf::Text* displayString) 
{
	for (int i = 0; i < 32; i++)
	{
		if (sf::Joystick::isButtonPressed(0, i))
		{
			displayString->setString("Button " + std::to_string(i) + " pressed.\n");
		}
	}
}

void queryAxis(sf::Text* displayString) 
{
	displayString->setString("Axis X: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X))
		+ "\nAxis Y: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y))
		+ "\nAxis Z: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Z))
		+ "\nAxis R: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::R))
		+ "\nAxis U: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U))
		+ "\nAxis V: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V))
		+ "\nAxis PovX: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX))
		+ "\nAxis PovY: " + std::to_string(sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY)));
}

void queryController(std::ostream& outStream, int n) 
{
	//Check a controller is connected
	if (sf::Joystick::isConnected(n))
	{
		//Joystick number 0 is detected
		//output total buttons/axis' available (buttons are 0-31)
		int axis_c = 0;
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::X))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::Y))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::Z))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::R))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::U))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::V))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::PovX))
		{
			axis_c++;
		}
		if (sf::Joystick::hasAxis(n, sf::Joystick::Axis::PovY))
		{
			axis_c++;
		}

		outStream << "Joystick 0 has connected!" << std::endl;
		outStream << "Number of available buttons: " << sf::Joystick::getButtonCount(n) << "\n"
			<< "Number of available axis: " << axis_c << std::endl;

	}
	else
	{
		outStream << "Exit called! Did you check HID-compliant game controller is enabled in DM & window active?" << std::endl;
	}
}