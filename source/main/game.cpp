#include "main/game.hpp"

#include <iostream>
#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <Thor/Resources.hpp>
#include <Thor/Input.hpp>

#include "state/main_menu.hpp"
#include "state/null_state.hpp"

Game::Context::Context() :
window(new sf::RenderWindow()),
resourceCache(new thor::MultiResourceCache()),
actionMap(new thor::ActionMap<std::string>)
{
}

Game::Game(int argc, const char* argv[]) :
mContext()
{
	(*(mContext.actionMap))["closed"] = thor::Action(sf::Event::Closed);
}

Game::~Game() {
}

int Game::run() {
	// Create a window
	mContext.window->create(sf::VideoMode(1000, 800), "TetriTake", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
	mContext.window->setFramerateLimit(60);

	state::State::uptr activeState(new state::MainMenu(mContext));

	while(true)
	{
		// Handle events
		mContext.actionMap->update(*(mContext.window));
		if(mContext.actionMap->isActive("closed")) {
			break;
		}

		// Update
		activeState->update(activeState);

		// Quit if needed
		if(dynamic_cast<state::NullState*>(activeState.get())) {
			// Quit on the null state
			break;
		}

		// Draw
		mContext.window->clear();
		mContext.window->draw(*activeState);
		mContext.window->display();
	}

	// Application has finished running
	mContext.window->close();

	return 0;
}

