#include <vector>

#include "nashResolver.h"

#include <algorithm>
#include <string>
#include <sstream>
#include <cmath>
#include <iomanip>
#include <utility> // Required for std::pair, though not directly used in the provided snippet, good for general C++ utilities

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
        // Ensure p1 and p2 are not empty and s1*s2 matches their size
        if (s1 == 0 || s2 == 0) {
            // Or handle error appropriately
            modified_p1.clear();
            modified_p2.clear();
            return;
        }
        
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
            double max_val = p1[j]; // p1[0*s2 + j]
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
            double max_val = p2[i * s2]; // p2[i*s2 + 0]
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
        double m_bai = 0; // Renamed M to m_bai to avoid conflict if this is in the same scope as another M
        std::vector<double> modified_p1, modified_p2;

        s_x_bai_algorithm(p1, p2, s1, s2, modified_p1, modified_p2, m_bai);

        std::vector<pure_nash_equilibrium> pure_nash_equilibria; // Renamed for clarity

        // Find positions where both modified_p1 and modified_p2 are greater than -m_bai
        for (size_t i = 0; i < s1; ++i)
        {
            for (size_t j = 0; j < s2; ++j)
            {
                if (modified_p1[i * s2 + j] > -m_bai && modified_p2[i * s2 + j] > -m_bai)
                {
                    pure_nash_equilibria.push_back({ // Corrected struct name
                        i,  // Player 1's strategy index
                        j,  // Player 2's strategy index
                        p1[i * s2 + j], // Player 1's payoff
                        p2[i * s2 + j]  // Player 2's payoff
                        });
                }
            }
        }
        return pure_nash_equilibria;
    }

    std::string format_double(const double value)
    {
        if (std::floor(value) == value)
        {
            return std::to_string(static_cast<int>(value));
        }
        else
        {
            std::ostringstream oss;
            oss << value; // Default precision
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
            for (const auto& eq : equilibrium) // Changed variable name for clarity
            {
                oss << "Player 1 chooses strategy " << eq.player1_strategy
                    << ", Player 2 chooses strategy " << eq.player2_strategy << "\n";
                oss << "Payoffs: (" << format_double(eq.player1_payoff) << ", "
                    << format_double(eq.player2_payoff) << ")\n";
            }
        }

        oss << "=====================\n";
        return oss.str();
    }

    mixed_nash_equilibrium _calculate_mixed_nash_equilibrium(const std::vector<double>& p1, const std::vector<double>& p2,
        const size_t s1, const size_t s2)
    {
        std::vector<bool> row_dominated(s1, false);
        std::vector<bool> col_dominated(s2, false);
        size_t active_s1 = s1;
        size_t active_s2 = s2;

        bool changed_in_iteration = true; 

        while (changed_in_iteration && active_s1 > 0 && active_s2 > 0)
        {
            changed_in_iteration = false; 

            // Check for dominated rows
            for (size_t i = 0; i < s1; ++i) {
                if (row_dominated[i]) continue;

                for (size_t k = 0; k < s1; ++k) {
                    if (i == k || row_dominated[k]) continue;

                    bool p1_row_i_strictly_dominated_by_k = true;
                    bool p2_row_i_strictly_dominated_by_k = true;
                    int S2_active_count_for_comparison = 0;

                    for (size_t j_col = 0; j_col < s2; ++j_col) {
                        if (col_dominated[j_col]) continue;
                        S2_active_count_for_comparison++;
                        if (p1[i * s2 + j_col] >= p1[k * s2 + j_col]) p1_row_i_strictly_dominated_by_k = false;
                        if (p2[i * s2 + j_col] >= p2[k * s2 + j_col]) p2_row_i_strictly_dominated_by_k = false;
                        if (!p1_row_i_strictly_dominated_by_k && !p2_row_i_strictly_dominated_by_k) break; 
                    }
                    
                    if (S2_active_count_for_comparison == 0 && active_s2 > 0) { // Check active_s2 to ensure some columns were initially available
                        p1_row_i_strictly_dominated_by_k = false;
                        p2_row_i_strictly_dominated_by_k = false;
                    } else if (active_s2 == 0) { // No columns left at all
                         p1_row_i_strictly_dominated_by_k = false;
                         p2_row_i_strictly_dominated_by_k = false;
                    }


                    if (p1_row_i_strictly_dominated_by_k || p2_row_i_strictly_dominated_by_k) {
                        row_dominated[i] = true;
                        active_s1--;
                        changed_in_iteration = true;
                        break; 
                    }
                }
                if (changed_in_iteration && row_dominated[i]) {
                    // A row was marked, restart row checks fully as this could enable further dominations.
                    // This break targets the outer 'i' loop for rows.
                    break; 
                }
            }

            if (changed_in_iteration) continue; // Restart while loop if a row was dominated

            // Check for dominated columns
            for (size_t j = 0; j < s2; ++j) {
                if (col_dominated[j]) continue;

                for (size_t k = 0; k < s2; ++k) {
                    if (j == k || col_dominated[k]) continue;

                    bool p1_col_j_strictly_dominated_by_k = true;
                    bool p2_col_j_strictly_dominated_by_k = true;
                    int S1_active_count_for_comparison = 0;

                    for (size_t i_row = 0; i_row < s1; ++i_row) {
                        if (row_dominated[i_row]) continue;
                        S1_active_count_for_comparison++;
                        if (p1[i_row * s2 + j] >= p1[i_row * s2 + k]) p1_col_j_strictly_dominated_by_k = false;
                        if (p2[i_row * s2 + j] >= p2[i_row * s2 + k]) p2_col_j_strictly_dominated_by_k = false;
                        if (!p1_col_j_strictly_dominated_by_k && !p2_col_j_strictly_dominated_by_k) break;
                    }

                    if (S1_active_count_for_comparison == 0 && active_s1 > 0) {
                        p1_col_j_strictly_dominated_by_k = false;
                        p2_col_j_strictly_dominated_by_k = false;
                    } else if (active_s1 == 0) {
                         p1_col_j_strictly_dominated_by_k = false;
                         p2_col_j_strictly_dominated_by_k = false;
                    }

                    if (p1_col_j_strictly_dominated_by_k || p2_col_j_strictly_dominated_by_k) {
                        col_dominated[j] = true;
                        active_s2--;
                        changed_in_iteration = true;
                        break; 
                    }
                }
                if (changed_in_iteration && col_dominated[j]) {
                    // A column was marked, restart column checks fully.
                    break;
                }
            }
            // If a column was dominated, 'changed_in_iteration' is true, so the while loop continues,
            // which will restart with row checks.
        }

        mixed_nash_equilibrium result;
        result.player1_strategy_probabilities.assign(s1, 0.0);
        result.player2_strategy_probabilities.assign(s2, 0.0);

        if (active_s1 == 0 || active_s2 == 0) {
            if (active_s1 > 0) { // P1 has strategies, P2 does not
                double prob = 1.0 / active_s1;
                for(size_t i_orig = 0; i_orig < s1; ++i_orig) {
                    if (!row_dominated[i_orig]) result.player1_strategy_probabilities[i_orig] = prob;
                }
            }
            if (active_s2 > 0) { // P2 has strategies, P1 does not
                double prob = 1.0 / active_s2;
                for(size_t j_orig = 0; j_orig < s2; ++j_orig) {
                    if (!col_dominated[j_orig]) result.player2_strategy_probabilities[j_orig] = prob;
                }
            }
            return result; // One player has no strategies, or both.
        }
        
        std::vector<double> active_p1_matrix(active_s1 * active_s2);
        std::vector<double> active_p2_matrix(active_s1 * active_s2);
        std::vector<size_t> p1_active_to_original_map(active_s1);
        std::vector<size_t> p2_active_to_original_map(active_s2);

        size_t current_active_row = 0;
        for(size_t r_orig = 0; r_orig < s1; ++r_orig) {
            if (!row_dominated[r_orig]) {
                p1_active_to_original_map[current_active_row] = r_orig;
                size_t current_active_col = 0;
                for (size_t c_orig = 0; c_orig < s2; ++c_orig) {
                    if (!col_dominated[c_orig]) {
                        active_p1_matrix[current_active_row * active_s2 + current_active_col] = p1[r_orig * s2 + c_orig];
                        active_p2_matrix[current_active_row * active_s2 + current_active_col] = p2[r_orig * s2 + c_orig];
                        current_active_col++;
                    }
                }
                current_active_row++;
            }
        }
        size_t current_active_col = 0;
        for(size_t c_orig = 0; c_orig < s2; ++c_orig) {
            if(!col_dominated[c_orig]) {
                p2_active_to_original_map[current_active_col++] = c_orig;
            }
        }

        double M_bai{ 0 }; // Renamed M for Bai algorithm
        std::vector<double> bai_active_p1_output(active_s1 * active_s2);
        std::vector<double> bai_active_p2_output(active_s1 * active_s2);

        s_x_bai_algorithm(active_p1_matrix, active_p2_matrix, active_s1, active_s2, bai_active_p1_output, bai_active_p2_output, M_bai);

        double sum_p1_active_probs{ 0 };
        for (size_t i_active = 0; i_active < active_s1; ++i_active) {
            for (size_t j_active = 0; j_active < active_s2; ++j_active) { 
                if (bai_active_p1_output[i_active * active_s2 + j_active] > -M_bai) {
                    sum_p1_active_probs += bai_active_p1_output[i_active * active_s2 + j_active];
                }
            }
        }
        
        if (sum_p1_active_probs == 0) { 
            double equal_prob = active_s1 > 0 ? 1.0 / active_s1 : 0;
            for (size_t i_active = 0; i_active < active_s1; ++i_active) {
                result.player1_strategy_probabilities[p1_active_to_original_map[i_active]] = equal_prob;
            }
        } else {
            for (size_t i_active = 0; i_active < active_s1; ++i_active) {
                double current_active_row_sum{ 0 };
                for (size_t j_active = 0; j_active < active_s2; ++j_active) {
                    if (bai_active_p1_output[i_active * active_s2 + j_active] > -M_bai) {
                         current_active_row_sum += bai_active_p1_output[i_active * active_s2 + j_active];
                    }
                }
                result.player1_strategy_probabilities[p1_active_to_original_map[i_active]] = current_active_row_sum / sum_p1_active_probs;
            }
        }

        double sum_p2_active_probs{ 0 };
        for (size_t j_active = 0; j_active < active_s2; ++j_active) { // Iterate active columns first for P2
            for (size_t i_active = 0; i_active < active_s1; ++i_active) { // Then active rows
                 // Accessing bai_active_p2_output: row_major (i_active * active_s2 + j_active)
                 if (bai_active_p2_output[i_active * active_s2 + j_active] > -M_bai) { 
                    sum_p2_active_probs += bai_active_p2_output[i_active * active_s2 + j_active];
                }
            }
        }

        if (sum_p2_active_probs == 0) {
            double equal_prob = active_s2 > 0 ? 1.0 / active_s2 : 0;
            for (size_t j_active = 0; j_active < active_s2; ++j_active) {
                result.player2_strategy_probabilities[p2_active_to_original_map[j_active]] = equal_prob;
            }
        } else {
            for (size_t j_active = 0; j_active < active_s2; ++j_active) {
                double current_active_col_sum{ 0 };
                for (size_t i_active = 0; i_active < active_s1; ++i_active) {
                    if (bai_active_p2_output[i_active * active_s2 + j_active] > -M_bai) {
                        current_active_col_sum += bai_active_p2_output[i_active * active_s2 + j_active];
                    }
                }
                result.player2_strategy_probabilities[p2_active_to_original_map[j_active]] = current_active_col_sum / sum_p2_active_probs;
            }
        }
        
        // Normalize probabilities
        double final_sum_p1 = 0;
        for(size_t i=0; i<s1; ++i) final_sum_p1 += result.player1_strategy_probabilities[i];
        if(final_sum_p1 > 1e-9){ 
            for(size_t i=0; i<s1; ++i) result.player1_strategy_probabilities[i] /= final_sum_p1;
        }

        double final_sum_p2 = 0;
        for(size_t i=0; i<s2; ++i) final_sum_p2 += result.player2_strategy_probabilities[i];
        if(final_sum_p2 > 1e-9){
            for(size_t i=0; i<s2; ++i) result.player2_strategy_probabilities[i] /= final_sum_p2;
        }

        return result;
    }

    std::string format_mixed_nash_equilibrium(const mixed_nash_equilibrium& equilibrium)
    {
        std::ostringstream oss;
        oss << "=====================\n";
        oss << "Mixed Strategy Equilibrium:\n";

        oss << "Player 1 (Horizontal):\n";
        bool p1_has_strategy = false;
        for (size_t i = 0; i < equilibrium.player1_strategy_probabilities.size(); ++i) {
            if (equilibrium.player1_strategy_probabilities[i] > 1e-9) p1_has_strategy = true;
            oss << "Strategy " << i << " = " << std::fixed << std::setprecision(2)
                << equilibrium.player1_strategy_probabilities[i] << "\n";
        }
        // The lines below were removed as active_s1 is not in scope here.
        // if (!p1_has_strategy && !equilibrium.player1_strategy_probabilities.empty() && active_s1 == 0) {
        //    // oss << "No non-dominated strategies for Player 1.\n"; 
        // }


        oss << "Player 2 (Vertical):\n";
        bool p2_has_strategy = false;
        for (size_t i = 0; i < equilibrium.player2_strategy_probabilities.size(); ++i) {
             if (equilibrium.player2_strategy_probabilities[i] > 1e-9) p2_has_strategy = true;
            oss << "Strategy " << i << " = " << std::fixed << std::setprecision(2)
                << equilibrium.player2_strategy_probabilities[i] << "\n";
        }
        // The lines below were removed as active_s2 is not in scope here.
        // if (!p2_has_strategy && !equilibrium.player2_strategy_probabilities.empty() && active_s2 == 0) {
        //    // oss << "No non-dominated strategies for Player 2.\n";
        // }


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
    // Basic validation for empty inputs, though s_x_bai_algorithm also has checks
    if (s1 == 0 || s2 == 0) {
        mixed_nash_equilibrium empty_result;
        if (s1 > 0) empty_result.player1_strategy_probabilities.assign(s1, 0.0);
        if (s2 > 0) empty_result.player2_strategy_probabilities.assign(s2, 0.0);
        // Or, if s1=0 and p1 is empty, the assign might be problematic.
        // The format function should gracefully handle empty probability vectors.
        return format_mixed_nash_equilibrium(empty_result);
    }
    return format_mixed_nash_equilibrium(_calculate_mixed_nash_equilibrium(p1, p2, s1, s2));
}

