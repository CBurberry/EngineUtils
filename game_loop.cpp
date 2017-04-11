#include <SFML/Graphics.hpp>

//#####################################################################################################
//gameprogrammingpatterns.com application of (fixed update loop, variable rendering) - same as Fix Your Timestep! blog post.


/*
* Update physics on a per-frame basis
* update() and update(double) kept separate becuase of potential IEEE 754 auto-formatting
*/
void update();

/* Update physics on a lerp'ed basis calculation */
void update(double alpha);


//Visual updating happens here.
void render(double alpha) {
	//Process the alpha value for update calculation

	//std::cout << "lerp value: " << alpha << std::endl;
	
}

int main() {
	//Setup key SFML features
	sf::RenderWindow window(sf::VideoMode(800, 600), "Prototype");
	sf::Event event;
	sf::Clock gameClock;

	/*At each step (tick), the game loop: 
		processes user input without blocking
		updates the game state
		renders the game 
		lerp inbetween frames
	*/

	sf::Time time = gameClock.getElapsedTime();
	double previous = time.asSeconds();
	double dt = 0.01;									//The number of updates / update() calls we want per second.
	double accumulator = 0.0;
	double current = 0.0;
	double elapsed = 0.0;
	int update_count_per_cycle = 0;
	int frame_counter = 0;
	int update_counter = 0;
	
	while (window.isOpen()) {

		//check for 'close' event
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		time = gameClock.getElapsedTime();
		current = time.asSeconds();
		elapsed = current - previous;
		previous = current;
		accumulator += elapsed;
		

		//Add some input processing here!
		update_count_per_cycle = 0;

		while ( accumulator >= dt ) {
			//update();
			update_count_per_cycle++;
			accumulator -= dt;
		}

		//Linear interpolation for rendering.
		render ( accumulator / dt );

		window.clear();	
		//Draw whatever here...
		window.display();

		//DEBUG OUTPUTS
		//std::cout << "Frame Elapsed time: " + std::to_string(elapsed) << std::endl;
		//std::cout << "Update count: " << update_count_per_cycle << std::endl;
		/*	COUNTERS
			frame_counter++;
			update_counter += update_count_per_cycle;
			if (frame_counter == 59) 
			{
				std::cout << "60 frames | " << update_counter << "updates" << std::endl;
				frame_counter = 0;
				update_counter = 0;
			}
		*/

		//Upper bound limit as 60fps
		while (abs(current - gameClock.getElapsedTime().asSeconds()) < (1.0 / 60.0))
		{
			//Consume time - IDLE
		}
	}


	return 0;
}