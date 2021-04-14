#include "nashResolver.h"
void pushback_double(double d, double c[], int& size)
{
	double* aux = new double[size];
	for (int i = 0; i < size; i++)
	{
		aux[i] = c[i];
	}
	delete[] c;
	size += 1;
	c = new double[size];
	for (int i = 0; i < size - 1; i++)
	{
		c[i] = aux[i];
	}
	c[size - 1] = d;
	delete[] aux;
}

double* biMatrixNashEquilibria
(
	double result[], int& sizeResult, bool mode,
	double** p1, int sizeX,
	double** p2, int sizeY,
	double** _p1, double** _p2,
	double& M
)
{
	//M = max_{i,j}(|a_{i,j}|,|b_{i,j}|) + 1
	double aux{ 0 };
	for (int y = 0; y < sizeY; ++y)
	{
		for (int x = 0; x < sizeX; ++x)
		{
			aux = std::max(std::abs(p1[y][x]), std::abs(p2[y][x]));
			if (M < aux)
				M = aux;
		}
	}
	M += 1;
	//Constructing A^{_} and B^{_}
	_p1 = new double* [sizeY];
	_p2 = new double* [sizeY];
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
	//A^{_}
	for (int x = 0; x < sizeX; x++)
	{
		aux = -M;
		for (int y = 0; y < sizeY; y++)
		{
			if (aux < p1[y][x])
				aux = p1[y][x];
		}
		for (int y = 0; y < sizeY; y++)
		{
			if (p1[y][x] == aux)
				_p1[y][x] = aux;
		}
	}
	//B^{_}
	for (int y = 0; y < sizeY; y++)
	{
		aux = -M;
		for (int x = 0; x < sizeX; x++)
		{
			if (aux < p2[y][x])
				aux = p2[y][x];
		}
		for (int x = 0; x < sizeX; x++)
		{
			if (p2[y][x] == aux)
				_p2[y][x] = aux;
		}
	}
	if (mode)
	{
		bool existSolution{ false };
		for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				if (_p1[y][x] > -M && _p2[y][x] > -M)
				{
					//pushback_double(_p1[y][x], result, sizeResult);
					//pushback_double(_p2[x][y], result, sizeResult);
					double* aux_ = new double[sizeResult];
					for (int i = 0; i < sizeResult; i++)
					{
						aux_[i] = result[i];
					}
					delete[] result;
					result = new double[sizeResult + 1];
					for (int i = 0; i < sizeResult; i++)
					{
						result[i] = aux_[i];
					}
					result[sizeResult] = _p1[y][x];
					sizeResult += 1;
					delete[] aux_;
					aux_ = new double[sizeResult];
					for (int i = 0; i < sizeResult; i++)
					{
						aux_[i] = result[i];
					}
					delete[] result;
					result = new double[sizeResult + 1];
					for (int i = 0; i < sizeResult; i++)
					{
						result[i] = aux_[i];
					}
					result[sizeResult] = _p2[y][x];
					sizeResult += 1;
					delete[] aux_;
					existSolution = true;
				}
			}
		}
		if (!existSolution)
		{
			sizeResult = -1;
		}
	}
	else
	{
		aux = 0;
		double sum1{ 0 };
		double sum2{ 0 };
		for (int y = 0; y < sizeY; y++)
		{
			for (int x = 0; x < sizeX; x++)
			{
				if (_p1[y][x] > -M)
				{
					aux += _p1[y][x];
				}
			}
		}
		sum1 = aux;
		for (int y = 0; y < sizeY; y++)
		{
			aux = 0;
			for (int x = 0; x < sizeX; x++)
			{
				if (_p1[y][x] > -M)
				{
					aux += _p1[y][x];
				}
			}
			//pushback_double(aux / sum1, result, sizeResult);
			double* aux_ = new double[sizeResult];
			for (int i = 0; i < sizeResult; i++)
			{
				aux_[i] = result[i];
			}
			delete[] result;
			result = new double[sizeResult + 1];
			for (int i = 0; i < sizeResult; i++)
			{
				result[i] = aux_[i];
			}
			if (sum1 == 0)
				result[sizeResult] = 0;
			else
				result[sizeResult] = aux / sum1;
			sizeResult += 1;
			delete[] aux_;
		}
		aux = 0;
		for (int x = 0; x < sizeX; x++)
		{
			for (int y = 0; y < sizeY; y++)
			{
				if (_p2[y][x] > -M)
				{
					aux += _p2[y][x];
				}
			}
		}
		sum2 = aux;
		for (int x = 0; x < sizeX; x++)
		{
			aux = 0;
			for (int y = 0; y < sizeY; y++)
			{
				if (_p2[y][x] > -M)
				{
					aux += _p2[y][x];
				}
			}
			//pushback_double(aux / sum2, result, sizeResult);
			double* aux_ = new double[sizeResult];
			for (int i = 0; i < sizeResult; i++)
			{
				aux_[i] = result[i];
			}
			delete[] result;
			result = new double[sizeResult + 1];
			for (int i = 0; i < sizeResult; i++)
			{
				result[i] = aux_[i];
			}
			if (sum2 == 0)
				result[sizeResult] = 0;
			else
				result[sizeResult] = aux / sum2;
			sizeResult += 1;
			delete[] aux_;
		}
	}
	return result;
}