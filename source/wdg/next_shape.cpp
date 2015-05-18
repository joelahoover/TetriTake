#include "wdg/next_shape.hpp"

#include <Thor/Resources.hpp>

#include "resourcePath.hpp"

namespace wdg {

NextShape::NextShape(Game::Context& gameContext, input::ActionManager& actionManager) :
	mGameContext(gameContext),
	mActionManager(actionManager),
	mPosition({650.f, 150.f}),
	mSize({32.f + 32*6, 32+128+32.f+16}), // x was 128+32.f
	mFont(nullptr),
	mOutlineBox(nullptr)
{
	mFont = mGameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Font>(resourcePath() + "fonts/prstartk.ttf"));
	mOutlineBox = std::make_unique<OutlineBox>(gameContext, mPosition + mSize/2.f, mSize/2.f);
}

NextShape::~NextShape() {
}

void NextShape::update() {
}

void NextShape::handleEvent(Event) {
	
}

void NextShape::handleEvent(const thor::ActionContext<std::string>&) {
	
}

sf::FloatRect NextShape::getRect() {
	return {mPosition, mSize};
}

void NextShape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draw the box outline
	target.draw(*mOutlineBox, states);
	
	sf::Text text("Next", *mFont, 32);
	text.setPosition(mPosition + sf::Vector2f(16.f+32.f, 16.f));
	target.draw(text, states);

	if(mNextShape) {
		target.draw(*mNextShape, states);
	}
}

void NextShape::setShape(std::unique_ptr<Shape> s) {
	mNextShape.reset(s.release());
	mNextShape->move(sf::Vector2i(mPosition + sf::Vector2f(mSize.x/2.f, (16.f+32)/2.f + mSize.y/2.f)) - mNextShape->getCenter());
}

} // namespace wdg

