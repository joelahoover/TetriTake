#ifndef _WDG__OUTLINE_BOX_HPP_
#define _WDG__OUTLINE_BOX_HPP_

#include <SFML/Graphics.hpp>

#include "main/game.hpp"

namespace wdg {

class OutlineBox : public sf::Drawable {
public:
	OutlineBox(Game::Context&, sf::Vector2f center, sf::Vector2f halfSize);
	virtual ~OutlineBox();
	
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
private:
	std::shared_ptr<sf::Texture> mBorderTexture;
	std::shared_ptr<sf::Texture> mCornerTexture;
	
	sf::Vector2f mPosition; // The center position
	sf::Vector2f mSize; // Size extending in each direction (half of total)
};

} // namespace wdg

#endif // _WDG__OUTLINE_BOX_HPP_
