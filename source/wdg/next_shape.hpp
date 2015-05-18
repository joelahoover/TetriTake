#ifndef _WDG__NEXT_SHAPE_HPP_
#define _WDG__NEXT_SHAPE_HPP_

#include <memory>

#include <SFML/Graphics.hpp>

#include "main/game.hpp"
#include "wdg/shape.hpp"
#include "wdg/outline_box.hpp"
#include "wdg/widget.hpp"

namespace wdg {

class NextShape : public Widget {
public:
	NextShape(Game::Context&, input::ActionManager&);
	virtual ~NextShape();

	// Update the widget
	virtual void update() override;

	// Handle events
	virtual void handleEvent(Event) override;
	virtual void handleEvent(const thor::ActionContext<std::string>&) override;
	
	// Get the enclosing rectangle
	virtual sf::FloatRect getRect() override;
	
	// Draw the widget
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// Set the shape
	void setShape(std::unique_ptr<Shape>);

private:
	Game::Context& mGameContext;
	input::ActionManager& mActionManager;
	
	sf::Vector2f mPosition; // The center position
	sf::Vector2f mSize; // Size extending in each direction (half of total)

	std::shared_ptr<sf::Font> mFont;
	std::unique_ptr<OutlineBox> mOutlineBox;
	
	std::unique_ptr<Shape> mNextShape;
};

} // namespace wdg

#endif // _WDG__NEXT_SHAPE_HPP_
