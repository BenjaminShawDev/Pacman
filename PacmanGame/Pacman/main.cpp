#include "Pacman.h"
#include <iostream>

//Entry Point for Application
int main(int argc, char* argv[]) {
	
	int munchieCount;
	std::cout << "How many munchies?" << std::endl;
	std::cin >> munchieCount;

	Pacman* game = new Pacman(argc, argv, munchieCount);
}