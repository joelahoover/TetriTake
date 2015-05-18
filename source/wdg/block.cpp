#include "wdg/block.hpp"

#include <Thor/Resources.hpp>

#include "resourcePath.hpp"

namespace wdg {

Block::Block() :
	mColor(-1),
	mEmpty(true)
{
}

Block::Block(Game::Context& gameContext, sf::Vector2i position, Color color) :
	mBlockTexture(nullptr),
	mPosition(position),
	mColor(color),
	mEmpty(false),
	mIsSelected(false)
{
	mBlockTexture = gameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Texture>(resourcePath() + "graphics/block.png"));
}

Block::~Block() {
}

// Draw the block
void Block::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if(mEmpty) return;
	sf::Sprite block(*mBlockTexture, sf::IntRect(0.f, 64.f*mColor, 64.f, 64.f));
	block.setPosition(sf::Vector2f(mPosition));
	block.setScale({0.5f, 0.5f});
	target.draw(block, states);
	
	if(mIsSelected) {
		// Draw it lighter
		block.setTexture(sf::Texture());
		block.setColor({255,255,255,96});
		states.blendMode = sf::BlendAdd;
		target.draw(block, states);
	}
}

sf::Vector2i Block::getPosition() {
	return mPosition;
}

void Block::move(sf::Vector2i offset) {
	mPosition += offset;
}

Block::Color Block::getColor() {
	return mColor;
}

bool Block::isEmpty() {
	return mEmpty;
}

void Block::setSelected(bool sel) {
	mIsSelected = sel;
}

} // namespace wdg

