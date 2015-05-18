#ifndef _WDG__BLOCK_HPP_
#define _WDG__BLOCK_HPP_

#include <SFML/Graphics.hpp>

#include "main/game.hpp"

namespace wdg {

class Block : public sf::Drawable {
public:

	using Color = unsigned int;

	Block();
	Block(Game::Context&, sf::Vector2i position, Color);
	virtual ~Block();
	
	// Draw the block
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	sf::Vector2i getPosition();
	void move(sf::Vector2i);
	Color getColor();
	
	// Returns true if empty
	bool isEmpty();
	
	// Set if block is highlighted
	void setSelected(bool);
	
private:
	std::shared_ptr<sf::Texture> mBlockTexture;
	
	sf::Vector2i mPosition;
	Color mColor;
	
	bool mEmpty;
	bool mIsSelected;
};

} // namespace wdg

#endif // _WDG__BLOCK_HPP_
