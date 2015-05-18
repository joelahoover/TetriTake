#include "wdg/shape.hpp"

#include "util/foreach.hpp"

namespace wdg {

// Returns true if a is below (or right if on same line as) b
inline bool belowOrRight(Block a, Block b) {
	auto diff = a.getPosition() - b.getPosition();
	return diff.y > 0 || (diff.y == 0 && diff.x > 0); 
}

inline bool sameRow(Block a, Block b) {
	return (a.getPosition() - b.getPosition()).y == 0;
}

inline bool sameCol(Block a, Block b) {
	return (a.getPosition() - b.getPosition()).x == 0;
}

// Returns true if a and b are next to eachother
inline bool near(Block a, Block b) {
	auto diff = a.getPosition() - b.getPosition();
	return abs(diff.x) + abs(diff.y) <= 32;
}

Shape::Shape(Shape::BlockGroup blocks) :
	mBlocks(),
	mType(Type::UNKNOWN),
	mRotation(0)
{	
	if(!verify(blocks)) {
		// Blocks don't form a shape!
		return;
	}
	
	Block a, b, c, d;
	std::tie(a, b, c, d) = blocks;
	
	// Make a the upmost block on the left
	if(belowOrRight(a,b)) std::swap(a,b);
	if(belowOrRight(a,c)) std::swap(a,c);
	if(belowOrRight(a,d)) std::swap(a,d);
	if(belowOrRight(b,c)) std::swap(b,c);
	if(belowOrRight(b,d)) std::swap(b,d);
	if(belowOrRight(c,d)) std::swap(c,d);

	//   AXXX
	// XXXXX
	//  XXX
	//   X
	if(sameRow(a,b)) {
		//  ABXX
		// XXXX 
		//  XX
		if(sameRow(b,c)) {
			// ABCX
			// XXX 
			if(sameRow(c,d)) {
				mType = Type::I;
				mRotation = 1;
			} else if(sameCol(a,d)) {
				mType = Type::L;
				mRotation = 3;
			} else if(sameCol(b,d)) {
				mType = Type::T;
				mRotation = 3;
			} else if(sameCol(c,d)) {
				mType = Type::J;
				mRotation = 3;
			} else {
				// Not a valid shape?
				mType = Type::UNKNOWN;
			}
		} else if(sameCol(a,c)) {
			// AB
			// CX
			// X
			if(sameCol(b,d)) {
				mType = Type::O;
				mRotation = 0;
			} else if (sameCol(c, d)) {
				mType = Type::J;
				mRotation = 0;
			} else {
				// Not a valid shape?
				mType = Type::UNKNOWN;
			}
		} else if(sameCol(b,c)) {
			// AB
			//  CX
			//  X
			if(sameRow(c,d)) {
				mType = Type::Z;
				mRotation = 1;
			} else if(sameCol(c,d)) {
				mType = Type::L;
				mRotation = 2;
			} else  {
				// Not a valid shape?
				mType = Type::UNKNOWN;
			}
		} else if(sameCol(a,d)) {
			mType = Type::S;
			mRotation = 1;
		} else  {
			// Not a valid shape?
			mType = Type::UNKNOWN;
		}
	} else if(sameCol(a,b)) {
		//  A
		//  BXX
		// XXX
		//  X
		if(sameRow(b,c)) {
			// A
			// BCX
			// XX
			if(sameRow(c,d)) {
				mType = Type::J;
				mRotation = 1;
			} else if(sameCol(b,d)) {
				mType = Type::T;
				mRotation = 0;
			} else if(sameCol(c,d)) {
				mType = Type::S;
				mRotation = 0;
			} else  {
				// Not a valid shape?
				mType = Type::UNKNOWN;
			}
		} else if(sameCol(b,c)) {
			// A
			// B
			// CX
			// X
			if(sameRow(c,d)) {
				mType = Type::L;
				mRotation = 0;
			} else if(sameCol(c,d)) {
				mType = Type::I;
				mRotation = 0;
			} else  {
				// Not a valid shape?
				mType = Type::UNKNOWN;
			}
		} else if(sameCol(b,d)) {
			mType = Type::J;
			mRotation = 2;
		} else {
			// Not a valid shape?
			mType = Type::UNKNOWN;
		}
	} else if(sameCol(a,c)) {
		//  A
		// BCX
		// XX
		if(sameRow(c,d)) {
			mType = Type::T;
			mRotation = 1;
		} else if(sameCol(b,d)) {
			mType = Type::Z;
			mRotation = 0;
		} else if(sameCol(c,d)) {
			mType = Type::T;
			mRotation = 2;
		} else  {
			// Not a valid shape?
			mType = Type::UNKNOWN;
		}
	} else if(sameCol(a,d)) {
		mType = Type::L;
		mRotation = 1;
	} else {
		// Not a valid shape?
		mType = Type::UNKNOWN;
	}
	
	// Hopefully that worked...
	mBlocks = std::tie(a,b,c,d);
}

Shape::~Shape() {
}

// Draw the shape
void Shape::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	for_each(mBlocks, [&](auto& x) {
		target.draw(x, states);
	});
}

