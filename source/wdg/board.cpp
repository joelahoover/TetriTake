#include "wdg/board.hpp"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>

#include "util/foreach.hpp"

namespace wdg {

Board::Board(Game::Context& gameContext, input::ActionManager& actionManager, Setup setup, std::shared_ptr<NextShape> nextShape, std::shared_ptr<ScoreCounter> scoreCounter) :
	mGameContext(gameContext),
	mActionManager(actionManager),
	mNextShape(nextShape),
	mScoreCounter(scoreCounter),
	mOutlineBox(nullptr),
	mBlocks(setup.num_cols, std::vector<Block>(setup.num_rows, Block())),
	mBlocksOffset({200, 80}),
	mShape(nullptr),
	mLockShape(false),
	mArrows(),
	mInsertSolutions(setup.num_rows, {false,{}}),
	mClock()
{
	sf::Vector2f topleft(sf::Vector2f(mBlocksOffset) + sf::Vector2f(-64 - 16.f, -16));
	sf::Vector2f bottomright(sf::Vector2f(mBlocksOffset) + sf::Vector2f(setup.num_cols * 32.f + 16.f, setup.num_rows * 32.f + 16.f));
	mOutlineBox = std::make_unique<OutlineBox>(gameContext, (topleft + bottomright)/2.f, (bottomright - topleft)/2.f);
	
	// Seed the randomness
	std::srand(std::time(0));
	
	for(int row = 8; row < mBlocks[0].size(); row++) {
		int empty = rand()%mBlocks[0].size();
		for(int col = 0; col < mBlocks.size(); col++) {
			if(col == empty) continue;
			// Choose a color that is similar to those around it
			std::vector<Block::Color> options{0, 1};
			if(col != 0 && !mBlocks[col-1][row].isEmpty()) {
				options.insert(options.end(), 3, mBlocks[col-1][row].getColor());
			}
			if(row != 0 && !mBlocks[col][row-1].isEmpty()) {
				options.insert(options.end(), 3, mBlocks[col][row-1].getColor());
			}
			
			mBlocks[col][row] = Block(mGameContext, mBlocksOffset + sf::Vector2i(32*col, 32*row), options[rand()%options.size()]);
		}
	}
	
	for(int row = 0; row < mBlocks[0].size(); row++) {
		Arrow arrow(mGameContext, mBlocksOffset + sf::Vector2i(-64, 32*row));
		mArrows.push_back(std::move(arrow));
	}
	
	updateSolutions();
	
	// Setup the input actions
	
	mActionManager.addAction("rotate", thor::Action(sf::Keyboard::Down, thor::Action::PressOnce) || thor::Action(sf::Keyboard::S, thor::Action::PressOnce));
	mActionManager.addCallback("rotate", [&](const thor::ActionContext<std::string>&) { 
		if(mShape && !mLockShape) {
			auto newShape = std::make_unique<Shape>(*mShape);
			newShape->rotate();
			if(verifyShape(*newShape)) {
				mShape.reset(newShape.release());
			}
		}
	});
	
	mActionManager.addAction("moveLeft", thor::Action(sf::Keyboard::Left, thor::Action::PressOnce) || thor::Action(sf::Keyboard::A, thor::Action::PressOnce));
	mActionManager.addCallback("moveLeft", [&](const thor::ActionContext<std::string>&) { 
		if(mShape && !mLockShape) {
			auto newShape = std::make_unique<Shape>(*mShape);
			newShape->move({-32, 0});
			if(verifyShape(*newShape)) {
				mShape.reset(newShape.release());
			}
		}
	});
	
	mActionManager.addAction("moveRight", thor::Action(sf::Keyboard::Right, thor::Action::PressOnce) || thor::Action(sf::Keyboard::D, thor::Action::PressOnce));
	mActionManager.addCallback("moveRight", [&](const thor::ActionContext<std::string>&) { 
		if(mShape && !mLockShape) {
			auto newShape = std::make_unique<Shape>(*mShape);
			newShape->move({32, 0});
			if(verifyShape(*newShape)) {
				mShape.reset(newShape.release());
			}
		}
	});
	
	mActionManager.addAction("moveUp", thor::Action(sf::Keyboard::Up, thor::Action::PressOnce) || thor::Action(sf::Keyboard::W, thor::Action::PressOnce));
	mActionManager.addCallback("moveUp", [&](const thor::ActionContext<std::string>&) { 
		if(mShape && !mLockShape) {
			auto newShape = std::make_unique<Shape>(*mShape);
			newShape->move({0, -32});
			if(verifyShape(*newShape)) {
				mShape.reset(newShape.release());
			}
		}
	});
	
	/*mActionManager.addAction("del", thor::Action(sf::Mouse::Right, thor::Action::Hold));
	mActionManager.addCallback("del", [&](const thor::ActionContext<std::string>& context) {
		auto pos(positionToIndex(sf::Vector2i(mGameContext.window->mapPixelToCoords(sf::Mouse::getPosition(*context.window)))));
		if(pos.x != -1) {
			mBlocks[pos.x][pos.y] = Block();
		}
	});*/
	
	mActionManager.addAction("insert", thor::Action(sf::Mouse::Left, thor::Action::ReleaseOnce));
	mActionManager.addCallback("insert", [&](const thor::ActionContext<std::string>& context) {
		sf::Vector2f pos(mGameContext.window->mapPixelToCoords(sf::Mouse::getPosition(*context.window)));
		sf::Vector2f index((pos - sf::Vector2f(mBlocksOffset)) / 32.f);
		if(index.x < -2.f || index.x > 0.f || index.y < 0.f || index.y > mBlocks[0].size()
			|| !mInsertSolutions[int(index.y)].first) {
			return;
		}
		insertRow(int(index.y));
		updateSolutions();
	});
	
	mActionManager.addAction("Unselect", thor::Action(sf::Mouse::Right, thor::Action::Hold) || thor::Action(sf::Keyboard::Space, thor::Action::PressOnce));
	mActionManager.addCallback("Unselect", [&](const thor::ActionContext<std::string>& context) {
		if(!mLockShape) {
			mShape.reset();
			returnSelected();
		}
	});
	
	mActionManager.addAction("mbd", !thor::Action(sf::Mouse::Left, thor::Action::Hold));
	mActionManager.addCallback("mbd", [&](const thor::ActionContext<std::string>& context) { mMouseBecameUp = true; });
	
	mActionManager.addAction("Select", thor::Action(sf::Mouse::Left, thor::Action::Hold));
	mActionManager.addCallback("Select", [&](const thor::ActionContext<std::string>& context) {
		if(mShape || !mMouseBecameUp) return;
		
		sf::Vector2i pos(positionToIndex(sf::Vector2i(mGameContext.window->mapPixelToCoords(sf::Mouse::getPosition(*context.window)))));
		int col = pos.x, row = pos.y;
		if(col != -1 && !mBlocks[col][row].isEmpty()) {
			for(auto& b : mSelected) {
				if(sf::Vector2i(col,row) == b.second) return;
			}
			
			mBlocks[col][row].setSelected(true);
			mSelected.push_back({mBlocks[col][row], {col,row}});
			mBlocks[col][row] = Block();
			
			if(mSelected.size() == 4) {
				Shape::BlockGroup bg(mSelected[0].first, mSelected[1].first, mSelected[2].first, mSelected[3].first);
				if(Shape::verify(bg) && rowVerify()) {
					mShape = std::make_unique<Shape>(bg);
				} else {
					returnSelected();
				}
				
				// Lock until next mouse click
				mMouseBecameUp = false;
			}
		}
	});
}

Board::~Board() {
}

void Board::update() {
	if(mShape) {
		if(positionToIndex(std::get<0>(mShape->getBlocks()).getPosition()).y == 0) {
			// Shape is at top of screen
			finishShape();
		} else {
			bool clearAbove = true;
			for_each(mShape->getBlocks(), [&](auto& b) {
				auto index = positionToIndex(b.getPosition());
				for(int i = 0; i < index.y; i++) {
					if(!mBlocks[index.x][i].isEmpty()) clearAbove = false;
				}
			});
			if(clearAbove == true) {
				if(mClock.getElapsedTime() > sf::milliseconds(30)) {
					mShape->move({0, -32});
					mClock.restart();
				}
			}
		}
	}
	
	if(mShape || mSelected.size() != 0) {
		updateArrowEnables(false);
	} else {
		updateArrowEnables();
	}
	
	for(int i = 0; i < mArrows.size(); i++) {
		sf::Vector2f pos(mGameContext.window->mapPixelToCoords(sf::Mouse::getPosition(*mGameContext.window)));
		sf::Vector2f index((pos - sf::Vector2f(mBlocksOffset)) / 32.f);
		if(index.x >= -2.f && index.x <= 0.f && index.y >= i && index.y < i+1) {
			mArrows[i].setHover(true);
		} else {
			mArrows[i].setHover(false);
		}
	}
}

// Handle events
void Board::handleEvent(Event) {
	
}

void Board::handleEvent(const thor::ActionContext<std::string>& context) {
}

// Get the enclosing rectangle
sf::FloatRect Board::getRect() {
	return {0.f, 0.f, 0.f, 0.f};
}

// Draw the widget
void Board::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for(auto& row : mBlocks) {
		for(auto& block : row) {
			target.draw(block, states);
		}
	}
	
