#include "nashResolver.h"

void __S_X_BAI_ALGORITHM(const std::vector<double>& p1, const std::vector<double>& p2,
	const size_t& s1, const size_t& s2, 
	std::vector<double>* _p1_, std::vector<double>* _p2_, double* M)
{
	//BASED ON:
	//https://www.sciencedirect.com/science/article/pii/0898122196001538
	
	//M = max_{i,j}(|a_{i,j}|,|b_{i,j}|) + 1
	*M = 0;
	for (size_t i = 0; i < s1 * s2; ++i)
	{
		*M = std::max(*M, std::max(std::abs(p1[i]), std::abs(p2[i])));
	}
	*M += 1;
	//Constructing A^{_} and B^{_}

	//A^{_}
	//a^{_} = a_ij , if a_ij = max_l(a_lj)
	//		   -M  , otherwise
	// l: constant
	for (size_t i = 0; i < s1; ++i)
	{
		size_t offset = 0;
		for (size_t j = 0; j < s2; ++j)
		{
			(*_p1_)[i * s2 + j] = -*M;
			if (p1[i * s2 + j] > p1[i * s2 + offset])
			{
				offset = j;
			}
		}
		(*_p1_)[i * s2 + offset] = p1[i * s2 + offset];
	}
	//B^{_}
	//b^{_} = a_ij , if b_ij = max_l(b_il)
	//		   -M  , otherwise
	// l: constant
	for (size_t j = 0; j < s2; ++j)
	{
		size_t offset = 0;
		for (size_t i = 0; i < s1; ++i)
		{
			(*_p2_)[i * s2 + j] = -*M;
			if (p2[i * s2 + j] > p2[offset * s2 + j])
			{
				offset = i;
			}
		}
		(*_p2_)[offset * s2 + j] = p2[offset * s2 + j];
	}
}

void pureNashEquilibria(const std::vector<double>& p1, const std::vector<double>& p2,
	const size_t& s1, const size_t& s2, std::string* result)
{
	//BASED ON:
	//https://www.sciencedirect.com/science/article/pii/0898122196001538

	double M{ 0 };
	std::vector<double> _p1_(p1.size());
	std::vector<double> _p2_(p2.size());
	
	__S_X_BAI_ALGORITHM(p1, p2, s1, s2, &_p1_, &_p2_, &M);

	*result = "=====================\n";
	*result += "Estrategia pura:\n";

	std::vector<std::pair<size_t, size_t>> pairs;


	//Definition 1
	for (size_t i = 0; i < s1; i++)
	{
		for (size_t j = 0; j < s2; j++)
		{
			if (_p1_[i * s2 + j] > -M && _p2_[i * s2 + j] > -M)
			{
				pairs.push_back({ i , j });
			}
		}
	}

	//Definition 2 && 3 -- Remove comment to apply
	for (const std::pair<size_t, size_t>& pair1 : pairs)
	{
		/*bool isAdmisible{true};
		for (const std::pair<size_t, size_t>& pair2 : pairs)
		{
			if (p1[pair1.first * s2 + pair1.second] < p1[pair2.first * s2 + pair2.second]
				&& p2[pair1.first * s2 + pair1.second] < p2[pair2.first * s2 + pair2.second])
			{
				isAdmisible = false;
				break;
			}
		}*/
		//if (isAdmisible)
		* result += "(" + std::to_string(_p1_[pair1.first * s2 + pair1.second]) + "," + std::to_string(_p2_[pair1.first * s2 + pair1.second]) + ")\n";
	}

	if (pairs.empty())
		*result += "No existe equilibrio de nash usando estrategias puras.\n";

	*result += "=====================\n";
}

