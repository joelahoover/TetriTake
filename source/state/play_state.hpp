#ifndef _STATE__PLAY_STATE_HPP_
#define _STATE__PLAY_STATE_HPP_

#include <memory>
#include <vector>

#include <SFML/System/Time.hpp>

#include "input/action_manager.hpp"
#include "main/game.hpp"
#include "state/state.hpp"
#include "wdg/widget.hpp"

namespace comp {
	class Position;
	class TargetingMouse;
}

namespace state {

class PlayState : public State {
public:
	PlayState(Game::Context&);
	virtual ~PlayState() override;

	// Update the state
	virtual void update(std::unique_ptr<State>& current_state) override;

	// Draw the state -- called once per frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	// Game Logic Members
	Game::Context& mGameContext;
	input::ActionManager mActionManager;
	sf::Clock mClock;

	// Graphics Members
	std::shared_ptr<sf::Texture> mTexture;
	sf::Sprite mSprite;

	// Widget Members
	std::vector<wdg::Widget::sptr> mWidgets;

	// State Management Members
	std::function<void(std::unique_ptr<State>&)> mUpdateState;
};

} // namespace state

#endif // _STATE__PLAY_STATE_HPP_
