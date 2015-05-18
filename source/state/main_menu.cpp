#include "state/main_menu.hpp"

#include <iostream>
#include <string>

#include <Thor/Resources.hpp>

#include "resourcePath.hpp"
#include "state/main_menu_background.hpp"
#include "state/null_state.hpp"
#include "state/play_state.hpp"

namespace state {

MainMenu::MainMenu(Game::Context& gameContext) :
	mGameContext(gameContext),
	mActionManager(*(gameContext.actionMap), "MMS - "),
	mMakeNextState(),
	mBackground(new MainMenuBackground(gameContext)),
	mFont()
{
	mFont = mGameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Font>(resourcePath() + "fonts/prstartk.ttf"));
	resize();
	
	// Setup the actions
	mActionManager.addAction("play", thor::Action(sf::Keyboard::Space, thor::Action::PressOnce));

	mActionManager.addAction("quit", thor::Action(sf::Event::Closed)
		                          || thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce)
		                          || thor::Action(sf::Keyboard::Q, thor::Action::PressOnce));

	// Setup the callbacks
	mActionManager.addCallback("play", [&](const thor::ActionContext<std::string>&){ mMakeNextState = [&]{ return new PlayState(gameContext); }; });
	mActionManager.addCallback("quit", [&](const thor::ActionContext<std::string>&){ mMakeNextState = []{ return new NullState(); }; });


	
	mActionManager.addAction("resize", thor::Action(sf::Event::Resized));
	mActionManager.addCallback("resize", [&](const thor::ActionContext<std::string>&) { resize(); });
}

MainMenu::~MainMenu() {
}

void MainMenu::update(std::unique_ptr<State>& currentState) {
	// Handle events
	mActionManager.invokeCallbacks(mGameContext.window.get());

	if(mMakeNextState) {
		currentState.reset(mMakeNextState());
		return;
	}

	mBackground->update(mBackground);
}

void MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(*mBackground, states);

	sf::Text text("TetriTake", *mFont, 64);
	text.setPosition({500.f - 4.5f * 64.f, 200.f});
	target.draw(text, states);

	sf::Text text2("Press space to play", *mFont, 32);
	text2.setPosition({500.f - 9.5f * 32.f, 400.f});
	target.draw(text2, states);
}

void MainMenu::resize() {
	sf::View view = mGameContext.window->getView();
	auto windowSize = mGameContext.window->getSize();

	float windowRatio = windowSize.x / (float) windowSize.y;
	float viewRatio = 1000.f / 800.f;

	bool horizontalSpacing = true;
	if (windowRatio < viewRatio)
		horizontalSpacing = false;

	if (horizontalSpacing) {
		view.setCenter(sf::Vector2f(500.f, 400.f));
		view.setSize(sf::Vector2f(1000.f*windowRatio/viewRatio, 800.f));
	}
	else {
		view.setCenter(sf::Vector2f(500.f, 400.f));
		view.setSize(sf::Vector2f(1000.f, 800.f*windowRatio/viewRatio));
	}

	view.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));

	mGameContext.window->setView(view);
}

} // namespace state

