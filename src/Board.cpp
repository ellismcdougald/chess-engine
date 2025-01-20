#ifndef BOARD_CPP // GUARD
#define BOARD_CPP // GUARD

#include "Board.hpp"
#include "Move.hpp"
#include "globals.hpp"
#include <assert.h>
#include <bit>
#include <bitset>
#include <random>
#include <sstream>
#include <string>

// Constructor:
Board::Board() {
  piece_bitboards[0].fill(0);
  piece_bitboards[1].fill(0);
  all_piece_bitboards.fill(0);
  turn_color = WHITE;
  initialize_lookups();
  init_zobrist_keys();
  castle_rights = 0xF;
  castle_rights_size = 0;
  for (int i = 0; i < 64; i++) {
    board_pieces[i] = NONE;
  }
  moves_size[0] = 0;
  moves_size[1] = 0;
  captured_pieces_size[0] = 0;
  captured_pieces_size[1] = 0;
  half_moves = 0;
  full_moves = 1;
  zkey = generate_zkey();
}

void Board::clear() {
  piece_bitboards[0].fill(0);
  piece_bitboards[1].fill(0);
  all_piece_bitboards.fill(0);
  turn_color = WHITE;
  zkey ^= side_zkey;
  castle_rights = 0xF;
  castle_rights_size = 0;
  for (int i = 0; i < 64; i++) {
    board_pieces[i] = NONE;
  }
  moves_size[0] = 0;
  moves_size[1] = 0;
  captured_pieces_size[0] = 0;
  captured_pieces_size[1] = 0;
  half_moves = 0;
  full_moves = 1;
  zkey = generate_zkey();
}

// Initializer:
void Board::initialize_board_starting_position() {
  piece_bitboards[WHITE][PAWN] = starting_white_pawn_position;
  piece_bitboards[WHITE][KNIGHT] = starting_white_knight_position;
  piece_bitboards[WHITE][BISHOP] = starting_white_bishop_position;
  piece_bitboards[WHITE][ROOK] = starting_white_rook_position;
  piece_bitboards[WHITE][QUEEN] = starting_white_queen_position;
  piece_bitboards[WHITE][KING] = starting_white_king_position;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[WHITE][ALL] |= piece_bitboards[WHITE][piece_index];
  }

  piece_bitboards[BLACK][PAWN] = starting_black_pawn_position;
  piece_bitboards[BLACK][KNIGHT] = starting_black_knight_position;
  piece_bitboards[BLACK][BISHOP] = starting_black_bishop_position;
  piece_bitboards[BLACK][ROOK] = starting_black_rook_position;
  piece_bitboards[BLACK][QUEEN] = starting_black_queen_position;
  piece_bitboards[BLACK][KING] = starting_black_king_position;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[BLACK][ALL] |= piece_bitboards[BLACK][piece_index];
  }

  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    all_piece_bitboards[piece_index] = piece_bitboards[WHITE][piece_index] |
                                       piece_bitboards[BLACK][piece_index];
  }

  board_pieces[0] = ROOK;
  board_pieces[1] = KNIGHT;
  board_pieces[2] = BISHOP;
  board_pieces[3] = KING;
  board_pieces[4] = QUEEN;
  board_pieces[5] = BISHOP;
  board_pieces[6] = KNIGHT;
  board_pieces[7] = ROOK;
  for (int i = 8; i < 16; i++) {
    board_pieces[i] = PAWN;
  }
  for (int i = 16; i < 48; i++) {
    board_pieces[i] = NONE;
  }
  for (int i = 48; i < 56; i++) {
    board_pieces[i] = PAWN;
  }
  board_pieces[56] = ROOK;
  board_pieces[57] = KNIGHT;
  board_pieces[58] = BISHOP;
  board_pieces[59] = KING;
  board_pieces[60] = QUEEN;
  board_pieces[61] = BISHOP;
  board_pieces[62] = KNIGHT;
  board_pieces[63] = ROOK;

  zkey = generate_zkey();
};

void Board::initialize_perft_position_2() {
  piece_bitboards[WHITE][PAWN] = 0x100800E700;
  piece_bitboards[WHITE][KNIGHT] = 0x800200000;
  piece_bitboards[WHITE][BISHOP] = 0x1800;
  piece_bitboards[WHITE][ROOK] = 0x81;
  piece_bitboards[WHITE][QUEEN] = 0x40000;
  piece_bitboards[WHITE][KING] = 0x8;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[WHITE][ALL] |= piece_bitboards[WHITE][piece_index];
  }

  piece_bitboards[BLACK][PAWN] = 0xB40A0040010000;
  piece_bitboards[BLACK][KNIGHT] = 0x440000000000;
  piece_bitboards[BLACK][BISHOP] = 0x2800000000000;
  piece_bitboards[BLACK][ROOK] = 0x8100000000000000;
  piece_bitboards[BLACK][QUEEN] = 0x8000000000000;
  piece_bitboards[BLACK][KING] = 0x800000000000000;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[BLACK][ALL] |= piece_bitboards[BLACK][piece_index];
  }

  for (int i = 0; i < 64; i++) {
    board_pieces[i] = NONE;
  }
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    all_piece_bitboards[piece_index] = piece_bitboards[WHITE][piece_index] |
                                       piece_bitboards[BLACK][piece_index];

    bitboard piece_bitboard = all_piece_bitboards[piece_index];
    while (piece_bitboard) {
      board_pieces[lsb(piece_bitboard)] = (Piece)piece_index;
      piece_bitboard &= piece_bitboard - 1;
    }
  }

  zkey = generate_zkey();
}

