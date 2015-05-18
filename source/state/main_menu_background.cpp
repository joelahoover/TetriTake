#include "state/main_menu_background.hpp"

#include <iostream>

#include "resourcePath.hpp"
#include "state/null_state.hpp"

namespace state {

MainMenuBackground::MainMenuBackground(Game::Context& gameContext) :
	mGameContext(gameContext),
	mBackgroundTexture(),
	mBackgroundSprite(),
	mMakeNextState(),
	mBackgroundOffset(0.f)
{
	// Load the textures
	mBackgroundTexture = mGameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Texture>(resourcePath() + "graphics/background.png"));
	mBackgroundTexture->setRepeated(true);
	mBackgroundTexture->setSmooth(true);

	mBackgroundSprite.setTexture(*mBackgroundTexture, true);
	mBackgroundSprite.setTextureRect({0, 0, 8192, 8192});
	mBackgroundSprite.setScale({0.75f, 0.75f});
	mBackgroundSprite.setPosition({-2048.f, -2048.f});
	mBackgroundSprite.setColor({128, 128, 128, 255});
}

MainMenuBackground::~MainMenuBackground() {
}

void MainMenuBackground::update(std::unique_ptr<State>& currentState) {
	if(mMakeNextState) {
		currentState.reset(mMakeNextState());
		return;
	}

	// Update the background position
	mBackgroundOffset += 1.0;
	auto rect = mBackgroundSprite.getTextureRect();
	rect.left = mBackgroundOffset * .5f + 300;
	rect.top = mBackgroundOffset * .2f;
	mBackgroundSprite.setTextureRect(rect);
}

void MainMenuBackground::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	target.draw(mBackgroundSprite);
}

} // namespace state

