#ifndef _STATE__MAIN_MENU_BACKGROUND_HPP_
#define _STATE__MAIN_MENU_BACKGROUND_HPP_

#include <memory>

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>

#include "main/game.hpp"
#include "state/state.hpp"

namespace state {

class MainMenuBackground : public State {
public:
	MainMenuBackground(Game::Context&);
	virtual ~MainMenuBackground() override;

	// Update the state
	virtual void update(std::unique_ptr<State>& current_state) override;

	// Draw the state -- called once per frame
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	// Resources Members
	Game::Context& mGameContext;

	std::shared_ptr<sf::Texture> mBackgroundTexture;
	sf::Sprite mBackgroundSprite;

	// State Management Members
	std::function<State*()> mMakeNextState;

	// Animation Members
	float mBackgroundOffset;
};

} // namespace state

#endif // _STATE__MAIN_MENU_BACKGROUND_HPP_
