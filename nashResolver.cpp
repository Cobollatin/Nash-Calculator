#include <vector>

#include "nashResolver.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <utility>

namespace {
    struct pure_nash_equilibrium {
        size_t player1_strategy;
        size_t player2_strategy;
        double player1_payoff;
        double player2_payoff;
    };

    struct mixed_nash_equilibrium {
        std::vector<double> player1_strategy_probabilities;
        std::vector<double> player2_strategy_probabilities;
    };

    struct TestResult {
        std::string output;
        int passed = 0;
        int failed = 0;
        std::vector<std::string> failed_tests;
    };

    void s_x_bai_algorithm(const std::vector<double>& p1, const std::vector<double>& p2,
        const size_t s1, const size_t s2,
        std::vector<double>& modified_p1, std::vector<double>& modified_p2, double& M)
    {
        // Compute M = max_{i,j}(|a_{i,j}|,|b_{i,j}|) + 1
        M = 0;
        for (size_t i = 0; i < s1 * s2; ++i)
        {
            M = std::max(M, std::max(std::abs(p1[i]), std::abs(p2[i])));
        }
        M += 1;

        // Initialize modified_p1 and modified_p2 with -M
        modified_p1.assign(s1 * s2, -M);
        modified_p2.assign(s1 * s2, -M);

        // Construct modified_p1
        // For each column j
        for (size_t j = 0; j < s2; ++j)
        {
            // Find the maximum of p1 over all i at column j
            double max_val = p1[j];
            std::vector<size_t> max_indices;
            max_indices.push_back(0);

            for (size_t i = 1; i < s1; ++i)
            {
                const double val = p1[i * s2 + j];
                if (val > max_val)
                {
                    max_val = val;
                    max_indices.clear();
                    max_indices.push_back(i);
                }
                else if (val == max_val)
                {
                    max_indices.push_back(i);
                }
            }

            // Set modified_p1[i * s2 + j] = p1[i * s2 + j] for max indices
            for (const size_t i : max_indices)
            {
                modified_p1[i * s2 + j] = p1[i * s2 + j];
            }
        }

        // Construct modified_p2
        // For each row i
        for (size_t i = 0; i < s1; ++i)
        {
            // Find the maximum of p2 over all j at row i
            double max_val = p2[i * s2];
            std::vector<size_t> max_indices;
            max_indices.push_back(0);

            for (size_t j = 1; j < s2; ++j)
            {
                const double val = p2[i * s2 + j];
                if (val > max_val)
                {
                    max_val = val;
                    max_indices.clear();
                    max_indices.push_back(j);
                }
                else if (val == max_val)
                {
                    max_indices.push_back(j);
                }
            }

            // Set modified_p2[i * s2 + j] = p2[i * s2 + j] for max indices
            for (const size_t j : max_indices)
            {
                modified_p2[i * s2 + j] = p2[i * s2 + j];
            }
        }
    }

