#include "wdg/outline_box.hpp"

#include <Thor/Resources.hpp>

#include "resourcePath.hpp"

namespace wdg {

OutlineBox::OutlineBox(Game::Context& gameContext, sf::Vector2f center, sf::Vector2f halfSize) :
	mBorderTexture(nullptr),
	mCornerTexture(nullptr),
	mPosition(center),
	mSize(halfSize)
{
	mBorderTexture = gameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Texture>(resourcePath() + "graphics/border.png"));
	mCornerTexture = gameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Texture>(resourcePath() + "graphics/corner.png"));
}

OutlineBox::~OutlineBox() {
}

void OutlineBox::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draw the four borders
	sf::RectangleShape border;
	border.setTexture(mBorderTexture.get());
	
	for(int i = 0; i<4; i++) {
		float width;
		sf::Vector2f offset;
		if(i==0 || i==2) {
			// Top or bottom
			width = mSize.x * 2;
			offset = sf::Vector2f(0.f, i==0?-1:1) * mSize.y;
		} else {
			// Left or right
			width = mSize.y * 2;
			offset = sf::Vector2f(i==1?1:-1, 0.f) * mSize.x;
		}
		border.setSize({width, 16.f});
		border.setOrigin({width/2.f, 16.f});
		border.setPosition(mPosition + offset);
		border.setRotation(90.f * i);
		target.draw(border, states);
	}
	
	// Draw the four corners
	sf::RectangleShape corner;
	corner.setTexture(mCornerTexture.get());
	
	for(int i = 0; i<4; i++) {
		corner.setSize({16.f, 16.f});
		corner.setPosition(mPosition + sf::Vector2f(mSize.x*(i==0||i==3?-1:1), mSize.y*(i==0||i==1?-1:1)));
		corner.setOrigin(16.f, 16.f);
		corner.setRotation(90.f * i);
		target.draw(corner, states);
	}
}

} // namespace wdg

