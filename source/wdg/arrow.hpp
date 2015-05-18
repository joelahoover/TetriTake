#ifndef _WDG__ARROW_HPP_
#define _WDG__ARROW_HPP_

#include <SFML/Graphics.hpp>

#include "main/game.hpp"

namespace wdg {
	
class Arrow : public sf::Drawable {
public:
	Arrow(Game::Context&, sf::Vector2i position);
	virtual ~Arrow();

	// Draw the arrow
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// Set if the arrow is enabled
	void setEnabled(bool);
	bool getEnabled();
	
	// Set if the mouse is hovering over the arrow
	void setHover(bool);

private:
	//Game::Context& mGameContext;
	std::shared_ptr<sf::Texture> mTexture;
	
	sf::Vector2i mPosition;
	
	bool mEnabled;
	bool mHover;
};

} // namespace wdg

#endif // _WDG__ARROW_HPP_
