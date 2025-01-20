/*
 * Engine interface implementation.
 */

#include "Engine.hpp"
#include "MoveList.hpp"

#ifndef ENGINE_CPP // GUARD
#define ENGINE_CPP // GUARD

Engine::Engine() {
  white_increment = 0;
  black_increment = 0;
  time_divider = 50;
  time_set = false;
}

bool Engine::set_position(std::string fen_string) {
  return board.initialize_fen(fen_string);
}

bool Engine::play_move(std::string move_str) {
  MoveList pseudo_legal_moves =
      move_gen.generate_pseudo_legal_moves(board, board.get_turn_color());
  for (int i = 0; i < pseudo_legal_moves.size(); ++i) {
    if (move_str == pseudo_legal_moves[i].to_uci_notation()) {
      board.execute_move(pseudo_legal_moves[i]);
      if (board.is_checked(negate_color(board.get_turn_color()))) {
        board.undo_move(pseudo_legal_moves[i]);
        return false;
      }
      return true;
    }
  }
  return false;
}

void Engine::search_best_move() {
  unsigned search_time = get_time_for_move();
  search.negamax_root_iterative_deepening(search_time, board, move_gen, eval);
  Move move = search.get_best_move();
  std::cout << "bestmove " << move.to_uci_notation() << std::endl;
}

unsigned Engine::get_time_for_move() {
  if (time_set) {
    if (moves_to_go == 0) {
      if (board.get_turn_color() == WHITE) {
        return white_increment +
               (white_time - white_time / time_divider) / time_divider;
      } else {
        return black_increment +
               (black_time - black_time / time_divider) / time_divider;
      }
    } else {
      if (board.get_turn_color() == WHITE) {
        return white_increment + (white_time - white_time / (moves_to_go + 3)) /
                                     (0.7 * moves_to_go + 3);
      } else {
        return black_increment + (black_time - black_time / (moves_to_go + 3)) /
                                     (0.7 * moves_to_go + 3);
      }
    }
  } else {
    return 3000;
  }
}

#endif // GUARD
