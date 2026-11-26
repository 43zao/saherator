#pragma once
#include "movegen.h"
#include <string>

// Implements Syzygy tablebase support using Fathom

namespace Tablebase
{
    enum class RootStatus
    {
        Unavailable,
        MoveFound,
        Checkmate,
        Stalemate
    };

    // Loads Syzygy files from a Fathom path list. An empty path unloads them.
    bool initialize(const std::string &path);
    unsigned largest();

    // Returns WDL from the side-to-move perspective (-2 loss, 0 draw, +2 win).
    bool probe_wdl(const Board &board, int &wdl);

    // Probes the root position using DTZ when available and returns its move/WDL.
    RootStatus probe_root(const Board &board, Move &move, int &wdl);
}
