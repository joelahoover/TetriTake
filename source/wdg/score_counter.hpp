#ifndef _WDG__SCORE_COUNTER_HPP_
#define _WDG__SCORE_COUNTER_HPP_

#include <SFML/Graphics.hpp>

#include "main/game.hpp"
#include "wdg/outline_box.hpp"
#include "wdg/widget.hpp"

namespace wdg {

class ScoreCounter : public Widget {
public:
	ScoreCounter(Game::Context&, input::ActionManager&, int score);
	virtual ~ScoreCounter();

	// Update the widget
	virtual void update() override;

	// Handle events
	virtual void handleEvent(Event) override;
	virtual void handleEvent(const thor::ActionContext<std::string>&) override;
	
	// Get the enclosing rectangle
	virtual sf::FloatRect getRect() override;
	
	// Draw the widget
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	// Set the score
	void setScore(int);
	
	// Subtract from the score
	void subtract(int);

private:
	Game::Context& mGameContext;
	input::ActionManager& mActionManager;
	
	sf::Vector2f mPosition; // The center position
	sf::Vector2f mSize; // Size extending in each direction (half of total)

	std::shared_ptr<sf::Font> mFont;
	std::unique_ptr<OutlineBox> mOutlineBox;
	
	int mScore;
};

} // namespace wdg

#endif // _WDG__SCORE_COUNTER_HPP_
