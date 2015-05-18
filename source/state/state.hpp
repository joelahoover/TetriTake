#ifndef _STATE__STATE_HPP_
#define _STATE__STATE_HPP_

#include <memory>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "util/pointers.hpp"

namespace state {

class State : public sf::Drawable, public util::MakeSmartPointers<State> {
public:
	virtual ~State() { };

	// Update the state
	virtual void update(State::uptr& active_state) = 0;

	// Draw the state -- called once per frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override = 0;
};

} // namespace state

#endif // _STATE__STATE_HPP_