    std::vector<pure_nash_equilibrium> _calculate_pure_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2,
        const size_t s1, const size_t s2)
    {
        double m = 0;
        std::vector<double> modified_p1, modified_p2;

        s_x_bai_algorithm(p1, p2, s1, s2, modified_p1, modified_p2, m);

        std::vector<pure_nash_equilibrium> pure_nash_equilibrium;

        // Find positions where both modified_p1 and modified_p2 are greater than -M
        for (size_t i = 0; i < s1; ++i)
        {
            for (size_t j = 0; j < s2; ++j)
            {
                if (modified_p1[i * s2 + j] > -m && modified_p2[i * s2 + j] > -m)
                {
                    pure_nash_equilibrium.push_back({
                        i,  // Player 1's strategy index
                        j,  // Player 2's strategy index
                        p1[i * s2 + j], // Player 1's payoff
                        p2[i * s2 + j]  // Player 2's payoff
                        });
                }
            }
        }

        return pure_nash_equilibrium;
    }

    std::string format_double(const double value)
    {
        if (std::floor(value) == value)
        {
            // It's an integer
            return std::to_string(static_cast<int>(value));
        }
        else
        {
            // It's not an integer, format with precision
            std::ostringstream oss;
            oss << value;
            return oss.str();
        }
    }

    std::string format_pure_nash_equilibrium(const std::vector<pure_nash_equilibrium>& equilibrium)
    {
        std::ostringstream oss;
        oss << "=====================\n";
        oss << "Pure Strategy Equilibrium:\n";

        if (equilibrium.empty())
        {
            oss << "No Nash equilibrium in pure strategies.\n";
        }
        else
        {
            for (const auto& pure_nash_equilibrium : equilibrium)
            {
                oss << "Player 1 chooses strategy " << pure_nash_equilibrium.player1_strategy
                    << ", Player 2 chooses strategy " << pure_nash_equilibrium.player2_strategy << "\n";
                oss << "Payoffs: (" << format_double(pure_nash_equilibrium.player1_payoff) << ", "
                    << format_double(pure_nash_equilibrium.player2_payoff) << ")\n";
            }
        }

        oss << "=====================\n";
        return oss.str();
    }

    mixed_nash_equilibrium _calculate_mixed_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2,
        const size_t s1, const size_t s2)
    {
        size_t _s1_{ s1 };
        size_t _s2_{ s2 };

        bool existRowDominated{ true };
        bool existColDominated{ true };

        std::vector dom_p1(p1);
        std::vector dom_p2(p2);

        while (existRowDominated || existColDominated)
        {
            existRowDominated = false;
            // Check for dominated rows
            for (size_t i = 0; i < _s1_; i++)
            {
                for (size_t k = 0; k < _s1_; k++)
                {
                    if (i == k) continue;

                    bool isDominatedP1 = true;
                    bool isDominatedP2 = true;

                    for (size_t j = 0; j < _s2_; j++)
                    {
                        if (dom_p1[i * _s2_ + j] >= dom_p1[k * _s2_ + j])
                            isDominatedP1 = false;
                        if (dom_p2[i * _s2_ + j] >= dom_p2[k * _s2_ + j])
                            isDominatedP2 = false;
                    }

                    if (isDominatedP1 || isDominatedP2)
                    {
                        // Remove dominated row i
                        dom_p1.erase(dom_p1.begin() + i * _s2_, dom_p1.begin() + (i + 1) * _s2_);
                        dom_p2.erase(dom_p2.begin() + i * _s2_, dom_p2.begin() + (i + 1) * _s2_);
                        _s1_--;
                        existRowDominated = true;
                        break;
                    }
                }
                if (existRowDominated)
                {
                    break;
                }
            }

            if (existRowDominated)
                continue;

            existColDominated = false;
            // Check for dominated columns
            for (size_t j = 0; j < _s2_; j++)
            {
                for (size_t k = 0; k < _s2_; k++)
                {
                    if (j == k) continue;

                    bool isDominatedP1 = true;
                    bool isDominatedP2 = true;

                    for (size_t i = 0; i < _s1_; i++)
                    {
                        if (dom_p1[i * _s2_ + j] >= dom_p1[i * _s2_ + k])
                            isDominatedP1 = false;
                        if (dom_p2[i * _s2_ + j] >= dom_p2[i * _s2_ + k])
                            isDominatedP2 = false;
                    }

                    if (isDominatedP1 || isDominatedP2)
                    {
                        // Remove dominated column j
                        for (size_t i = 0; i < _s1_; i++)
                        {
                            dom_p1.erase(dom_p1.begin() + i * _s2_ + j - i * (_s2_ - 1));
                            dom_p2.erase(dom_p2.begin() + i * _s2_ + j - i * (_s2_ - 1));
                        }
                        _s2_--;
                        existColDominated = true;
                        break;
                    }
                }
                if (existColDominated)
                {
                    break;
                }
            }

            if (!existRowDominated && !existColDominated)
            {
                break;
            }
        }


        double M{ 0 };
        std::vector<double> _dom_p1_(p1.size());
        std::vector<double> _dom_p2_(p2.size());

        s_x_bai_algorithm(p1, p2, _s1_, _s2_, _dom_p1_, _dom_p2_, M);

        mixed_nash_equilibrium result;

        // Player 1 strategy probabilities
        double sum{ 0 };
        for (const double& i : _dom_p1_)
        {
            if (i > -M)
                sum += i;
        }

        if (sum == 0)
        {
            // Assign equal probabilities
            double equal_prob = 1.0 / _s1_;
            for (size_t i = 0; i < _s1_; i++)
            {
                result.player1_strategy_probabilities.push_back(equal_prob);
            }
        }
        else
        {
            for (size_t i = 0; i < _s1_; i++)
            {
                double current{ 0 };
                for (size_t j = 0; j < _s2_; j++)
                {
                    if (_dom_p1_[i * _s2_ + j] > -M)
                        current += _dom_p1_[i * _s2_ + j];
                }
                double probability = current / sum;
                result.player1_strategy_probabilities.push_back(probability);
            }
        }

        // Player 2 strategy probabilities
        sum = 0;
        for (const double& i : _dom_p2_)
        {
            if (i > -M)
                sum += i;
        }

        if (sum == 0)
        {
            // Assign equal probabilities
            double equal_prob = 1.0 / _s2_;
            for (size_t j = 0; j < _s2_; j++)
            {
                result.player2_strategy_probabilities.push_back(equal_prob);
            }
        }
        else
        {
            for (size_t j = 0; j < _s2_; j++)
            {
                double current{ 0 };
                for (size_t i = 0; i < _s1_; i++)
                {
                    if (_dom_p2_[i * _s2_ + j] > -M)
                        current += _dom_p2_[i * _s2_ + j];
                }
                double probability = current / sum;
                result.player2_strategy_probabilities.push_back(probability);
            }
        }

        return result;
    }

    std::string format_mixed_nash_equilibrium(const mixed_nash_equilibrium& equilibrium)
    {
        std::ostringstream oss;
        oss << "=====================\n";
        oss << "Mixed Strategy Equilibrium:\n";

        oss << "Player 1 (Horizontal):\n";

        for (size_t i = 0; i < equilibrium.player1_strategy_probabilities.size(); ++i)
        {
            oss << "Strategy " << i << " = " << std::fixed << std::setprecision(2)
                << equilibrium.player1_strategy_probabilities[i] << "\n";
        }

        oss << "Player 2 (Vertical):\n";

        for (size_t i = 0; i < equilibrium.player2_strategy_probabilities.size(); ++i)
        {
            oss << "Strategy " << i << " = " << std::fixed << std::setprecision(2)
                << equilibrium.player2_strategy_probabilities[i] << "\n";
        }

        oss << "=====================\n";
        return oss.str();
    }
}

