#include "mGame.h"

mGame::mGame(double** p1, double** p2, int x, int y)
{
    player1 = p1;
    player2 = p2;
    sizeX = x;
    sizeY = y;
    resultSize = 0;
    _p1 = _p2 = nullptr;
    result = nullptr;
    M = 0;
    mode = false;
}

mGame::~mGame()
{
    for (int i = 0; i < sizeX; i++)
    {
        delete[] player1[i];
        delete[] player2[i];
    }
    delete[] player1;
    delete[] player2;
}

void mGame::Compute(bool _mode)
{
    mode = _mode;
    result = biMatrixNashEquilibria(
        result, resultSize, _mode,
        player1, sizeY,
        player2, sizeX,
        _p1, _p2, M
    );
}

std::string mGame::get_result()
{
    std::string output = "";

    if (mode)
    {
        output.append("Solucion usando estrategias puras: \n\n");
        if (resultSize == -1)
        {
            output.append("No se encontro solucion \n\n");
            output.append("///////////////////////////////////\n\n");
        }
        else
        {
            for (int i = 0; i < resultSize; i += 2)
            {
                output.append("{ ");
                output.append(std::to_string(result[i]));
                output.append(" , ");
                output.append(std::to_string(result[i + 1]));
                output.append(" }\n");
            }
            output.append("///////////////////////////////////\n\n");
        }
    }
    else
    {
        output.append("Solucion usando estrategias mixtas: \n\n");
        output.append("Jugador 1: \n");
        for (int i = 0; i < sizeX; i++)
        {
            output.append("Estrategia " + std::to_string(i + 1) + ": { ");
            output.append(std::to_string(result[i]));
            output.append(" }\n");
        }
        output.append("\n");
        output.append("Jugador 2: \n");
        for (int i = 0; i < sizeY; i++)
        {
            char letter = 'A';
            output.append("Estrategia ");
            output.append(std::string(1,letter + i));
            output.append(": { ");
            output.append(std::to_string(result[i + sizeX]));
            output.append(" }\n");
        }
        output.append("///////////////////////////////////\n\n");
    }

    return output;
}
