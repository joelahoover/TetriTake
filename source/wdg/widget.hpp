#ifndef _WDG__WIDGET_HPP_
#define _WDG__WIDGET_HPP_

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "input/action_manager.hpp"
#include "util/pointers.hpp"

namespace wdg {

enum class Event {
	MOUSE_ENTERED,
	MOUSE_LEFT,
	FOCUS_GAINED,
	FOCUS_LOST
};

class Widget : public sf::Drawable, public util::MakeSmartPointers<Widget> {
public:
	virtual ~Widget() = default;

	// Update the widget
	virtual void update() = 0;

	// Handle events
	virtual void handleEvent(Event) = 0;
	virtual void handleEvent(const thor::ActionContext<std::string>&) = 0;
	
	// Get the enclosing rectangle
	virtual sf::FloatRect getRect() = 0;
	
	// Draw the widget
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;
};

} // namespace wdg

#endif // _WDG__WIDGET_HPP_
