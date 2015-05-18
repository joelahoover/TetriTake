#ifndef _STATE__NULL_STATE_HPP_
#define _STATE__NULL_STATE_HPP_

#include "state/state.hpp"

namespace state {

class NullState : public State {
public:
	virtual ~NullState() override { };

	// Update the state
	virtual void update(std::unique_ptr<State>& current_state) override { }

	// Draw the state -- called once per frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override { }
};

} // namespace state

#endif // _STATE__NULL_STATE_HPP_