	if(mShape) {
		target.draw(*mShape, states);
	} else {
		for(auto& b : mSelected) {
			target.draw(b.first, states);
		}
	}
	
	for(auto& arrow : mArrows) {
		target.draw(arrow, states);
	}
	
	target.draw(*mOutlineBox, states);
}

void Board::returnSelected() {
	// Return blocks to board
	for(auto& b : mSelected) {
		b.first.setSelected(false);
		mBlocks[b.second.x][b.second.y] = b.first;
	}
	mSelected.clear();
}

void Board::finishShape() {
	mShape->standardize();
	mNextShape->setShape(std::move(mShape));
	mScoreCounter->subtract(150);
	mSelected.clear();
	mLockShape = false;
	
	clearEmptyRows();
	
	updateSolutions();
}

bool Board::verifyShape(Shape& s) {
	bool valid = true;
	for_each(s.getBlocks(), [&](auto& x) {
		auto pos = positionToIndex(x.getPosition());
		if(pos.x == -1 || !mBlocks[pos.x][pos.y].isEmpty()) valid = false;
	});
	return valid;
}

bool Board::rowVerify() {
	/*bool blockAbove = false;
	for(int row = 0; row < mBlocks[0].size(); row++) {
		bool blockFound = false;
		for(int col = 0; col < mBlocks.size(); col++) {
			if(!mBlocks[col][row].isEmpty()) blockFound = true;
		}
		
		if(blockAbove && !blockFound) {
			return false;
		}
		blockAbove = blockFound;
	}*/
	
	return true;
}