void Board::initialize_perft_position_3() {
  piece_bitboards[WHITE][PAWN] = 0x4000000A00;
  piece_bitboards[WHITE][ROOK] = 0x40000000;
  piece_bitboards[WHITE][KING] = 0x8000000000;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[WHITE][ALL] |= piece_bitboards[WHITE][piece_index];
  }

  piece_bitboards[BLACK][PAWN] = 0x20100004000000;
  piece_bitboards[BLACK][ROOK] = 0x100000000;
  piece_bitboards[BLACK][KING] = 0x1000000;
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    piece_bitboards[BLACK][ALL] |= piece_bitboards[BLACK][piece_index];
  }

  for (int i = 0; i < 64; i++) {
    board_pieces[i] = NONE;
  }
  for (int piece_index = 0; piece_index < 6; ++piece_index) {
    all_piece_bitboards[piece_index] = piece_bitboards[WHITE][piece_index] |
                                       piece_bitboards[BLACK][piece_index];

    bitboard piece_bitboard = all_piece_bitboards[piece_index];
    while (piece_bitboard) {
      board_pieces[lsb(piece_bitboard)] = (Piece)piece_index;
      piece_bitboard &= piece_bitboard - 1;
    }
  }

  zkey = generate_zkey();
}

bool Board::initialize_fen(std::string fen) {
  clear();

  bitboard current_position = 0x8000000000000000;
  std::istringstream fen_ss(fen);

  std::string pieces_str;
  fen_ss >> pieces_str;
  Piece piece_to_place;
  Color piece_color;
  int numSpaces;
  for (int i = 0; i < pieces_str.length(); i++) {
    if (pieces_str[i] == '/') {
      continue;
    } else if (isdigit(pieces_str[i])) {
      for (int j = 0; j < pieces_str[i] - '0'; j++) {
        current_position >>= 1;
      }
    } else {
      piece_color = isupper(pieces_str[i]) ? WHITE : BLACK;
      piece_to_place = get_piece_from_char(pieces_str[i]);
      set_piece(piece_to_place, piece_color, current_position);
      current_position >>= 1;
    }
  }

  std::string color_str;
  fen_ss >> color_str;
  if (!(color_str == "w" | color_str == "b")) {
    std::cout << "Invalid fen." << std::endl;
    return false;
  }
  set_turn_color(color_str == "w" ? WHITE : BLACK);

  castle_rights = 0; // clear castle rights
  std::string castle_str;
  fen_ss >> castle_str;
  for (int i = 0; i < castle_str.length(); ++i) {
    if (castle_str[i] == 'K') {
      set_king_castle_right(WHITE);
    } else if (castle_str[i] == 'Q') {
      set_queen_castle_right(WHITE);
    } else if (castle_str[i] == 'k') {
      set_king_castle_right(BLACK);
    } else {
      set_queen_castle_right(BLACK);
    }
  }

  // En passant
  // Add a double pawn push move corresponding to the given en passant square
  std::string ep_str;
  fen_ss >> ep_str;
  if (ep_str != "-") {
    Color move_color = negate_color(turn_color);
    bitboard ep_square = position_string_to_bitboard(ep_str);
    bitboard above = north(ep_square);
    bitboard below = south(ep_square);
    if (move_color == WHITE) {
      Move move(below, above, 1);
      moves[move_color][moves_size[move_color]++] = move;
    } else {
      Move move(above, below, 1);
      moves[move_color][moves_size[move_color]++] = move;
    }
  }

  fen_ss >> half_moves;

  fen_ss >> full_moves;

  zkey = generate_zkey();

  return true;
}

Piece Board::get_piece_from_char(char piece_char) {
  switch (tolower(piece_char)) {
  case 'p':
    return PAWN;
  case 'n':
    return KNIGHT;
  case 'b':
    return BISHOP;
  case 'r':
    return ROOK;
  case 'q':
    return QUEEN;
  case 'k':
    return KING;
  default:
    return NONE;
  }
}

bitboard Board::get_end_edge_mask(Direction direction) {
  switch (direction) {
  case NORTH:
    return RANK_8;
  case SOUTH:
    return RANK_1;
  case EAST:
    return FILE_H;
  case WEST:
    return FILE_A;
  default:
    return 0;
  }
}

// Getters:
Piece Board::get_piece_at_position(bitboard position, Color color) {
  return board_pieces[lsb(position)];
}

Move Board::get_last_move(Color color) {
  return moves[color][moves_size[color] - 1];
}

bool Board::is_moves_empty(Color color) { return moves_size[color] == 0; }

bool Board::get_can_castle_queen(Color color) {
  return castle_rights & (0x4 >> (color << 1));
}

bool Board::get_can_castle_king(Color color) {
  return castle_rights & (0x8 >> (color << 1));
}

Color Board::get_turn_color() { return turn_color; }

std::array<std::array<bitboard, 7>, 2> &Board::get_piece_bitboards() {
  return piece_bitboards;
}

int Board::get_square_index(bitboard square) {
  return square_index_lookup[square];
}

bitboard Board::get_square(int square_index) {
  return square_lookup[square_index];
}

