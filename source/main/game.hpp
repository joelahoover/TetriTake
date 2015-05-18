#ifndef _MAIN__GAME_HPP_
#define _MAIN__GAME_HPP_

#include <memory>

namespace sf {
	class RenderWindow;
}

namespace thor {
	class MultiResourceCache;
	template <typename ActionId>
	class ActionMap;
}

class Game {
public:

	struct Context {
		std::unique_ptr<sf::RenderWindow> window;
		std::unique_ptr<thor::MultiResourceCache> resourceCache;
		std::unique_ptr<thor::ActionMap<std::string>> actionMap;

		Context();
	};

private:
	~Game();
	Game(int argc, const char* argv[]);

	int run();

	friend int main(int, const char**);

	Context mContext;
};

#endif
