#include "nashResolver.h"
void pushback_double(double const d, double** c, int& size)
{
	double* aux = new double[size];
	for (int i = 0; i < size; i++)
		aux[i] = (*c)[i];
	delete[] *c;
	*c = new double[size + 1];
	for (int i = 0; i < size; i++)
		(*c)[i] = aux[i];
	(*c)[size] = d;
	size += 1;
	delete[] aux;
}

void biMatrixNashEquilibria
(
	double** result, int& sizeResult, bool mode,
	double** p1, int sizeX,	double** p2, int sizeY
)
{
	double M{ 0 };
	//M = max_{i,j}(|a_{i,j}|,|b_{i,j}|) + 1
	double aux{ 0 };
	for (int y = 0; y < sizeY; ++y)
		for (int x = 0; x < sizeX; ++x)
		{
			aux = std::max(std::abs(p1[y][x]), std::abs(p2[y][x]));
			if (M < aux)
				M = aux;
		}
	M += 1;
	//Constructing A^{_} and B^{_}
	double** _p1 = new double* [sizeY];
	double** _p2 = new double* [sizeY];
	for (int y = 0; y < sizeY; y++)
	{
		_p1[y] = new double[sizeX];
		_p2[y] = new double[sizeX];
		for (int x = 0; x < sizeX; x++)
		{
			_p1[y][x] = -M;
			_p2[y][x] = -M;
		}
	}
	int p{ 0 };
	//A^{_}
	for (int x = 0; x < sizeX; x++)
	{
		aux = -M;
		for (int y = 0; y < sizeY; y++)
			if (aux < p1[y][x])
			{
				aux = p1[y][x];
				p = y;
			}
		_p1[p][x] = aux;
	}
	//B^{_}
	p = 0;
	for (int y = 0; y < sizeY; y++)
	{
		aux = -M;
		for (int x = 0; x < sizeX; x++)
			if (aux < p2[y][x])
			{
				aux = p2[y][x];
				p = x;
			}
		_p2[y][p] = aux;
	}
	sizeResult = 0;
	if (mode)
	{
		for (int y = 0; y < sizeY; y++)
			for (int x = 0; x < sizeX; x++)
				if (_p1[y][x] > -M && _p2[y][x] > -M)
				{
					pushback_double(_p1[y][x], result, sizeResult);
					pushback_double(_p2[y][x], result, sizeResult);
				}
	}
	else
	{
		aux = 0;
		double sum{ 0 };
		for (int y = 0; y < sizeY; y++)
			for (int x = 0; x < sizeX; x++)
				if (_p1[y][x] > -M)
					aux += _p1[y][x];
		sum = aux;
		for (int y = 0; y < sizeY; y++)
		{
			aux = 0;
			for (int x = 0; x < sizeX; x++)
				if (_p1[y][x] > -M)
					aux += _p1[y][x];
			pushback_double(aux / sum, result, sizeResult);
		}
		aux = 0;
		for (int x = 0; x < sizeX; x++)
			for (int y = 0; y < sizeY; y++)
				if (_p2[y][x] > -M)
					aux += _p2[y][x];
		sum = aux;
		for (int x = 0; x < sizeX; x++)
		{
			aux = 0;
			for (int y = 0; y < sizeY; y++)
				if (_p2[y][x] > -M)
					aux += _p2[y][x];
			pushback_double(aux / sum, result, sizeResult);
		}
	}
}