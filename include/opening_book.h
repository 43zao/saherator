#pragma once
#include "movegen.h"
#include <string>

// Reads Polyglot opening books and converts their moves to/from the engine's
// packed Move representation.

namespace OpeningBook
{
    struct Entry
    {
        U64 key;
        U16 move;
        U16 weight;
        U32 learn;
    };

    // loads a caller-selected Polyglot book.
    // an empty path disables the book.
    bool initialize(const std::string &path);

    // enables or disables book probing without discarding the loaded book.
    void set_enabled(bool enabled);

    // returns the Polyglot position key.
    U64 polyglot_hash(const Board &board);

    // converts a raw 16-bit Polyglot move into a legal engine move.
    bool decode_polyglot_move(const Board &board, U16 rawMove, Move &engineMove);

    // converts an engine move back to the Polyglot 16-bit move encoding.
    U16 encode_polyglot_move(const Board &board, Move engineMove);

    // selects a legal book move for the position, weighted by book frequency.
    bool probe(const Board &board, Move &engineMove);
}