namespace {
    TestResult run_calculate_pure_nash_equilibrium_tests()
    {
        TestResult result;
        int total_passed = 0;
        int total_failed = 0;

        auto local_format_double = [](double value) -> std::string // Renamed to avoid conflict
            {
                if (std::floor(value) == value)
                {
                    return std::to_string(static_cast<int>(value));
                }
                std::ostringstream oss;
                oss << value;
                return oss.str();
            };

        auto format_payoff_matrix = [&](const std::vector<double>& p1, const std::vector<double>& p2, size_t s1, size_t s2) -> std::string
            {
                std::ostringstream oss;
                oss << "Payoff Matrix:\n";
                oss << "Format: (Player1 payoff, Player2 payoff)\n\n";
                oss << std::setw(20) << "";
                for (size_t j = 0; j < s2; ++j) oss << std::setw(20) << "Player2 Strategy " + std::to_string(j);
                oss << "\n";
                for (size_t i = 0; i < s1; ++i) {
                    oss << std::setw(20) << "Player1 Strategy " + std::to_string(i);
                    for (size_t j = 0; j < s2; ++j) {
                        oss << std::setw(20) << "(" + local_format_double(p1[i * s2 + j]) + ", " + local_format_double(p2[i * s2 + j]) + ")";
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
            std::vector<double> p1_matrix = { 3, 0, 5, 1 }; // Renamed to avoid conflict
            std::vector<double> p2_matrix = { 3, 5, 0, 1 }; // Renamed
            size_t s1_val = 2, s2_val = 2; // Renamed
            result.output += format_payoff_matrix(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (1, 1)\n"
                "=====================\n";
            std::string actual_output = calculate_pure_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }

        // Battle of the Sexes
        {
            std::string test_name = "Battle of the Sexes Test";
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 2, 0, 0, 1 }; std::vector<double> p2_matrix = { 1, 0, 0, 2 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 0\n"
                "Payoffs: (2, 1)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (1, 2)\n"
                "=====================\n";
            std::string actual_output = calculate_pure_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }

        // Matching Pennies
        {
            std::string test_name = "Matching Pennies Test";
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 1, -1, -1, 1 }; std::vector<double> p2_matrix = { -1, 1, 1, -1 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "No Nash equilibrium in pure strategies.\n"
                "=====================\n";
            std::string actual_output = calculate_pure_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }
         // Stag Hunt
        {
            std::string test_name = "Stag Hunt Test";
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 4, 1, 1, 2 }; std::vector<double> p2_matrix = { 4, 1, 1, 2 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 0\n"
                "Payoffs: (4, 4)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 1\n"
                "Payoffs: (2, 2)\n"
                "=====================\n";
            std::string actual_output = calculate_pure_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
             result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }

        // Hawk-Dove Game
        {
            std::string test_name = "Hawk-Dove Test"; // Corrected name
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 0, 7, 2, 2 }; std::vector<double> p2_matrix = { 0, 2, 7, 2 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Pure Strategy Equilibrium:\n"
                "Player 1 chooses strategy 0, Player 2 chooses strategy 1\n"
                "Payoffs: (7, 2)\n"
                "Player 1 chooses strategy 1, Player 2 chooses strategy 0\n"
                "Payoffs: (2, 7)\n"
                "=====================\n";
            std::string actual_output = calculate_pure_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
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

        auto local_format_double_mixed = [](double value) -> std::string // Renamed
            {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << value;
                return oss.str();
            };

        auto format_payoff_matrix_mixed = [&](const std::vector<double>& p1, const std::vector<double>& p2, size_t s1, size_t s2) -> std::string // Renamed
            {
                std::ostringstream oss;
                oss << "Payoff Matrix:\n";
                oss << "Format: (Player1 payoff, Player2 payoff)\n\n";
                oss << std::setw(20) << "";
                for (size_t j = 0; j < s2; ++j) oss << std::setw(20) << "Player2 Strategy " + std::to_string(j);
                oss << "\n";
                for (size_t i = 0; i < s1; ++i) {
                    oss << std::setw(20) << "Player1 Strategy " + std::to_string(i);
                    for (size_t j = 0; j < s2; ++j) {
                         oss << std::setw(20) << "(" + local_format_double_mixed(p1[i * s2 + j]) + ", " + local_format_double_mixed(p2[i * s2 + j]) + ")";
                    }
                    oss << "\n";
                }
                oss << "\n";
                return oss.str();
            };

        // Matching Pennies Test
        {
            std::string test_name = "Mixed Matching Pennies Test"; // Differentiated name
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 1, -1, -1, 1 }; std::vector<double> p2_matrix = { -1, 1, 1, -1 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix_mixed(p1_matrix, p2_matrix, s1_val, s2_val);
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
            std::string actual_output = calculate_mixed_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }

        // Rock-Paper-Scissors Test
        {
            std::string test_name = "Mixed Rock-Paper-Scissors Test"; // Differentiated
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 0, -1, 1, 1, 0, -1, -1, 1, 0 }; 
            std::vector<double> p2_matrix = { 0, 1, -1, -1, 0, 1, 1, -1, 0 };
            size_t s1_val = 3, s2_val = 3;
            result.output += format_payoff_matrix_mixed(p1_matrix, p2_matrix, s1_val, s2_val);
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
            std::string actual_output = calculate_mixed_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }

        // Battle of the Sexes Test (Mixed)
        {
            std::string test_name = "Mixed Battle of the Sexes Test"; // Differentiated
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 2, 0, 0, 1 }; std::vector<double> p2_matrix = { 1, 0, 0, 2 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix_mixed(p1_matrix, p2_matrix, s1_val, s2_val);
            std::string expected_output =
                "=====================\n"
                "Mixed Strategy Equilibrium:\n"
                "Player 1 (Horizontal):\n"
                "Strategy 0 = 0.67\n" // P1 plays Ballet with 2/3
                "Strategy 1 = 0.33\n" // P1 plays Football with 1/3
                "Player 2 (Vertical):\n"
                "Strategy 0 = 0.33\n" // P2 plays Ballet with 1/3
                "Strategy 1 = 0.67\n" // P2 plays Football with 2/3
                "=====================\n";
            std::string actual_output = calculate_mixed_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
            result.output += "\n";
        }
        
        // Zero Payoff Test (Mixed)
        {
            std::string test_name = "Mixed Zero Payoff Test";
            result.output += test_name + "\n";
            std::vector<double> p1_matrix = { 0, 0, 0, 0 }; std::vector<double> p2_matrix = { 0, 0, 0, 0 };
            size_t s1_val = 2, s2_val = 2;
            result.output += format_payoff_matrix_mixed(p1_matrix, p2_matrix, s1_val, s2_val);
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
            std::string actual_output = calculate_mixed_nash_equilibrium(p1_matrix, p2_matrix, s1_val, s2_val);
            result.output += "Expected Output:\n" + expected_output + "Actual Output:\n" + actual_output;
            if (actual_output == expected_output) { total_passed++; result.output += "Test PASSED\n"; }
            else { total_failed++; result.failed_tests.push_back(test_name); result.output += "Test FAILED\n"; }
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

    auto pure_result = run_calculate_pure_nash_equilibrium_tests();
    total_result.output += "PURE STRATEGY TESTS\n";
    total_result.output += pure_result.output;
    total_result.passed += pure_result.passed;
    total_result.failed += pure_result.failed;
    total_result.failed_tests.insert(total_result.failed_tests.end(), pure_result.failed_tests.begin(), pure_result.failed_tests.end());

    total_result.output += "\n===============================================================\n";

    auto mixed_result = run_calculate_mixed_nash_equilibrium_tests();
    total_result.output += "MIXED STRATEGY TESTS\n";
    total_result.output += mixed_result.output;
    total_result.passed += mixed_result.passed;
    total_result.failed += mixed_result.failed;
    total_result.failed_tests.insert(total_result.failed_tests.end(), mixed_result.failed_tests.begin(), mixed_result.failed_tests.end());

    std::ostringstream oss;
    oss << "===============================================================\n";
    oss << "SUMMARY OF TESTS\n";
    oss << "===============================================================\n";
    oss << "Total Tests Run: " << (total_result.passed + total_result.failed) << "\n";
    oss << "Passed: " << total_result.passed << "\n";
    oss << "Failed: " << total_result.failed << "\n";
    if (!total_result.failed_tests.empty()) {
        oss << "\nFailed Tests:\n";
        for (const auto& test_name : total_result.failed_tests) {
            oss << "- " << test_name << "\n";
        }
    }
    oss << "===============================================================\n";
    total_result.output += oss.str();

    return total_result.output;
}