// Setters:
void Board::set_piece_positions(Piece piece, Color color,
                                bitboard new_positions) {
  all_piece_bitboards[piece] &= ~piece_bitboards[color][piece];
  piece_bitboards[color][ALL] &= ~piece_bitboards[color][piece];
  piece_bitboards[color][piece] = new_positions;
  all_piece_bitboards[piece] |= new_positions;
  piece_bitboards[color][ALL] |= new_positions;

  while (new_positions) {
    board_pieces[lsb(new_positions)] = piece;
    zkey ^= piece_square_zkeys[color][piece][lsb(new_positions)];
    pop_lsb(new_positions);
  }
}

void Board::set_turn_color(Color new_turn_color) {
  turn_color = new_turn_color;
  zkey ^= side_zkey;
}

// Board Logic:
bool Board::is_checked(Color color) {
  return get_attacks_to_king(get_piece_positions(KING, color), color) > 0;
}

bool Board::is_move_legal(Move &move, Color color) {
  execute_move(move);
  bool is_legal = !is_checked(color);
  undo_move(move);
  return is_legal;
}

// Attacks:
bool Board::is_position_attacked_by(bitboard position, Color color) {
  std::array<Piece, 6> piece_array = {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
  for (int piece_index = 0; piece_index < 6; piece_index++) {
    if (get_piece_attacks(piece_array[piece_index], position,
                          negate_color(color)) &
        get_piece_positions(piece_array[piece_index], color)) {
      return true;
    }
  }
  return false;
}

bitboard Board::get_piece_attacks(Piece piece, bitboard position, Color color) {
  assert(position);
  assert(piece != NONE && piece != ALL);
  switch (piece) {
  case PAWN:
    return get_pawn_attacks(position, color);
  case KNIGHT:
    return get_knight_attacks(position);
  case BISHOP:
    return get_bishop_attacks(position);
  case ROOK:
    return get_rook_attacks(position);
  case QUEEN:
    return get_queen_attacks(position);
  case KING:
    return get_king_attacks(position);
  case ALL:
    return 0;
  case NONE:
    return 0;
  }
  return 0;
}

bitboard Board::get_pawn_single_push(bitboard position, Color color) {
  return pawn_single_pushes_lookups[color][lsb(position)];
}

bitboard Board::get_pawn_double_push(bitboard position, Color color) {
  return pawn_double_pushes_lookups[color][lsb(position)];
}

bitboard Board::get_blockers(bitboard position) {
  return (get_all_piece_positions(WHITE) | get_all_piece_positions(BLACK)) &
         ~position;
}

// Moves:
/**
 * Move cases:
 * Quiet move: remove bit for origin, set bit for destination
 * Capture move: remove bit for origin, remove bit for captured piece at
 * destination, set bit at destination En passant: Remove bit for origin, remove
 * bit for captured piece at its position, set bit at destination Promotion:
 * Remove bit for original piece at origin, set bit for promotion piece at
 * destination
 */
void Board::execute_move(Move &move) {
  uint8_t move_flags = move.get_flags();
  assert(move_flags != 6 && move_flags != 7);
  bitboard origin = move.get_origin();
  bitboard destination = move.get_destination();

  Piece moving_piece = get_piece_at_position(origin, turn_color);
  update_castle_rights(move, moving_piece);

  if (move_flags == 1) {
    int file_index = 7 - (lsb(origin) % 8);
    zkey ^= en_passant_zkeys[file_index];
  }

  if (move_flags == 0 || move_flags == 1) { // Quiet move
    move_piece(moving_piece, turn_color, origin, destination);
  } else if (move_flags == 2 || move_flags == 3) { // Castle move
    execute_castle_move(origin, destination);
  } else if (move_flags == 4) { // capture move
    Piece captured_piece =
        get_piece_at_position(destination, negate_color(turn_color));
    assert(captured_piece != NONE);
    remove_piece(captured_piece, negate_color(turn_color), destination);
    move_piece(moving_piece, turn_color, origin, destination);
    captured_pieces[turn_color][captured_pieces_size[turn_color]++] =
        captured_piece;
  } else if (move_flags == 5) { // en passant move
    bitboard capture_square =
        (turn_color == WHITE ? south(destination) : north(destination));
    Piece captured_piece =
        get_piece_at_position(capture_square, negate_color(turn_color));
    captured_pieces[turn_color][captured_pieces_size[turn_color]++] =
        captured_piece;
    move_piece(moving_piece, turn_color, origin, destination);
    remove_piece(captured_piece, negate_color(turn_color), capture_square);
  } else { // promotion  move
    Piece promotion_piece = get_promotion_piece_from_flags(move_flags);
    remove_piece(moving_piece, turn_color, origin);
    if (move_flags >= 12 && move_flags <= 15) {
      Piece captured_piece =
          get_piece_at_position(destination, negate_color(turn_color));
      remove_piece(captured_piece, negate_color(turn_color), destination);
      captured_pieces[turn_color][captured_pieces_size[turn_color]++] =
          captured_piece;
    }
    set_piece(promotion_piece, turn_color, destination);
  }

  Move last_move = get_last_move(negate_color(turn_color));
  if (last_move.get_flags() == 1) {
    bitboard last_origin = last_move.get_origin();
    int file_index = 7 - (lsb(last_origin) % 8);
    zkey ^= en_passant_zkeys[file_index];
  }

  moves[turn_color][moves_size[turn_color]++] = move;

  set_turn_color(negate_color(turn_color));

  assert(zkey == generate_zkey());
}

// Opposite of execute_move
void Board::undo_move(Move &move) {
  set_turn_color(negate_color(turn_color));

  uint8_t move_flags = move.get_flags();
  assert(move_flags != 6 && move_flags != 7);
  bitboard origin = move.get_origin();
  bitboard destination = move.get_destination();

  Piece moved_piece = get_piece_at_position(destination, turn_color);
  revert_castle_rights(turn_color);

  if (move_flags == 0 || move_flags == 1) { // Quiet move
    move_piece(moved_piece, turn_color, destination, origin);
  } else if (move_flags == 2 || move_flags == 3) { // Castle move
    undo_castle_move(origin, destination);
  } else if (move_flags == 4) { // capture move
    Piece captured_piece =
        captured_pieces[turn_color][--captured_pieces_size[turn_color]];
    move_piece(moved_piece, turn_color, destination, origin);
    set_piece(captured_piece, negate_color(turn_color), destination);
  } else if (move_flags == 5) { // en passant move
    bitboard capture_square =
        (turn_color == WHITE ? south(destination) : north(destination));
    Piece captured_piece =
        captured_pieces[turn_color][--captured_pieces_size[turn_color]];
    move_piece(moved_piece, turn_color, destination, origin);
    set_piece(captured_piece, negate_color(turn_color), capture_square);
  } else { // promotion move
    moved_piece = PAWN;
    Piece promotion_piece = get_promotion_piece_from_flags(move_flags);
    set_piece(moved_piece, turn_color, origin);
    remove_piece(promotion_piece, turn_color, destination);
    if (move_flags >= 12 && move_flags <= 15) {
      Piece captured_piece =
          captured_pieces[turn_color][--captured_pieces_size[turn_color]];
      set_piece(captured_piece, negate_color(turn_color), destination);
    }
  }

  // undo ep square if neccessary
  if (move_flags == 1) {
    int file_index = 7 - (lsb(origin) % 8);
    zkey ^= en_passant_zkeys[file_index];
  }

  --moves_size[turn_color];

  // put previous ep square back in hash if neccessary
  Move last_move = get_last_move(negate_color(turn_color));
  if (last_move.get_flags() == 1) {
    bitboard last_origin = last_move.get_origin();
    int file_index = 7 - (lsb(last_origin) % 8);
    zkey ^= en_passant_zkeys[file_index];
  }

  assert(zkey == generate_zkey());
}

// Print:
void Board::print() {
  std::string separator_line(17, '-');
  std::array<char, 8> piece_chars = {'p', 'n', 'b', 'r', 'q', 'k', ' ', ' '};
  bitboard mask = 0x8000000000000000;
  Piece current_piece;
  std::cout << " " << separator_line << std::endl;
  for (int row = 0; row < 8; row++) {
    std::cout << (8 - row) << "|";
    for (int col = 0; col < 8; col++) {
      current_piece = get_piece_at_position(mask, WHITE);
      if (current_piece < 6) {
        std::cout << (char)toupper(piece_chars[current_piece]);
      } else {
        current_piece = get_piece_at_position(mask, BLACK);
        std::cout << piece_chars[current_piece];
      }
      std::cout << "|";
      mask >>= 1;
    }
    std::cout << std::endl << " " << separator_line << std::endl;
  }
  std::cout << "  A B C D E F G H" << std::endl;
  std::cout << (turn_color == WHITE ? "White" : "Black") << " to move"
            << std::endl;
}

// Zobrist Keys:
void Board::init_zobrist_keys() {
  std::mt19937_64 rand_num_gen(1);

  for (int color_index = 0; color_index < 2; ++color_index) {
    for (int piece_index = 0; piece_index < 6; ++piece_index) {
      for (int square_index = 0; square_index < 64; ++square_index) {
        piece_square_zkeys[color_index][piece_index][square_index] =
            rand_num_gen();
      }
    }
  }

  side_zkey = rand_num_gen();

  for (int i = 0; i < 4; ++i) {
    castling_zkeys[i] = rand_num_gen();
  }

  for (int i = 0; i < 8; ++i) {
    en_passant_zkeys[i] = rand_num_gen();
  }
}

uint64_t Board::generate_zkey() {
  uint64_t new_zkey = 1;

  for (Color color = WHITE; color <= BLACK; color = (Color)(color + 1)) {
    for (Piece piece = PAWN; piece <= KING; piece = (Piece)(piece + 1)) {
      bitboard piece_positions = get_piece_positions(piece, color);
      while (piece_positions) {
        int square_index = lsb(piece_positions);
        new_zkey ^= piece_square_zkeys[color][piece][square_index];
        piece_positions &= piece_positions - 1; // pop
      }
    }
  }

  int castle_rights_index = 0;
  for (uint8_t mask = 0x8; mask > 0; mask >>= 1) {
    if (castle_rights & mask) {
      new_zkey ^= castling_zkeys[castle_rights_index];
    }
    ++castle_rights_index;
  }

  if (!is_moves_empty(negate_color(turn_color))) {
    Move last_move = get_last_move(negate_color(turn_color));
    if (last_move.get_flags() == 1) {
      bitboard origin = last_move.get_origin();
      bitboard file_masks[8] = {FILE_A, FILE_B, FILE_C, FILE_D,
                                FILE_E, FILE_F, FILE_G, FILE_H};
      for (int file_index = 0; file_index < 8; ++file_index) {
        if (origin & file_masks[file_index]) {
          new_zkey ^= en_passant_zkeys[file_index];
          break;
        }
      }
    }
  }

  if (turn_color == WHITE) {
    new_zkey ^= side_zkey;
  }

  return new_zkey;
}

// Castling:
void Board::update_castle_rights(Move &move, Piece moving_piece) {
  bitboard origin = move.get_origin();
  previous_castle_rights[castle_rights_size++] = castle_rights;
  if (moving_piece == KING) {
    if (get_can_castle_king(turn_color)) {
      clear_king_castle_right(turn_color);
    }
    if (get_can_castle_queen(turn_color)) {
      clear_queen_castle_right(turn_color);
    }
  } else if (moving_piece == ROOK) {
    if ((turn_color == WHITE && origin == 0x80) ||
        (turn_color == BLACK && origin == 0x8000000000000000)) {
      if (get_can_castle_queen(turn_color)) {
        clear_queen_castle_right(turn_color);
      }
    } else if ((turn_color == WHITE && origin == 1) ||
               (turn_color == BLACK && origin == 0x100000000000000)) {
      if (get_can_castle_king(turn_color)) {
        clear_king_castle_right(turn_color);
      }
    }
  }
}

void Board::revert_castle_rights(Color color) {
  uint8_t prev_rights = previous_castle_rights[--castle_rights_size];
  uint8_t changed = prev_rights ^ castle_rights;
  if (changed & 0x8) {
    zkey ^= castling_zkeys[0];
  }
  if (changed & 0x4) {
    zkey ^= castling_zkeys[1];
  }
  if (changed & 0x2) {
    zkey ^= castling_zkeys[2];
  }
  if (changed & 0x1) {
    zkey ^= castling_zkeys[3];
  }
  castle_rights = prev_rights;
}

// Moves:
void Board::move_piece(Piece piece, Color color, bitboard origin,
                       bitboard destination) {
  piece_bitboards[color][piece] ^= (origin | destination); // piece bitboard
  piece_bitboards[color][ALL] ^=
      (origin | destination); // color all piece bitboard
  all_piece_bitboards[piece] =
      piece_bitboards[WHITE][piece] | piece_bitboards[BLACK][piece];

  board_pieces[lsb(origin)] = NONE;
  board_pieces[lsb(destination)] = piece;

  zkey ^= piece_square_zkeys[color][piece][lsb(origin)];
  zkey ^= piece_square_zkeys[color][piece][lsb(destination)];
}

void Board::set_piece(Piece piece, Color color, bitboard position) {
  assert((piece_bitboards[color][piece] & position) == 0);
  piece_bitboards[color][piece] |= position;
  piece_bitboards[color][ALL] |= position;
  all_piece_bitboards[piece] =
      piece_bitboards[WHITE][piece] | piece_bitboards[BLACK][piece];

  board_pieces[lsb(position)] = piece;

  zkey ^= piece_square_zkeys[color][piece][lsb(position)];
}

void Board::remove_piece(Piece piece, Color color, bitboard position) {
  assert(piece_bitboards[color][piece] & position);
  piece_bitboards[color][piece] &= ~position;
  piece_bitboards[color][ALL] &= ~position;
  all_piece_bitboards[piece] =
      piece_bitboards[WHITE][piece] | piece_bitboards[BLACK][piece];

  board_pieces[lsb(position)] = NONE;
  zkey ^= piece_square_zkeys[color][piece][lsb(position)];
}

void Board::execute_castle_move(bitboard king_origin,
                                bitboard king_destination) {
  bitboard rook_origin = castle_rook_origin_lookup[lsb(king_destination)];
  bitboard rook_destination =
      castle_rook_destination_lookup[lsb(king_destination)];

  move_piece(KING, turn_color, king_origin, king_destination);
  move_piece(ROOK, turn_color, rook_origin, rook_destination);
}

void Board::undo_castle_move(bitboard king_origin, bitboard king_destination) {
  bitboard rook_origin = castle_rook_origin_lookup[lsb(king_destination)];
  bitboard rook_destination =
      castle_rook_destination_lookup[lsb(king_destination)];

  move_piece(KING, turn_color, king_destination, king_origin);
  move_piece(ROOK, turn_color, rook_destination, rook_origin);
}

// Attacks:
bitboard Board::get_pawn_attacks(bitboard position, Color color) {
  if (!position) {
    return 0;
  }
  return pawn_attacks_lookups[color][lsb(position)];
}

bitboard Board::get_knight_attacks(bitboard position) {
  if (!position) {
    return 0;
  }
  return knight_moves_lookup[lsb(position)];
}

bitboard Board::get_bishop_attacks(bitboard position) {
  if (!position) {
    return 0;
  }
  int square_index = lsb(position);
  bitboard blockers =
      (get_all_piece_positions(WHITE) | get_all_piece_positions(BLACK)) &
      BISHOP_MASKS[square_index];
  uint64_t key =
      (blockers * BISHOP_MAGICS[square_index]) >> BISHOP_SHIFTS[square_index];
  bitboard result = bishop_attacks_magic_bb[square_index][key];
  return result;
}

bitboard Board::get_rook_attacks(bitboard position) {
  if (!position) {
    return 0;
  }
  int square_index = lsb(position);
  bitboard blockers =
      (get_all_piece_positions(WHITE) | get_all_piece_positions(BLACK)) &
      ROOK_MASKS[square_index];
  uint64_t key =
      (blockers * ROOK_MAGICS[square_index]) >> ROOK_SHIFTS[square_index];
  bitboard result = rook_attacks_magic_bb[square_index][key];
  return result;
}

bitboard Board::get_queen_attacks(bitboard position) {
  if (!position) {
    return 0;
  }
  return get_rook_attacks(position) | get_bishop_attacks(position);
}

bitboard Board::get_king_attacks(bitboard position) {
  if (!position) {
    return 0;
  }
  return king_moves_lookup[lsb(position)];
}

bitboard Board::get_attacks_to_king(bitboard king_position, Color king_color) {
  Color op_color = negate_color(king_color);
  return (get_pawn_attacks(king_position, king_color) &
          get_piece_positions(PAWN, op_color)) |
    (get_knight_attacks(king_position) &
     get_piece_positions(KNIGHT, op_color)) |
    (get_bishop_attacks(king_position) &
     (get_piece_positions(BISHOP, op_color) |
      get_piece_positions(QUEEN, op_color))) |
    (get_rook_attacks(king_position) &
     (get_piece_positions(ROOK, op_color) |
      get_piece_positions(QUEEN, op_color))) |
    (get_king_attacks(king_position) &
     (get_piece_positions(KING, op_color)));
}

// Sliding piece attack generation
bitboard Board::generate_sliding_attacks(bitboard position, Direction direction,
                                         bitboard blockers) {
  assert((get_all_piece_positions(WHITE) & get_all_piece_positions(BLACK)) ==
         0);

  bitboard result = 0;
  while ((position & blockers) == 0) {
    position = move_direction(position, direction);
    if (!position) {
      break;
    }
    result |= position;
  }

  return result;
}

bitboard Board::generate_bishop_attacks(bitboard position, bitboard blockers) {
  return generate_sliding_attacks(position, NORTHEAST, blockers) |
         generate_sliding_attacks(position, NORTHWEST, blockers) |
         generate_sliding_attacks(position, SOUTHEAST, blockers) |
         generate_sliding_attacks(position, SOUTHWEST, blockers);
}

bitboard Board::generate_rook_attacks(bitboard position, bitboard blockers) {
  return generate_sliding_attacks(position, NORTH, blockers) |
         generate_sliding_attacks(position, EAST, blockers) |
         generate_sliding_attacks(position, SOUTH, blockers) |
         generate_sliding_attacks(position, WEST, blockers);
}

// Helpers:
bitboard Board::move_direction(bitboard position, Direction direction) {
  switch (direction) {
  case NORTH:
    return north(position);
  case EAST:
    return east(position);
  case SOUTH:
    return south(position);
  case WEST:
    return west(position);
  case NORTHEAST:
    return north(east(position));
  case NORTHWEST:
    return north(west(position));
  case SOUTHEAST:
    return south(east(position));
  case SOUTHWEST:
    return south(west(position));
  }
}

Piece Board::get_promotion_piece_from_flags(uint8_t flags) {
  switch (flags) {
  case 8:
    return KNIGHT;
  case 9:
    return BISHOP;
  case 10:
    return ROOK;
  case 11:
    return QUEEN;
  case 12:
    return KNIGHT;
  case 13:
    return BISHOP;
  case 14:
    return ROOK;
  case 15:
    return QUEEN;
  }
  return NONE;
}

void Board::verify_board_pieces_consistency() {
  for (int square_index = 0; square_index < 64; square_index++) {
    bitboard square = ((uint64_t)1) << square_index;
    Piece square_piece = board_pieces[square_index];
    if (square_piece == NONE) {
      assert(
          !((get_all_piece_positions(WHITE) | get_all_piece_positions(BLACK)) &
            square));
    } else {
      assert(all_piece_bitboards[square_piece] & square);
    }
  }
}

// Lookup Tables:
void Board::initialize_lookups() { // Called by constructor
  initialize_square_lookups();
  initialize_single_pawn_pushes_lookups();
  initialize_double_pawn_pushes_lookups();
  initialize_pawn_attacks_lookups();
  initialize_knight_moves_lookup();
  initialize_king_moves_lookup();
  initialize_castle_rook_origin_lookup();
  initialize_castle_rook_destination_lookup();

  initialize_rook_attacks_magic_bb();
  initialize_bishop_attacks_magic_bb();
}

void Board::initialize_square_lookups() {
  bitboard square = 0x8000000000000000;
  for (int square_index = 63; square_index >= 0; --square_index) {
    square_lookup[square_index] = square;
    square_index_lookup[square] = square_index;
    square >>= 1;
  }
}

void Board::initialize_single_pawn_pushes_lookups() {
  for (int square_index = 0; square_index < 64; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    pawn_single_pushes_lookups[WHITE][square_index] = north(position);
    pawn_single_pushes_lookups[BLACK][square_index] = south(position);
  }
}

void Board::initialize_double_pawn_pushes_lookups() {
  for (int square_index = 8; square_index < 16; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    pawn_double_pushes_lookups[WHITE][square_index] = north(north(position));
  }
  for (int square_index = 48; square_index < 56; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    pawn_double_pushes_lookups[BLACK][square_index] = south(south(position));
  }
}

void Board::initialize_pawn_attacks_lookups() {
  for (int square_index = 0; square_index < 64; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    pawn_attacks_lookups[WHITE][square_index] =
        east(north(position)) | west(north(position));
    pawn_attacks_lookups[BLACK][square_index] =
        east(south(position)) | west(south(position));
  }
}

void Board::initialize_knight_moves_lookup() {
  for (int square_index = 0; square_index < 64; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    knight_moves_lookup[square_index] =
        east(north(north(position))) | west(north(north(position))) |
        east(south(south(position))) | west(south(south(position))) |
        north(east(east(position))) | north(west(west(position))) |
        south(east(east(position))) | south(west(west(position)));
  }
}

void Board::initialize_king_moves_lookup() {
  for (int square_index = 0; square_index < 64; ++square_index) {
    bitboard position = ((bitboard)1) << square_index;
    king_moves_lookup[square_index] =
        north(position) | east(position) | south(position) | west(position) |
        east(north(position)) | west(north(position)) | east(south(position)) |
        west(south(position));
  }
}

void Board::initialize_castle_rook_origin_lookup() {
  castle_rook_origin_lookup[lsb(0x200000000000000)] = 0x100000000000000;
  castle_rook_origin_lookup[lsb(0x2000000000000000)] = 0x8000000000000000;
  castle_rook_origin_lookup[lsb(0x2)] = 0x1;
  castle_rook_origin_lookup[lsb(0x20)] = 0x80;
}

void Board::initialize_castle_rook_destination_lookup() {
  castle_rook_destination_lookup[lsb(0x200000000000000)] = 0x400000000000000;
  castle_rook_destination_lookup[lsb(0x2000000000000000)] = 0x1000000000000000;
  castle_rook_destination_lookup[lsb(0x2)] = 0x4;
  castle_rook_destination_lookup[lsb(0x20)] = 0x10;
}

void Board::initialize_rook_attacks_magic_bb() {
  for (int square_index = 0; square_index < 64; square_index++) {
    bitboard square = square_lookup[square_index];

    bitboard blockers = 0;
    do {
      bitboard attack_set = generate_rook_attacks(square, blockers);

      uint64_t key =
          ((blockers & ROOK_MASKS[square_index]) * ROOK_MAGICS[square_index]) >>
          ROOK_SHIFTS[square_index];
      rook_attacks_magic_bb[square_index][key] = attack_set;

      blockers = (blockers - ROOK_MASKS[square_index]) &
                 ROOK_MASKS[square_index]; // get next subset of blockers
    } while (blockers);
  }
}

void Board::initialize_bishop_attacks_magic_bb() {
  for (int square_index = 0; square_index < 64; square_index++) {
    bitboard square = square_lookup[square_index];

    bitboard blockers = 0;
    do {
      bitboard attack_set = generate_bishop_attacks(square, blockers);

      uint64_t key = ((blockers & BISHOP_MASKS[square_index]) *
                      BISHOP_MAGICS[square_index]) >>
                     BISHOP_SHIFTS[square_index];
      bishop_attacks_magic_bb[square_index][key] = attack_set;

      blockers = (blockers - BISHOP_MASKS[square_index]) &
                 BISHOP_MASKS[square_index]; // get next subset of blockers
    } while (blockers);
  }
}

// Constants:
const std::array<bitboard, 64> Board::ROOK_MAGICS = {
    0x0080001020400080, 0x0040001000200040, 0x0080081000200080,
    0x0080040800100080, 0x0080020400080080, 0x0080010200040080,
    0x0080008001000200, 0x0080002040800100, 0x0000800020400080,
    0x0000400020005000, 0x0000801000200080, 0x0000800800100080,
    0x0000800400080080, 0x0000800200040080, 0x0000800100020080,
    0x0000800040800100, 0x0000208000400080, 0x0000404000201000,
    0x0000808010002000, 0x0000808008001000, 0x0000808004000800,
    0x0000808002000400, 0x0000010100020004, 0x0000020000408104,
    0x0000208080004000, 0x0000200040005000, 0x0000100080200080,
    0x0000080080100080, 0x0000040080080080, 0x0000020080040080,
    0x0000010080800200, 0x0000800080004100, 0x0000204000800080,
    0x0000200040401000, 0x0000100080802000, 0x0000080080801000,
    0x0000040080800800, 0x0000020080800400, 0x0000020001010004,
    0x0000800040800100, 0x0000204000808000, 0x0000200040008080,
    0x0000100020008080, 0x0000080010008080, 0x0000040008008080,
    0x0000020004008080, 0x0000010002008080, 0x0000004081020004,
    0x0000204000800080, 0x0000200040008080, 0x0000100020008080,
    0x0000080010008080, 0x0000040008008080, 0x0000020004008080,
    0x0000800100020080, 0x0000800041000080, 0x00FFFCDDFCED714A,
    0x007FFCDDFCED714A, 0x003FFFCDFFD88096, 0x0000040810002101,
    0x0001000204080011, 0x0001000204000801, 0x0001000082000401,
    0x0001FFFAABFAD1A2};
const std::array<bitboard, 64> Board::ROOK_MASKS = {
    0x000101010101017E, 0x000202020202027C, 0x000404040404047A,
    0x0008080808080876, 0x001010101010106E, 0x002020202020205E,
    0x004040404040403E, 0x008080808080807E, 0x0001010101017E00,
    0x0002020202027C00, 0x0004040404047A00, 0x0008080808087600,
    0x0010101010106E00, 0x0020202020205E00, 0x0040404040403E00,
    0x0080808080807E00, 0x00010101017E0100, 0x00020202027C0200,
    0x00040404047A0400, 0x0008080808760800, 0x00101010106E1000,
    0x00202020205E2000, 0x00404040403E4000, 0x00808080807E8000,
    0x000101017E010100, 0x000202027C020200, 0x000404047A040400,
    0x0008080876080800, 0x001010106E101000, 0x002020205E202000,
    0x004040403E404000, 0x008080807E808000, 0x0001017E01010100,
    0x0002027C02020200, 0x0004047A04040400, 0x0008087608080800,
    0x0010106E10101000, 0x0020205E20202000, 0x0040403E40404000,
    0x0080807E80808000, 0x00017E0101010100, 0x00027C0202020200,
    0x00047A0404040400, 0x0008760808080800, 0x00106E1010101000,
    0x00205E2020202000, 0x00403E4040404000, 0x00807E8080808000,
    0x007E010101010100, 0x007C020202020200, 0x007A040404040400,
    0x0076080808080800, 0x006E101010101000, 0x005E202020202000,
    0x003E404040404000, 0x007E808080808000, 0x7E01010101010100,
    0x7C02020202020200, 0x7A04040404040400, 0x7608080808080800,
    0x6E10101010101000, 0x5E20202020202000, 0x3E40404040404000,
    0x7E80808080808000};
const std::array<bitboard, 64> Board::ROOK_SHIFTS = {
    52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53,
    53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 53, 53, 53, 53, 53};
const std::array<bitboard, 64> Board::BISHOP_MASKS = {
    0x0040201008040200, 0x0000402010080400, 0x0000004020100A00,
    0x0000000040221400, 0x0000000002442800, 0x0000000204085000,
    0x0000020408102000, 0x0002040810204000, 0x0020100804020000,
    0x0040201008040000, 0x00004020100A0000, 0x0000004022140000,
    0x0000000244280000, 0x0000020408500000, 0x0002040810200000,
    0x0004081020400000, 0x0010080402000200, 0x0020100804000400,
    0x004020100A000A00, 0x0000402214001400, 0x0000024428002800,
    0x0002040850005000, 0x0004081020002000, 0x0008102040004000,
    0x0008040200020400, 0x0010080400040800, 0x0020100A000A1000,
    0x0040221400142200, 0x0002442800284400, 0x0004085000500800,
    0x0008102000201000, 0x0010204000402000, 0x0004020002040800,
    0x0008040004081000, 0x00100A000A102000, 0x0022140014224000,
    0x0044280028440200, 0x0008500050080400, 0x0010200020100800,
    0x0020400040201000, 0x0002000204081000, 0x0004000408102000,
    0x000A000A10204000, 0x0014001422400000, 0x0028002844020000,
    0x0050005008040200, 0x0020002010080400, 0x0040004020100800,
    0x0000020408102000, 0x0000040810204000, 0x00000A1020400000,
    0x0000142240000000, 0x0000284402000000, 0x0000500804020000,
    0x0000201008040200, 0x0000402010080400, 0x0002040810204000,
    0x0004081020400000, 0x000A102040000000, 0x0014224000000000,
    0x0028440200000000, 0x0050080402000000, 0x0020100804020000,
    0x0040201008040200};
const std::array<bitboard, 64> Board::BISHOP_MAGICS = {
    0x0002020202020200, 0x0002020202020000, 0x0004010202000000,
    0x0004040080000000, 0x0001104000000000, 0x0000821040000000,
    0x0000410410400000, 0x0000104104104000, 0x0000040404040400,
    0x0000020202020200, 0x0000040102020000, 0x0000040400800000,
    0x0000011040000000, 0x0000008210400000, 0x0000004104104000,
    0x0000002082082000, 0x0004000808080800, 0x0002000404040400,
    0x0001000202020200, 0x0000800802004000, 0x0000800400A00000,
    0x0000200100884000, 0x0000400082082000, 0x0000200041041000,
    0x0002080010101000, 0x0001040008080800, 0x0000208004010400,
    0x0000404004010200, 0x0000840000802000, 0x0000404002011000,
    0x0000808001041000, 0x0000404000820800, 0x0001041000202000,
    0x0000820800101000, 0x0000104400080800, 0x0000020080080080,
    0x0000404040040100, 0x0000808100020100, 0x0001010100020800,
    0x0000808080010400, 0x0000820820004000, 0x0000410410002000,
    0x0000082088001000, 0x0000002011000800, 0x0000080100400400,
    0x0001010101000200, 0x0002020202000400, 0x0001010101000200,
    0x0000410410400000, 0x0000208208200000, 0x0000002084100000,
    0x0000000020880000, 0x0000001002020000, 0x0000040408020000,
    0x0004040404040000, 0x0002020202020000, 0x0000104104104000,
    0x0000002082082000, 0x0000000020841000, 0x0000000000208800,
    0x0000000010020200, 0x0000000404080200, 0x0000040404040400,
    0x0002020202020200};
const std::array<bitboard, 64> Board::BISHOP_SHIFTS = {
    58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59,
    59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59,
    59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59,
    59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

#endif
