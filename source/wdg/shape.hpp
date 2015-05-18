#ifndef _WDG__SHAPE_HPP_
#define _WDG__SHAPE_HPP_

#include <tuple>

#include "wdg/block.hpp"

namespace wdg {

class Shape : public sf::Drawable {
public:
	enum class Type {I, O, T, J, L, S, Z, UNKNOWN};
	using BlockGroup = std::tuple<Block, Block, Block, Block>;

	Shape(BlockGroup);
	virtual ~Shape();
	
	// Draw the shape
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	
	// Rotate the shape counter-clockwise
	void rotate();
	
	// Move the shape (in pixels)
	// Just a shortcut for moving the individual blocks
	void move(sf::Vector2i offset);
	
	// Get the blocks
	BlockGroup& getBlocks();
	
	// Verify a group of blocks form a Shape
	static bool verify(BlockGroup);
	
	sf::Vector2i getCenter();
	
	// Rotate to standard position and delect blocks
	void standardize();
	
private:
	BlockGroup mBlocks;
	Type mType;
	unsigned int mRotation;
};

} // namespace wdg

#endif // _WDG__SHAPE_HPP_
