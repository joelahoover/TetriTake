#include "state/play_state.hpp"

#include <iostream>
#include <cmath>

#include <Thor/Input.hpp>
#include <Thor/Resources.hpp>

#include "resourcePath.hpp"
#include "state/main_menu.hpp"
#include "wdg/board.hpp"
#include "wdg/next_shape.hpp"
#include "wdg/score_counter.hpp"

namespace state {

PlayState::PlayState(Game::Context& gameContext) :
	mGameContext(gameContext),
	mActionManager(*(gameContext.actionMap), "PS - "),
	mClock(),
	mTexture(),
	mSprite(),
	mUpdateState()
{
	// Setup the input actions
	mActionManager.addAction("quit", thor::Action(sf::Event::Closed)
	                              || thor::Action(sf::Keyboard::Escape, thor::Action::PressOnce)
	                              || thor::Action(sf::Keyboard::Q, thor::Action::PressOnce));
	mActionManager.addCallback("quit", [&](const thor::ActionContext<std::string>&) { 
		mUpdateState = [=](std::unique_ptr<State>& currentState){ currentState.reset(new MainMenu(mGameContext)); };
	});
	
	mActionManager.addAction("resize", thor::Action(sf::Event::Resized));
	mActionManager.addCallback("resize", [&](const thor::ActionContext<std::string>&) {
		
		sf::View view = mGameContext.window->getView();
		auto windowSize = mGameContext.window->getSize();

		float windowRatio = windowSize.x / (float) windowSize.y;
		float viewRatio = 1000.f / 800.f;
		float sizeX = 1;
		float sizeY = 1;
		float posX = 0;
		float posY = 0;

		bool horizontalSpacing = true;
		if (windowRatio < viewRatio)
			horizontalSpacing = false;

		// If horizontalSpacing is true, the black bars will appear on the left and right side.
		// Otherwise, the black bars will appear on the top and bottom.

		if (horizontalSpacing) {
			sizeX = viewRatio / windowRatio;
			posX = (1 - sizeX) / 2.0;
		}
		else {
			sizeY = windowRatio / viewRatio;
			posY = (1 - sizeY) / 2.0;
		}

		view.setViewport( sf::FloatRect(posX, posY, sizeX, sizeY) );

		mGameContext.window->setView(view);
	});
	
	// Setup the widgets
	auto nextShape = std::make_shared<wdg::NextShape>(mGameContext, mActionManager);
	auto scoreCounter = std::make_shared<wdg::ScoreCounter>(mGameContext, mActionManager, 60000);
	mWidgets.push_back(std::make_shared<wdg::Board>(mGameContext, mActionManager, wdg::Board::Setup(20, 10), nextShape, scoreCounter));
	mWidgets.push_back(nextShape);
	mWidgets.push_back(scoreCounter);
}

PlayState::~PlayState() { }

void PlayState::update(std::unique_ptr<State>& currentState) {
	float frameTime = mClock.restart().asSeconds();
	
	// Update the widgets
	for(auto& widget : mWidgets) {
		widget->update();
	}

	// Handle events
	mActionManager.invokeCallbacks(mGameContext.window.get());

	// Update the state if needed
	if(mUpdateState) {
		mUpdateState(currentState);
		return;
	}
	
}

void PlayState::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	/*std::multiset<comp::Drawable::Data, std::function<bool(comp::Drawable::Data, comp::Drawable::Data)>>
		entitySorted([](comp::Drawable::Data a, comp::Drawable::Data b) {
		return std::get<0>(a) < std::get<0>(b); // Sort least to greatest (this will change when Reverse Painter's algorithm is implemented!)
	});

	auto drawable = const_cast<PlayState*>(this)->mEcs.get<comp::Drawable>();

	for(const auto& entity : *drawable) {
		entitySorted.insert(entity.second);
	}

	for(const auto& data : entitySorted) {
		std::get<1>(data)(target, states);
	}*/
	for(auto& widget : mWidgets) {
		target.draw(*widget, states);
	}
}


} // namespace state

