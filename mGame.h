#pragma once
#include <string>
#include "nashResolver.h"
class mGame
{
private:
	double** player1;
	double** player2;
	double** _p1;
	double** _p2;
	double M;
	int sizeX;
	int sizeY;
	double* result;
	int resultSize;
	bool mode;
public:
	mGame(double** p1, double** p2, int x, int y);
	~mGame();
	void Compute(bool _mode);
	std::string get_result();
};