#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>

//Get a printout text on the game window of buttons being pressed.
void queryButtons(sf::Text* displayString);

//Get a printout text on the game window of axis values.
void queryAxis(sf::Text* displayString);

//Query the status of the nth controller, starting from 0.
void queryController(std::ostream& outStream, int n);