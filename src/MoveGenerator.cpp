#ifndef MOVE_GENERATOR_CPP // GUARD
#define MOVE_GENERATOR_CPP // GUARD

#include "MoveGenerator.hpp"

MoveGenerator::MoveGenerator() {}

std::vector<Move> MoveGenerator::generate_pseudo_legal_moves(Board &board, Color color) {
  std::vector<Move> pseudo_legal_moves;
  add_pseudo_legal_pawn_moves(board, color, pseudo_legal_moves);
  add_pseudo_legal_piece_moves(board, color, KNIGHT, pseudo_legal_moves);
  add_pseudo_legal_piece_moves(board, color, BISHOP, pseudo_legal_moves);
  add_pseudo_legal_piece_moves(board, color, ROOK, pseudo_legal_moves);
  add_pseudo_legal_piece_moves(board, color, QUEEN, pseudo_legal_moves);
  add_pseudo_legal_piece_moves(board, color, KING, pseudo_legal_moves);
  return pseudo_legal_moves;
}

// Needs update to consider promotion moves
void MoveGenerator::add_pseudo_legal_pawn_moves(Board &board, Color color, std::vector<Move> &moves) {
  bitboard promotion_file = color == WHITE ? RANK_8 : RANK_1;
  
  bitboard moving_pawns = board.get_piece_positions(PAWN, color);
  bitboard other_pieces = board.get_all_piece_positions(color);
  bitboard opposing_pieces = board.get_all_piece_positions(negate_color(color));
  bitboard current_position, capture_squares, normal_capture_squares, promotion_capture_squares, single_push_squares, quiet_single_push_squares, promotion_push_squares, double_push_squares;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_position = moving_pawns & mask;
    if(current_position) {
      // Get single push destinations
      single_push_squares = board.get_pawn_single_push(current_position, color) & ~opposing_pieces & ~other_pieces;
      // Add quiet single push moves
      quiet_single_push_squares = single_push_squares & ~promotion_file;
      add_moves(current_position, quiet_single_push_squares, 0, moves);
      // Add promotion single push moves
      promotion_push_squares = single_push_squares & promotion_file;
      add_promotion_moves(current_position, promotion_push_squares, false, moves);

      // Add double push moves
      double_push_squares = board.get_pawn_double_push(current_position, color) & ~opposing_pieces & ~other_pieces;
      add_moves(current_position, double_push_squares, 1, moves);

      // Get capture destinations
      capture_squares = board.get_piece_attacks(PAWN, current_position, color) & opposing_pieces;
      // Add normal capture moves
      normal_capture_squares = capture_squares & ~promotion_file;
      add_moves(current_position, normal_capture_squares, 4, moves);
      // Add promotion capture moves
      promotion_capture_squares = capture_squares & promotion_file;
      add_promotion_moves(current_position, promotion_capture_squares, true, moves);
    }
  }
}

void MoveGenerator::add_pseudo_legal_piece_moves(Board &board, Color color, Piece piece, std::vector<Move> &moves) {
  bitboard moving_pieces = board.get_piece_positions(piece, color);
  bitboard other_pieces = board.get_all_piece_positions(color);
  bitboard opposing_pieces = board.get_all_piece_positions(negate_color(color));

  bitboard current_position, destination_squares, quiet_squares, capture_squares;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_position = moving_pieces & mask;
    if(current_position) {
      destination_squares = board.get_piece_attacks(piece, current_position, color) & ~other_pieces;
      quiet_squares = destination_squares & ~opposing_pieces;
      add_moves(current_position, quiet_squares, 0, moves);
      capture_squares = destination_squares & opposing_pieces;
      add_moves(current_position, quiet_squares, 4, moves);
    }
  }
}

