#ifndef _STATE__MAIN_MENU_HPP_
#define _STATE__MAIN_MENU_HPP_

#include <memory>

#include <SFML/Graphics.hpp>
#include <Thor/Input.hpp>

#include "input/action_manager.hpp"
#include "main/game.hpp"
#include "state/state.hpp"

namespace state {

class MainMenu : public State {
public:
	MainMenu(Game::Context&);
	virtual ~MainMenu() override;

	// Update the state
	virtual void update(std::unique_ptr<State>& current_state) override;

	// Draw the state -- called once per frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	// Resources
	Game::Context& mGameContext;

	// Action Manager
	input::ActionManager mActionManager;

	// State Management
	std::function<State*()> mMakeNextState;
	State::uptr mBackground;
	
	// Graphics
	std::shared_ptr<sf::Font> mFont;
	
	// Fix the view after a resize
	void resize();
};

} // namespace state

#endif // _STATE__MAIN_MENU_HPP_