void Board::shiftDown(int r) {
	for(int row = r; row > 0; row--) {
		for(int col = 0; col < mBlocks.size(); col++) {
			mBlocks[col][row] = mBlocks[col][row-1];
			mBlocks[col][row].move({0,32});
			mBlocks[col][row-1] = Block();
		}
	}
}

void Board::clearEmptyRows() {
	for(int row = 0; row < mBlocks[0].size(); row++) {
		bool rowEmpty = true;
		for(int col = 0; col < mBlocks.size(); col++) {
			if(!mBlocks[col][row].isEmpty()) rowEmpty = false;
		}

		if(rowEmpty) shiftDown(row);
	}
}

sf::Vector2i Board::positionToIndex(sf::Vector2i pos) {
	int col = (pos - mBlocksOffset).x >> 5;
	int row = (pos - mBlocksOffset).y >> 5;
	
	if(col < 0 || col >= mBlocks.size() || row < 0 || row >= mBlocks[0].size()) {
		return {-1, -1};
	}
	
	return {col, row};
}

void Board::insertRow(int row) {
	// Move above rows up
	for(int r = 0; r < row; r++) {
		//std::cout << "r: " << r << std::endl;
		for(int c = 0; c < mBlocks.size(); c++) {
			//std::cout << "c: " << c << std::endl;
			mBlocks[c][r] = mBlocks[c][r+1];
			mBlocks[c][r].move({0,-32});
			mBlocks[c][r+1] = Block();
		}
	}

	// Insert the new blocks on the row
	auto blocks = mInsertSolutions[row].second;
	for(int col = 0; col < mBlocks.size(); col++) {
		bool skip = false;
		for_each(blocks, [&](auto& i) {
			if(i == sf::Vector2i(col, row)) skip = true;
		});
		
		if(!skip) {
			// Choose a color that is similar to those around it
			std::vector<Block::Color> options{0, 1};
			if(col != 0 && !mBlocks[col-1][row].isEmpty()) {
				options.insert(options.end(), 2, mBlocks[col-1][row].getColor());
			}
			if(row != 0 && !mBlocks[col][row-1].isEmpty()) {
				options.insert(options.end(), 2, mBlocks[col][row-1].getColor());
			}
			if(row+1 != mBlocks[0].size() && !mBlocks[col][row+1].isEmpty()) {
				options.insert(options.end(), 2, mBlocks[col][row+1].getColor());
			}
			
			mBlocks[col][row] = Block(mGameContext, mBlocksOffset + sf::Vector2i(32*col, 32*row), options[rand()%options.size()]);
		}
	}

	// Create the removing shape
	Block::Color color = rand() % 2;
	for_each(blocks, [&](auto& i) {
		auto& block = mBlocks[i.x][i.y];
		if(!block.isEmpty()) {
			color = block.getColor();
			block.setSelected(true);
			mSelected.push_back({block, i});
			block = Block();
			//std::cout << "adding (" << i.x << ", " << i.y << "): " << block.getColor() << std::endl;
		}
	});
	
	for_each(blocks, [&](auto& i) {
		if(i.y == row) {
			mSelected.push_back({Block(mGameContext, mBlocksOffset + sf::Vector2i(32*i.x, 32*i.y), color), i});
			mSelected.back().first.setSelected(true);
			//std::cout << "adding (" << i.x << ", " << i.y << "): " << color << std::endl;
		}
	});
	//std::cout << mSelected.size() << std::endl;
	for(auto& block : mSelected) {
		//std::cout << block.first.getColor() << " -- " << block.first.getPosition().x << ", " << block.first.getPosition().y << std::endl;
	}
	Shape::BlockGroup bg(mSelected[0].first, mSelected[1].first, mSelected[2].first, mSelected[3].first);
	mShape = std::make_unique<Shape>(bg);
}