void MoveGenerator::add_pseudo_legal_en_passant_moves(Board &board, Color color, std::vector<Move> &moves) {
  Color other_color = negate_color(color);
  
  if(board.is_moves_empty(other_color)) return;

  Move last_move = board.get_last_move(other_color);
  if(!last_move.is_double_pawn_push()) return;

  bitboard vulnerable_pawn = last_move.get_destination();
  bitboard attack_pawns = board.get_piece_positions(PAWN, color) & (west(vulnerable_pawn) | east(vulnerable_pawn));
  
  bitboard destination_square;
  if(color == WHITE) {
    destination_square = north(vulnerable_pawn);
  } else {
    destination_square = south(vulnerable_pawn);
  }

  bitboard origin_square;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    origin_square = attack_pawns & mask;
    if(origin_square) {
      Move move(origin_square, destination_square, 5);
      moves.push_back(move);
    }
  }
}

void MoveGenerator::add_legal_castle_moves(Board &board, Color color, std::vector<Move> &moves) {
  add_legal_kingside_castle_move(board, color, moves);
  add_legal_queenside_castle_move(board, color, moves);
}

void MoveGenerator::add_legal_kingside_castle_move(Board &board, Color color, std::vector<Move> &moves) {
  // if color has no castle rights, don't add move
  if(!board.get_can_castle_king(color)) return;
  
  bitboard king_position = (color == WHITE ? 0x8 : 0x800000000000000);
  bitboard king_side_rook_position = (color == WHITE ? 0x1 : 0x100000000000000);
  bitboard king_side_castle_path = (color == WHITE ? WHITE_KINGSIDE_CASTLE_PATH : BLACK_KINGSIDE_CASTLE_PATH);
  
  // if castle is blocked, don't add move
  bitboard other_pieces = board.get_all_piece_positions(color) & ~king_position & ~king_side_rook_position;
  if(king_side_castle_path & other_pieces) return;

  // if castle path is attacked, don't add move
  bitboard current_position;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_position = king_side_castle_path & mask;
    if(current_position && board.is_position_attacked_by(current_position, negate_color(color))) return;
  }

  // otherwise, add move
  Move king_side_castle_move(king_position, king_side_rook_position, 2);
  moves.push_back(king_side_castle_move);
}

void MoveGenerator::add_legal_queenside_castle_move(Board &board, Color color, std::vector<Move> &moves) {
  // if color has no castle rights, don't add move
  if(!board.get_can_castle_queen(color)) return;
  
  bitboard king_position = (color == WHITE ? 0x8 : 0x800000000000000);
  bitboard queen_side_rook_position = (color == WHITE ? 0x80 : 0x8000000000000000);
  bitboard queen_side_castle_path = (color == WHITE ? WHITE_QUEENSIDE_CASTLE_PATH : BLACK_QUEENSIDE_CASTLE_PATH);
  
  // if castle is blocked, don't add move
  bitboard other_pieces = board.get_all_piece_positions(color) & ~king_position & ~queen_side_rook_position;
  if(queen_side_castle_path & other_pieces) return;

  // if castle path is attacked, don't add move
  bitboard current_position;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_position = queen_side_castle_path & mask;
    if(current_position && board.is_position_attacked_by(current_position, negate_color(color))) return;
  }

  // otherwise, add move
  Move queen_side_castle_move(king_position, queen_side_rook_position, 2);
  moves.push_back(queen_side_castle_move);
}

void MoveGenerator::add_moves(bitboard origin, bitboard all_destinations, char flag, std::vector<Move> &moves) {
  bitboard current_destination;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_destination = all_destinations & mask;
    if(current_destination) {
      Move move(origin, current_destination, flag);
      moves.push_back(move);
    }
  }
}

void MoveGenerator::add_promotion_moves(bitboard origin, bitboard all_destinations, bool capture, std::vector<Move> &moves) {
  int start_flag = capture ? 12 : 8;
  bitboard current_destination;
  for(bitboard mask = 1; mask > 0; mask <<= 1) {
    current_destination = all_destinations & mask;
    if(current_destination) {
      Move move(origin, current_destination, 0);
      moves.push_back(move);
      for(int i = 0; i < 4; i++) {
	Move move(origin, current_destination, start_flag + i);
	moves.push_back(move);
      }
    }
  }
}

#endif
