#include "wdg/score_counter.hpp"

#include <iomanip>
#include <sstream>

#include <SFML/Graphics.hpp>
#include <Thor/Resources.hpp>

#include "resourcePath.hpp"

namespace wdg {

ScoreCounter::ScoreCounter(Game::Context& gameContext, input::ActionManager& actionManager, int score) :
	mGameContext(gameContext),
	mActionManager(actionManager),
	mPosition({650.f, 450.f}),
	mSize({32.f + 32*6, 32 + 64.f}),
	mFont(nullptr),
	mOutlineBox(nullptr),
	mScore(score)
{
	mFont = mGameContext.resourceCache->acquire(thor::Resources::fromFile<sf::Font>(resourcePath() + "fonts/prstartk.ttf"));
	mOutlineBox = std::make_unique<OutlineBox>(gameContext, mPosition + mSize/2.f, mSize/2.f);
}

ScoreCounter::~ScoreCounter() {
}

void ScoreCounter::update() {
}

void ScoreCounter::handleEvent(Event) {
	
}

void ScoreCounter::handleEvent(const thor::ActionContext<std::string>&) {
	
}

sf::FloatRect ScoreCounter::getRect() {
	return {mPosition, mSize};
}

void ScoreCounter::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	// Draw the box outline
	target.draw(*mOutlineBox, states);
	
	sf::Text text("Score:", *mFont, 32);
	text.setPosition(mPosition + sf::Vector2f(16.f, 16.f));
	target.draw(text, states);
	
	std::stringstream ss;
	ss << std::setw(6) << std::setfill('0') << mScore;
	text.setString(ss.str());
	text.move({0,32});
	target.draw(text, states);
}

void ScoreCounter::subtract(int val) {
	mScore -= val;
}

} // namespace wdg

