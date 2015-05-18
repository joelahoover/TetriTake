#include "wdg/arrow.hpp"

#include <Thor/Resources.hpp>

#include "resourcePath.hpp"

namespace wdg {

Arrow::Arrow(Game::Context& gameContext, sf::Vector2i position) :
	mTexture(nullptr),
	mPosition(position),
	mEnabled(false),
	mHover(false)
{
	mTexture = gameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Texture>(resourcePath() + "graphics/arrow.png"));
}

Arrow::~Arrow() {
}

// Draw the arrow
void Arrow::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	sf::Sprite sprite(*mTexture);
	sprite.setColor({255, 255, 255, sf::Uint8(63 + mEnabled*128 + mEnabled*mHover*64)});
	sprite.setScale({0.5f, 0.25f});
	sprite.setPosition(sf::Vector2f(mPosition));
	target.draw(sprite, states);
}

// Set if the arrow is enabled
void Arrow::setEnabled(bool enabled) {
	mEnabled = enabled;
}

bool Arrow::getEnabled() {
	return mEnabled;
}
	
// Set if the mouse is hovering over the arrow
void Arrow::setHover(bool hover) {
	mHover = hover;
}

} // namespace wdg

