#include "Generator.h"

Generator::Generator(int min, int max) : min(min), max(max)
{
    gen = std::mt19937(rd());
    dist = std::uniform_int_distribution<>(min, max);
}

int Generator::generate()
{
    return dist(gen);
}
