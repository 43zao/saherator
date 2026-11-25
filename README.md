# Šaherator

A bitboard-based chess engine written from scratch in C++.

Built with passion as a learning project that connects my career with my favorite passtime. :]

2000+ Lichess elo.

Challenge it at: https://lichess.org/@/saherator

## Features

- Fast legal / pseudo-legal move generation
- Captures only generation
- Magic bitboards
- Flawless perft
- UCI support
- Alpha-beta pruning negamax search
- Move ordering
- Killer moves
- History heuristics
- Principal variation search
- Late move reductions
- Check extensions
- Reverse futility pruning
- Null move pruning
- Quiescence search
- Transposition tables
- Aspiration windows
- Positional evaluation:
  - material
  - piece square tables
  - king safety
  - pawn structure
  - tapered evaluation (mid / endgame)
- Iterative deepening
- Time management
- Pondering
- Weighted Polyglot opening book

## Building

The engine uses CMake and requires a C++17 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

By default, CMake embeds `opening-book/komodo.bin` into the executable. The `.bin` file is needed when building, but does not need to accompany the finished executable. You can select a different external file from your UCI GUI with `BookFile`, or disable book moves with `OwnBook`:

```text
setoption name BookFile value /path/to/my-book.bin
setoption name OwnBook value false
```

### Opening book setup

The default CMake configuration embeds the Komodo book. To choose another Polyglot `.bin` book to embed, set its path when configuring:

```sh
cmake -S . -B build-embedded \
  -DCMAKE_BUILD_TYPE=Release \
  -DSAHERATOR_EMBED_OPENING_BOOK=ON \
  -DSAHERATOR_OPENING_BOOK_FILE=/path/to/another-book.bin
cmake --build build-embedded --config Release
```

`SAHERATOR_OPENING_BOOK_FILE` defaults to `opening-book/komodo.bin`. To build without an embedded book and load one at runtime, configure with `-DSAHERATOR_EMBED_OPENING_BOOK=OFF`; the runtime default path is `opening-book/komodo.bin`. UCI options can still disable book moves or select another external book at runtime.

For the file format, runtime configuration, and supported UCI options, see [Opening book support](docs/documentation.md#opening-book-support).

## Roadmap

- Multithreading options for search
- Static exchange evaluation (SEE)
- Quiescence improvements
- Incremental cached evaluations
- Adding perft to UCI
- Adding namespaces to board, movegen, attacks
- Recapture, passed pawn extensions
- Aspiration window tuning
- History heuristic decay
- Transposition table upgrades
- Mobility evaluation
- King safety eval improvements
- Pawn structure eval improvements
- Endgame tablebase support

## Documentation

See `docs/`
