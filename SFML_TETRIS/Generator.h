#ifndef SFML_TETRIS_GENERATOR_H
#define SFML_TETRIS_GENERATOR_H

#include <random>
class Generator
{
private:
	int min, max;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<> dist;
public:
	Generator(int min, int max);
	int generate();
};

#endif