void Board::updateSolutions() {
	// Remove old solutions
	mInsertSolutions.clear();
	for(int i = 0; i < mBlocks[0].size(); i++) mInsertSolutions.push_back({false,{}});

	// Generate the new solutions (only if there is room on the board for an insertation)
	bool roomOnBoard = true;
	for(int col = 0; col < mBlocks.size(); col++) {
		if(!mBlocks[col][0].isEmpty()) roomOnBoard = false;
	}
	
	for(int row = 0; roomOnBoard && row < mBlocks[0].size(); row++) {
		solveRowInsert(row);
	}
	
	// Update the graphics
	updateArrowEnables();
}

void Board::updateArrowEnables(bool useActual) {
	for(int row = 0; row < mBlocks[0].size(); row++) {
		mArrows[row].setEnabled(useActual && mInsertSolutions[row].first);
	}
}

int Board::getHighestRow() {
	for(int row = 0; row < mBlocks[0].size(); row++) {
		for(int col = 0; col < mBlocks.size(); col++) {
			if(!mBlocks[col][row].isEmpty()) return row;
		}
	}
	
	return -1;
}

int Board::topInCol(int col) {
	for(int row = 0; row < mBlocks[0].size(); row++) {
		if(!mBlocks[col][row].isEmpty()) return row;
	}
	
	return -1;
}

Board::Solution Board::makeSol(bool x, sf::Vector2i a, sf::Vector2i b, sf::Vector2i c, sf::Vector2i d) {
	return Board::Solution(x, {a, b, c, d});
}