std::string calculate_pure_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2, const size_t& s1, const size_t& s2)
{
    return format_pure_nash_equilibrium(_calculate_pure_nash_equilibrium(p1, p2, s1, s2));
}

std::string calculate_mixed_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2,
    const size_t& s1, const size_t& s2)
{
    return format_mixed_nash_equilibrium(_calculate_mixed_nash_equilibrium(p1, p2, s1, s2));
}

namespace {
    TestResult run_calculate_pure_nash_equilibrium_tests()
    {
        TestResult result;
        int total_passed = 0;
        int total_failed = 0;

        // Helper function to format doubles
        auto format_double = [](double value) -> std::string
            {
                if (std::floor(value) == value)
                {
                    // It's an integer
                    return std::to_string(static_cast<int>(value));
                }
                // It's not an integer, format with precision
                std::ostringstream oss;
                oss << value;
                return oss.str();
            };

        // Helper function to format and display the payoff matrices
        auto format_payoff_matrix = [&](const std::vector<double>& p1, const std::vector<double>& p2, size_t s1, size_t s2) -> std::string
            {
                std::ostringstream oss;
                oss << "Payoff Matrix:\n";
                oss << "Format: (Player1 payoff, Player2 payoff)\n\n";

                // Header for Player 2's strategies
                oss << std::setw(20) << "";
                for (size_t j = 0; j < s2; ++j)
                {
                    oss << std::setw(20) << "Player2 Strategy " + std::to_string(j);
                }
                oss << "\n";

                // Matrix rows for Player 1's strategies
                for (size_t i = 0; i < s1; ++i)
                {
                    oss << std::setw(20) << "Player1 Strategy " + std::to_string(i);
                    for (size_t j = 0; j < s2; ++j)
                    {
                        oss << std::setw(20) << "(" + format_double(p1[i * s2 + j]) + ", " + format_double(p2[i * s2 + j]) + ")";
                    }
                    oss << "\n";
                }
                oss << "\n";
                return oss.str();
            };

        // Prisoner's Dilemma
        {
            std::string test_name = "Prisoner's Dilemma Test";
            result.output += test_name + "\n";

            result.output += "Prisoner's Dilemma Test\n";

            // Payoff matrix:
            //          C       D
            //     C  (3,3)  (0,5)
            //     D  (5,0)  (1,1)
            std::vector<double> p1 = { 3, 0, 5, 1 };
            std::vector<double> p2 = { 3, 5, 0, 1 };
            size_t s1 = 2; // number of strategies for player 1
            size_t s2 = 2; // number of strategies for player 2

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (1, 1)\n"
                "=====================\n";

            std::string actual_output = calculate_pure_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Battle of the Sexes
        {
            std::string test_name = "Battle of the Sexes Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          Ballet  Football
            // Ballet   (2,1)     (0,0)
            // Football (0,0)     (1,2)
            std::vector<double> p1 = { 2, 0, 0, 1 };
            std::vector<double> p2 = { 1, 0, 0, 2 };
            size_t s1 = 2;
            size_t s2 = 2;

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 0\n"
                "Payoffs: (2, 1)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (1, 2)\n"
                "=====================\n";

            std::string actual_output = calculate_pure_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Matching Pennies
        {
            std::string test_name = "Matching Pennies Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          H     T
            // H      (1,-1) (-1,1)
            // T      (-1,1) (1,-1)
            std::vector<double> p1 = { 1, -1, -1, 1 };
            std::vector<double> p2 = { -1, 1, 1, -1 };
            size_t s1 = 2;
            size_t s2 = 2;

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "No Nash equilibrium in pure strategies.\n"
                "=====================\n";

            std::string actual_output = calculate_pure_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Stag Hunt
        {
            std::string test_name = "Stag Hunt Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          Stag   Hare
            // Stag   (4,4)   (1,1)
            // Hare   (1,1)   (2,2)
            std::vector<double> p1 = { 4, 1, 1, 2 };
            std::vector<double> p2 = { 4, 1, 1, 2 };
            size_t s1 = 2;
            size_t s2 = 2;

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 0\n"
                "Payoffs: (4, 4)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (2, 2)\n"
                "=====================\n";

            std::string actual_output = calculate_pure_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Hawk-Dove Game
        {
            std::string test_name = "Hawk-Dove  Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          Hawk    Dove
            // Hawk   (0,0)    (7,2)
            // Dove   (2,7)    (2,2)
            std::vector<double> p1 = { 0, 7, 2, 2 };
            std::vector<double> p2 = { 0, 2, 7, 2 };
            size_t s1 = 2;
            size_t s2 = 2;

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 1\n"
                "Payoffs: (7, 2)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 0\n"
                "Payoffs: (2, 7)\n"
                "=====================\n";

            std::string actual_output = calculate_pure_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        result.failed = total_failed;
        result.passed = total_passed;
        return result;
    }

    TestResult  run_calculate_mixed_nash_equilibrium_tests()
    {
        TestResult result;
        int total_passed = 0;
        int total_failed = 0;

        // Helper function to format doubles
        auto format_double = [](double value) -> std::string
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << value;
                return oss.str();
            };

        // Helper function to format and display the payoff matrices
        auto format_payoff_matrix = [&](const std::vector<double>& p1, const std::vector<double>& p2, size_t s1, size_t s2) -> std::string
            {
                std::ostringstream oss;
                oss << "Payoff Matrix:\n";
                oss << "Format: (Player1 payoff, Player2 payoff)\n\n";

                // Header for Player 2's strategies
                oss << std::setw(20) << "";
                for (size_t j = 0; j < s2; ++j)
                {
                    oss << std::setw(20) << "Player2 Strategy " + std::to_string(j);
                }
                oss << "\n";

                // Matrix rows for Player 1's strategies
                for (size_t i = 0; i < s1; ++i)
                {
                    oss << std::setw(20) << "Player1 Strategy " + std::to_string(i);
                    for (size_t j = 0; j < s2; ++j)
                    {
                        oss << std::setw(20) << "(" + format_double(p1[i * s2 + j]) + ", " + format_double(p2[i * s2 + j]) + ")";
                    }
                    oss << "\n";
                }
                oss << "\n";
                return oss.str();
            };

        // Matching Pennies Test
        {
            std::string test_name = "Matching Pennies Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          H     T
            // H      (1,-1) (-1,1)
            // T      (-1,1) (1,-1)
            std::vector<double> p1 = { 1, -1, -1, 1 };
            std::vector<double> p2 = { -1, 1, 1, -1 };
            size_t s1 = 2;
            size_t s2 = 2;

            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Mixed Strategy Equilibrium:\n"
                "Player 1 (Horizontal):\n"
                "Strategy 0 = 0.50\n"
                "Strategy 1 = 0.50\n"
                "Player 2 (Vertical):\n"
                "Strategy 0 = 0.50\n"
                "Strategy 1 = 0.50\n"
                "=====================\n";

            std::string actual_output = calculate_mixed_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Rock-Paper-Scissors Test
        {
            std::string test_name = "Rock-Paper-Scissors Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          Rock    Paper   Scissors
            // Rock     (0,0)  (-1,1)   (1,-1)
            // Paper    (1,-1)  (0,0)  (-1,1)
            // Scissors (-1,1)  (1,-1)  (0,0)
            std::vector<double> p1 = { 0, -1, 1, 1, 0, -1, -1, 1, 0 };
            std::vector<double> p2 = { 0, 1, -1, -1, 0, 1, 1, -1, 0 };
            size_t s1 = 3;
            size_t s2 = 3;

            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Mixed Strategy Equilibrium:\n"
                "Player 1 (Horizontal):\n"
                "Strategy 0 = 0.33\n"
                "Strategy 1 = 0.33\n"
                "Strategy 2 = 0.33\n"
                "Player 2 (Vertical):\n"
                "Strategy 0 = 0.33\n"
                "Strategy 1 = 0.33\n"
                "Strategy 2 = 0.33\n"
                "=====================\n";

            std::string actual_output = calculate_mixed_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        // Battle of the Sexes Test
        {
            std::string test_name = "Battle of the Sexes Test";
            result.output += test_name + "\n";

            // Payoff matrix:
            //          Ballet  Football
            // Ballet   (2,1)     (0,0)
            // Football (0,0)     (1,2)
            std::vector<double> p1 = { 2, 0, 0, 1 };
            std::vector<double> p2 = { 1, 0, 0, 2 };
            size_t s1 = 2;
            size_t s2 = 2;

            result.output += format_payoff_matrix(p1, p2, s1, s2);

            std::string expected_output =
                "=====================\n"
                "Mixed Strategy Equilibrium:\n"
                "Player 1 (Horizontal):\n"
                "Strategy 0 = 0.67\n"
                "Strategy 1 = 0.33\n"
                "Player 2 (Vertical):\n"
                "Strategy 0 = 0.33\n"
                "Strategy 1 = 0.67\n"
                "=====================\n";

            std::string actual_output = calculate_mixed_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        {
            std::string test_name = "Zero Payoff Test";
            result.output += test_name + "\n";

            // Payoff matrix with zero payoffs
            std::vector<double> p1 = { 0, 0, 0, 0 };
            std::vector<double> p2 = { 0, 0, 0, 0 };
            size_t s1 = 2;
            size_t s2 = 2;

            // Display the payoff matrix
            result.output += format_payoff_matrix(p1, p2, s1, s2);

            // Expected Output
            std::string expected_output =
                "=====================\n"
                "Mixed Strategy Equilibrium:\n"
                "Player 1 (Horizontal):\n"
                "Strategy 0 = 0.50\n"
                "Strategy 1 = 0.50\n"
                "Player 2 (Vertical):\n"
                "Strategy 0 = 0.50\n"
                "Strategy 1 = 0.50\n"
                "=====================\n";

            std::string actual_output = calculate_mixed_nash_equilibrium(p1, p2, s1, s2);

            result.output += "Expected Output:\n" + expected_output;
            result.output += "Actual Output:\n" + actual_output;

            if (actual_output == expected_output)
            {
                total_passed++;
                result.output += "Test PASSED\n";
            }
            else {
                total_failed++;
                result.failed_tests.push_back(test_name);
                result.output += "Test FAILED\n";
            }

            result.output += "\n";
        }

        result.failed = total_failed;
        result.passed = total_passed;
        return result;
    }
}

std::string run_all_tests()
{
    TestResult total_result;

    // Run pure strategy tests
    auto pure_result = run_calculate_pure_nash_equilibrium_tests();
    total_result.output += "PURE STRATEGY TESTS\n";
    total_result.output += pure_result.output;
    total_result.passed += pure_result.passed;
    total_result.failed += pure_result.failed;
    total_result.failed_tests.insert(
        total_result.failed_tests.end(),
        pure_result.failed_tests.begin(),
        pure_result.failed_tests.end()
    );

    total_result.output += "\n===============================================================\n";

    // Run mixed strategy tests
    auto mixed_result = run_calculate_mixed_nash_equilibrium_tests();
    total_result.output += "MIXED STRATEGY TESTS\n";
    total_result.output += mixed_result.output;
    total_result.passed += mixed_result.passed;
    total_result.failed += mixed_result.failed;
    total_result.failed_tests.insert(
        total_result.failed_tests.end(),
        mixed_result.failed_tests.begin(),
        mixed_result.failed_tests.end()
    );

    // Add summary
    std::ostringstream oss;
    oss << "===============================================================\n";
    oss << "SUMMARY OF TESTS\n";
    oss << "===============================================================\n";
    oss << "Total Tests Run: " << (total_result.passed + total_result.failed) << "\n";
    oss << "Passed: " << total_result.passed << "\n";
    oss << "Failed: " << total_result.failed << "\n";

    if (!total_result.failed_tests.empty())
    {
        oss << "\nFailed Tests:\n";
        for (const auto& test_name : total_result.failed_tests)
        {
            oss << "- " << test_name << "\n";
        }
    }

    oss << "===============================================================\n";

    total_result.output += oss.str();

    return total_result.output;
}