// Rotate the shape
void Shape::rotate() {
	Block& a = std::get<0>(mBlocks);
	Block& b = std::get<1>(mBlocks);
	Block& c = std::get<2>(mBlocks);
	Block& d = std::get<3>(mBlocks);
	
	constexpr int bd = 32;
	
	switch(mType) {
		case Type::I:
			if(mRotation == 0) {
				a.move({-2*bd, 2*bd});
				b.move({-bd, bd});
				d.move({bd, -bd});
				mRotation = 1;
			} else {
				a.move({2*bd, -2*bd});
				b.move({bd, -bd});
				d.move({-bd, bd});
				mRotation = 0;
			}
			break;
		case Type::J:
			switch(mRotation) {
				case 0:
					a.move({-bd,0});
					b.move({-2*bd,bd});
					d.move({bd,-bd});
					mRotation++;
					break;
				case 1:
					a.move({bd,0});
					b.move({bd,0});
					c.move({-bd,bd});
					d.move({-bd,bd});
					mRotation++;
					break;
				case 2:
					a.move({-bd, bd});
					c.move({2*bd,-bd});
					d.move({bd, 0});
					mRotation++;
					break;
				case 3:
					a.move({bd, -bd});
					b.move({bd, -bd});
					c.move({-bd, 0});
					d.move({-bd, 0});
					mRotation = 0;
					break;
				default:
					break;
			}
			break;
		case Type::L:
			switch(mRotation) {
				case 0:
					a.move({bd, 0});
					b.move({-bd, 0});
					c.move({0, -bd});
					d.move({0, -bd});
					mRotation++;
					break;
				case 1:
					a.move({-2*bd, 0});
					b.move({bd, -bd});
					d.move({-bd, bd});
					mRotation++;
					break;
				case 2:
					a.move({0, bd});
					b.move({0, bd});
					c.move({bd, 0});
					d.move({-bd, 0});
					mRotation++;
					break;
				case 3:
					a.move({bd, -bd});
					c.move({-bd, bd});
					d.move({2*bd, 0});
					mRotation = 0;
					break;
				default:
					break;
			}
			break;
		case Type::S:
			if(mRotation == 0) {
				a.move({0, bd});
				b.move({bd, 0});
				c.move({-2*bd, bd});
				d.move({-bd, 0});
				mRotation = 1;
			} else {
				a.move({0, -bd});
				b.move({-bd, 0});
				c.move({2*bd, -bd});
				d.move({bd, 0});
				mRotation = 0;
			}
			break;
		case Type::Z:
			if(mRotation == 0) {
				a.move({-2*bd, bd});
				c.move({-bd, bd});
				d.move({bd, 0});
				mRotation = 1;
			} else {
				a.move({2*bd, -bd});
				c.move({bd, -bd});
				d.move({-bd, 0});
				mRotation = 0;
			}
			break;
		case Type::T:
			switch(mRotation) {
				case 0:
					b.move({-bd, 0});
					c.move({-bd, 0});
					d.move({bd, -bd});
					mRotation++;
					break;
				case 1:
					d.move({-bd, bd});
					mRotation++;
					break;
				case 2:
					a.move({-bd, bd});
					b.move({bd, 0});
					c.move({bd, 0});
					mRotation++;
					break;
				case 3:
					a.move({bd, -bd});
					mRotation = 0;
					break;
				default:
					break;
			}
			break;
		case Type::O:
		default:
			break;
	}
}

