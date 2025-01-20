/*
 * Search class.
 */

#ifndef SEARCH_HPP // GUARD
#define SEARCH_HPP // GUARD

#include "Board.hpp"
#include "Evaluation.hpp"
#include "MoveGenerator.hpp"
#include "MoveList.hpp"
#include "PVTable.hpp"
#include "TTable.hpp"

class Search {
public:
  // Constructor:
  Search();

  // Getters:
  Move get_best_move();

  // Root:
  int alpha_beta_max_root(int alpha, int beta, int depth_left, Board &board,
                          MoveGenerator &move_gen, Evaluation &eval);
  int alpha_beta_max_root(int alpha, int beta, int depth_left, Board &board,
                          MoveGenerator &move_gen, Evaluation &eval,
                          unsigned time_limit);

  // Alpha beta:
  int alpha_beta_max(int alpha, int beta, int depth_left, Board &board,
                     MoveGenerator &move_gen, Evaluation &eval);
  int alpha_beta_min(int alpha, int beta, int depth_left, Board &board,
                     MoveGenerator &move_gen, Evaluation &eval);

  // Negamax:
  int negamax(int depth, Board &board, MoveGenerator &move_gen,
              Evaluation &eval);
  int negamax_root(int depth, Board &board, MoveGenerator &move_gen,
                   Evaluation &eval);

  // Iterative Deepening:
  int negamax_id(int depth, int alpha, int beta, Board &board,
                 MoveGenerator &move_gen, Evaluation &eval);
  int negamax_root_iterative_deepening(unsigned time_limit, Board &board,
                                       MoveGenerator &move_gen,
                                       Evaluation &eval);
  int quiescence_search(int alpha, int beta, Board &board,
                        MoveGenerator &move_gen, Evaluation &eval);
  // TODO implement iterative deepening with time management
private:
  unsigned current_ply;
  unsigned nodes_evaluated;

  Move best_move;
  TTable t_table;
  PVTable pv_table;

  // Move Ordering
  static const uint8_t victim_aggressor_values[6][6]; // victim_aggressor_values[victim][attacker]
  void sort_moves(MoveList& moves, Move& pv_move, Move& tt_move, Board& board);
};

#endif // GUARD