void mixedNashEquilibria(const std::vector<double>& p1, const std::vector<double>& p2,
	const size_t& s1, const size_t& s2, std::string* result)
{
	size_t _s1_{ s1 };
	size_t _s2_{ s2 };

	bool existRowDominated{ true };
	bool existColDominated{ true };

	std::vector<double> dom_p1(p1);
	std::vector<double> dom_p2(p2);
	/*
	while (existRowDominated || existColDominated)
	{
		for (size_t i = 0; i < _s1_ - 1; i++)
		{
			for (size_t k = i + 1; k < _s1_; k++)
			{
				bool isP1RowDom{ dom_p1[i * _s2_ + 0] > dom_p1[k * _s2_ + 0] };
				bool isP2RowDom{ dom_p2[i * _s2_ + 0] > dom_p2[k * _s2_ + 0] };
				for (size_t j = 1; j < _s2_; j++)
				{
					if (isP1RowDom != dom_p1[i * _s2_ + j] > dom_p1[k * _s2_ + j])
					{
						existRowDominated = false;
					}
					if (isP2RowDom != dom_p2[i * _s2_ + j] > dom_p2[k * _s2_ + j])
					{
						existRowDominated = false;
					}
					if ((isP1RowDom != dom_p1[i * _s2_ + j] > dom_p1[k * _s2_ + j])
						== (isP2RowDom != dom_p2[i * _s2_ + j] > dom_p2[k * _s2_ + j]))
						break;
				}
				if (existRowDominated)
				{
					if (isP1RowDom || isP2RowDom)
					{
						dom_p1.erase(std::next(dom_p1.begin(), k * _s2_), std::next(dom_p1.begin(), (k + 1) * _s2_));
						dom_p2.erase(std::next(dom_p2.begin(), k * _s2_), std::next(dom_p2.begin(), (k + 1) * _s2_));
						_s1_--;
					}
					break;
				}
			}
			if (existRowDominated)
			{
				break;
			}
		}

		for (size_t j = 0; j < _s2_ - 1; j++)
		{
			for (size_t k = j + 1; k < _s2_; k++)
			{
				bool isP1ColDom{ dom_p1[0 * _s2_ + j] > dom_p1[1 * _s2_ + k] };
				bool isP2ColDom{ dom_p2[0 * _s2_ + j] > dom_p2[1 * _s2_ + k] };
				for (size_t i = 1; i < _s1_; i++)
				{
					if (isP1ColDom != dom_p1[i * _s2_ + j] > dom_p1[i * _s2_ + k])
					{
						existColDominated = false;
					}
					if (isP2ColDom != dom_p2[i * _s2_ + j] > dom_p2[i * _s2_ + k])
					{
						existColDominated = false;
					}
					if ((isP1ColDom != dom_p1[i * _s2_ + j] > dom_p1[i * _s2_ + k])
						== (isP2ColDom != dom_p2[i * _s2_ + j] > dom_p2[i * _s2_ + k]))
						break;
				}
				if (existColDominated)
				{
					if (isP1ColDom || isP2ColDom)
					{
						for (size_t i = 0; i < _s1_; i++)
						{
							dom_p1.erase(std::next(dom_p1.begin(), i * _s2_ + j));
							dom_p2.erase(std::next(dom_p1.begin(), i * _s2_ + j));
						}
						_s2_--;
					}
					break;
				}
			}
			if (existColDominated)
			{
				break;
			}
		}
	}
	*/
	double M{ 0 };
	std::vector<double> _dom_p1_(p1.size());
	std::vector<double> _dom_p2_(p2.size());

	__S_X_BAI_ALGORITHM(p1, p2, _s1_, _s2_, &_dom_p1_, &_dom_p2_, &M);

	*result = "=====================\n";
	*result += "Estrategia mixta:\n";

	*result += "Jugador 1(Horizontal)\n";

	double sum{ 0 };
	for (const double& i: _dom_p1_)
	{
		sum += i * (i > -M);
	}
	
	for (size_t i = 0; i < _s1_; i++)
	{
		double current{ 0 };
		for (size_t j = 0; j < _s2_; j++)
		{
			current += _dom_p1_[i * _s2_ + j] * (_dom_p1_[i * _s2_ + j] > -M);
		}
		*result += "Estrategia " + std::to_string(_s1_ - i) + " = " + std::to_string(current / sum) + "\n";
	}

	*result += "Jugador 2(Vertical)\n";

	sum = 0 ;
	for (const double& i : _dom_p2_)
	{
		sum += i * (i > -M);
	}
	for (size_t j = 0; j < _s2_; j++)
	{
		double current{ 0 };
		for (size_t i = 0; i < _s1_; i++)
		{
			current += _dom_p2_[i * _s2_ + j] * (_dom_p2_[i * _s2_ + j] > -M);
		}
		*result += "Estrategia " + std::string(1, char(_s2_ - j + 'A' - 1)) + " = " + std::to_string(current / sum) + "\n";
	}
}