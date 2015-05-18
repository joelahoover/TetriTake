#include <iostream>

#include "main/game.hpp"

using namespace std;

int main(int argc, const char* argv[]) {
    try {
        return Game(argc, argv).run();
    }
    catch(exception& e) {
        cerr << "Exception thrown: " << e.what() << endl;
        cerr << "Exiting..." << endl;
        return 1;
    }
    catch(...) {
        cerr << "Unknown exception occured" << endl;
        return 1;
    }
}

