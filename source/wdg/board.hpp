#ifndef _WDG__BOARD_HPP_
#define _WDG__BOARD_HPP_

#include <vector>

#include "main/game.hpp"
#include "wdg/arrow.hpp"
#include "wdg/block.hpp"
#include "wdg/next_shape.hpp"
#include "wdg/outline_box.hpp"
#include "wdg/score_counter.hpp"
#include "wdg/shape.hpp"
#include "wdg/widget.hpp"

namespace wdg {

class Board : public Widget {
public:
	struct Setup {
		Setup(unsigned int r, unsigned int c) : num_rows(r), num_cols(c) { }
		
		unsigned int num_rows;
		unsigned int num_cols;
	};

	Board(Game::Context&, input::ActionManager&, Setup, std::shared_ptr<NextShape>, std::shared_ptr<ScoreCounter>);
	virtual ~Board();

	// Update the widget
	virtual void update() override;

	// Handle events
	virtual void handleEvent(Event) override;
	virtual void handleEvent(const thor::ActionContext<std::string>&) override;
	
	// Get the enclosing rectangle
	virtual sf::FloatRect getRect() override;
	
	// Draw the widget
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

private:
	Game::Context& mGameContext;
	input::ActionManager& mActionManager;
	std::shared_ptr<NextShape> mNextShape;
	std::shared_ptr<ScoreCounter> mScoreCounter;

	std::unique_ptr<OutlineBox> mOutlineBox;
	
	std::vector<std::vector<Block>> mBlocks; // Access is blocks[col][row], row starts at top, col starts at left
	sf::Vector2i mBlocksOffset;
	std::vector<std::pair<Block,sf::Vector2i>> mSelected;
	
	std::unique_ptr<Shape> mShape;
	bool mLockShape;
	
	std::vector<Arrow> mArrows;
	using Solution = std::pair<bool, std::tuple<sf::Vector2i,sf::Vector2i,sf::Vector2i,sf::Vector2i>>;
	std::vector<Solution> mInsertSolutions;
	
	sf::Clock mClock;
	
	bool mMouseBecameUp;
	
	// Destroy the piece and return the generating pieces
	void returnSelected();
	
	// Move the shape to the last block and update score
	void finishShape();

	// Verify that a shape is valid on the board
	bool verifyShape(Shape&);
	
	// Verify that all empty rows are above the filled rows
	bool rowVerify();
	
	// Shift down all the rows above r
	void shiftDown(int r);
	
	// Remove any empty rows on the board
	void clearEmptyRows();

	// Update the enables on the arrows
	void updateArrowEnables(bool useActual = true);
	
	// Get the highest row that contains a block
	int getHighestRow();
	
	// Insert the given row
	void insertRow(int);
	
	// Returns row of the top block in the column
	int topInCol(int col);
	
	// Update the insertation solutions
	void updateSolutions();
	
	// TODO
	void solveRowInsert(int row);
	
	// Verify a solution for row r doesn't break row invariant
	bool verifySolution(Solution, int r);
	
	static Solution makeSol(bool x, sf::Vector2i a, sf::Vector2i b, sf::Vector2i c, sf::Vector2i d);
	
	// Convert a position to an index on the board. Returns {-1, -1} if not on board
	sf::Vector2i positionToIndex(sf::Vector2i position);
};

} // namespace wdg

#endif // _WDG__BOARD_HPP_