void Board::solveRowInsert(int row) {
	// Start at a random column
	int offset = rand() % mBlocks.size();
	int col = offset - 1;
	bool first = true;

	while(true) {
		// Incement column, looping back if necessary
		col = (col + 1) % mBlocks.size();
		if(!first && col == offset) break;
		first = false;

		int top = topInCol(col);
		int topleft = col-1<0 ? -2 : topInCol(col-1);
		int topright = col+1==mBlocks.size() ? -2 : topInCol(col+1);
		if(top <= row) {
			// We are not top
			Block::Color color = mBlocks[col][row].getColor();
			
			if(top == row) {
				// Contiguous and match color
				if(mBlocks[col][row].getColor() != color) continue;
				//    -
				//   X-
				// --O-
				if(topright >= 0) {
					if(topright == row-1 && mBlocks[col+1][row-1].getColor() == color && mBlocks[col+1][row].getColor() == color) {
						//  X
						// XX
						// O
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col+1,row-1}, {col+1,row-2});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					} else if(topright == row && mBlocks[col+1][row].getColor() == color) {
						// XX
						// Oo
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col+1,row}, {col+1,row-1});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					} else if(topright > row && col+2 < mBlocks.size() && (topInCol(col+2) > row || topInCol(col+2) == -1)) {
						// X
						// Ooo
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col+1,row}, {col+2,row});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					}
				}
			} else if(top + 1 == row) {
				// Contiguous and match color
				if(mBlocks[col][row].isEmpty() || mBlocks[col][row].getColor() != color
					|| mBlocks[col][row-1].getColor() != color) continue;
				//  X-
				//  X-
				// -O-
				if(topright >= 0) {
					if(topright == row-1 && mBlocks[col+1][row-1].getColor() == color) {
						// XX
						// X
						// O
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col,row-2}, {col+1,row-2});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					} else if(topright == row && mBlocks[col+1][row].getColor() == color) {
						// X
						// XX
						// O
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col,row-2}, {col+1,row-1});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					}
				}
				if(topleft >= 0) {
					if(topleft == row-1 && mBlocks[col-1][row-1].getColor() == color) {
						// XX
						//  X
						//  O
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col,row-2}, {col-1,row-2});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					} else if(topleft == row && mBlocks[col-1][row].getColor() == color) {
						//  X
						// XX
						//  O
						auto sol = makeSol(true, {col, row}, {col, row-1}, {col,row-2}, {col-1,row-1});
						if(verifySolution(sol, row)) {
							mInsertSolutions[row] = sol;
							return;
						}
					}
				}
			} else if(top + 2 == row) {
				if(mBlocks[col][row].isEmpty() || mBlocks[col][row].getColor() != color
					|| mBlocks[col][row-1].isEmpty() || mBlocks[col][row-1].getColor() != color
					|| mBlocks[col][row-2].getColor() != color) continue;
				// X
				// X
				// X
				// O
				auto sol = makeSol(true, {col, row}, {col, row-1}, {col,row-2}, {col,row-3});
				if(verifySolution(sol, row)) {
					mInsertSolutions[row] = sol;
					return;
				}
			} else {
				// Too high in this column
				continue;
			}
		} else {
			// We're on top!
			if(row + 1 < mBlocks[0].size() && !mBlocks[col][row+1].isEmpty()) {
				//
				//  O
				//  X
				Block::Color color = mBlocks[col][row+1].getColor();
				
				if((topleft > row || topleft == -1) && topright == row+1 && mBlocks[col+1][row+1].getColor() == color) {
					// Oo
					// XX
					auto sol = makeSol(true, {col, row}, {col+1, row}, {col,row+1}, {col+1,row+1});
					if(verifySolution(sol, row)) {
						mInsertSolutions[row] = sol;
						return;
					}
				} else if(row+2 < mBlocks[0].size() && (topright > row || topright == -1) && mBlocks[col][row+2].getColor() == color) {
					// Oo
					// X
					// X
					auto sol = makeSol(true, {col, row}, {col+1, row}, {col,row+1}, {col,row+2});
					if(verifySolution(sol, row)) {
						mInsertSolutions[row] = sol;
						return;
					}
				} else if((topleft > row || topleft == -1) && (topright > row || topright == -1)) {
					// oOo
					//  X
					auto sol = makeSol(true, {col, row}, {col-1, row}, {col+1,row}, {col,row+1});
					if(verifySolution(sol, row)) {
						mInsertSolutions[row] = sol;
						return;
					}
				}
			}
		}
	}
}

int adjust(int row, int r) {
	if(row < r) return row+1;
	else return row;
}

bool Board::verifySolution(Solution sol, int r) {
	/*std::vector<int> rows;
	for_each(sol.second, [&](auto& i) {
		rows.push_back(adjust(i.y, r));
	});
	for(int row : rows) {
		bool hasOther = false;
		for(int col = 0; col < mBlocks.size(); col++) {
			if(!mBlocks[col][row].isEmpty()) {
				bool inShape = false;
				for_each(sol.second, [&](auto& i) {
					if(col == i.x && row != r && row != adjust(i.y, r)) inShape = true;
				});
				if(!inShape) {
					hasOther = true;
					break;
				}
			}
		}
		if(!hasOther) return false;
	}*/
	/*std::vector<sf::Vector2i> indexes;
	for_each(sol.second, [&](auto& i) {
		if(i.y != r) {
			indexes.push_back({i.x,adjust(i.y, r)});
		}
	});
	for(auto block : indexes) {
		bool hasOther = false;
		for(int col = 0; col < mBlocks.size(); col++) {
			if(!mBlocks[col][block.y].isEmpty()) {
				bool inShape = false;
				for(auto i : indexes) {
					if(col == i.x && block.y == i.y) inShape = true;
				}
				if(!inShape) {
					hasOther = true;
					continue;
				}
			}
		}
		if(!hasOther) return false;
	}*/
	
	return true;
}

} // namespace wdg