// Move the shape (in pixels)
// Just a shortcut for moving the individual blocks
void Shape::move(sf::Vector2i offset) {
	for_each(mBlocks, [&](auto& x) {
		x.move(offset);
	});
}

// Get the blocks
Shape::BlockGroup& Shape::getBlocks() {
	return mBlocks;
}

// Verify a group of blocks form a Shape
bool Shape::verify(Shape::BlockGroup blocks) {
	Block a, b, c, d;
	std::tie(a, b, c, d) = blocks;
	
	// First, verify the color
	auto color = a.getColor();
	if(b.getColor() != color || c.getColor() != color || d.getColor() != color) {
		return false;
	}

	// Now check if they are all touching
	// Note: it may be easier just to count the total number of near relationships...
	if(near(a,b)) {
		if(near(a,c) || near(b,c)) {
			if(near(a,d) || near(b,d) || near(c,d)) {
				return true;
			}
		} else if(near(a, d) || near(b, d)) {
			if(near(d, c)) {
				return true;
			}
		}
	} else if(near(a,c)) {
		if(near(c,b)) {
			if(near(a,d) || near(b,d) || near(c,d)) {
				return true;
			}
		} else if(near(c, d)) {
			if(near(d, b)) {
				return true;
			}
		}
	} else if(near(a,d)) {
		if(near(d,b)) {
			if(near(b,c) || near(d,c)) {
				return true;
			}
		} else if(near(d,c)) {
			if(near(c,b)) {
				return true;
			}
		}
	}

	return false;
}

sf::Vector2i Shape::getCenter() {
	switch(mType) {
		case Type::I:
			return (std::get<1>(mBlocks).getPosition() + std::get<2>(mBlocks).getPosition()) / 2 + sf::Vector2i(16, 16);
		case Type::O:
			return std::get<3>(mBlocks).getPosition();// + sf::Vector2i(32, 32);
		case Type::J:
			// This one is inconsistant!! (but looks better)
			if(mRotation <= 1) return std::get<2>(mBlocks).getPosition() + sf::Vector2i(16, 16);
			else return std::get<1>(mBlocks).getPosition() + sf::Vector2i(16, 32); // <--- this 32 should be 16
		case Type::L:
			// This one is inconsistant!! (but looks better)
			if(mRotation == 1 || mRotation == 2) return std::get<2>(mBlocks).getPosition() + sf::Vector2i(16, 16);
			else return std::get<1>(mBlocks).getPosition() + sf::Vector2i(16, 32); // <--- this 32 should be 16
		case Type::S:
			if(mRotation == 1) return std::get<0>(mBlocks).getPosition() + sf::Vector2i(16, 32);
			else return std::get<1>(mBlocks).getPosition() + sf::Vector2i(32, 16);
		case Type::T:
			// This one is inconsistant!! (but looks better)
			if(mRotation == 1 || mRotation == 2) return std::get<2>(mBlocks).getPosition() + sf::Vector2i(16, 16);
			else return std::get<1>(mBlocks).getPosition() + sf::Vector2i(16, 32); // <--- this 32 should be 16
		case Type::Z:
			return std::get<1>(mBlocks).getPosition() + sf::Vector2i(16, 32);
		default:
			return {-1, -1};
	}
}

void Shape::standardize() {
	for_each(mBlocks, [&](auto& x) {
		x.setSelected(false);
	});
	
	// This is a bit overcomplicated, but it works
	while(mRotation != 0) rotate();
	/*if(mType != Type::I)*/ rotate();
	rotate();
	rotate();
}

} // namespace wdg

