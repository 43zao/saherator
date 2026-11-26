#include "tablebase.h"
#include "attacks.h"
#include "tbprobe.h"

namespace Tablebase
{
    namespace
    {
        bool loaded = false;

        struct Position
        {
            U64 white;
            U64 black;
            U64 kings;
            U64 queens;
            U64 rooks;
            U64 bishops;
            U64 knights;
            U64 pawns;
            unsigned ep;
            bool whiteToMove;
        };

        Position to_position(const Board &board)
        {
            return {
                board.occ[WHITE],
                board.occ[BLACK],
                board.pieces[W_K] | board.pieces[B_K],
                board.pieces[W_Q] | board.pieces[B_Q],
                board.pieces[W_R] | board.pieces[B_R],
                board.pieces[W_B] | board.pieces[B_B],
                board.pieces[W_N] | board.pieces[B_N],
                board.pieces[W_P] | board.pieces[B_P],
                board.ep < 0 ? 0U : static_cast<unsigned>(board.ep),
                board.stm == WHITE};
        }

        bool can_probe(const Board &board)
        {
            return loaded && board.castling == 0 &&
                   bit_count(board.occ[BOTH]) <= static_cast<int>(TB_LARGEST);
        }

        int convert_wdl(unsigned result)
        {
            switch (result)
            {
            case TB_WIN: return 2;
            case TB_LOSS: return -2;
            case TB_CURSED_WIN:
            case TB_BLESSED_LOSS:
            case TB_DRAW: return 0;
            default: return 0;
            }
        }

        bool decode_root_move(unsigned result, const Board &board, Move &move)
        {
            const int from = static_cast<int>(TB_GET_FROM(result));
            const int to = static_cast<int>(TB_GET_TO(result));
            const unsigned promotion = TB_GET_PROMOTES(result);

            int promotionPiece = -1;
            switch (promotion)
            {
            case TB_PROMOTES_NONE: break;
            case TB_PROMOTES_QUEEN: promotionPiece = W_Q; break;
            case TB_PROMOTES_ROOK: promotionPiece = W_R; break;
            case TB_PROMOTES_BISHOP: promotionPiece = W_B; break;
            case TB_PROMOTES_KNIGHT: promotionPiece = W_N; break;
            default: return false;
            }

            if (board.stm == BLACK && promotionPiece >= 0)
                promotionPiece += 6;

            MoveList legalMoves;
            generate_legal_fast(board, legalMoves);
            for (int i = 0; i < legalMoves.count; ++i)
            {
                const Move candidate = legalMoves.moves[i];
                const bool isPromotion = (move_flags(candidate) & MF_PROMO) != 0;

                if (move_from(candidate) != from || move_to(candidate) != to)
                    continue;
                if (isPromotion != (promotion != TB_PROMOTES_NONE))
                    continue;
                if (isPromotion && move_piece(candidate) != promotionPiece)
                    continue;

                move = candidate;
                return true;
            }

            return false;
        }
    }

    bool initialize(const std::string &path)
    {
        loaded = tb_init(path.c_str()) && TB_LARGEST > 0;
        return loaded;
    }

    unsigned largest()
    {
        return loaded ? TB_LARGEST : 0;
    }

    bool probe_wdl(const Board &board, int &wdl)
    {
        if (!can_probe(board) || board.halfmove != 0) return false;

        const Position position = to_position(board);
        const unsigned result = tb_probe_wdl(
            position.white,
            position.black,
            position.kings,
            position.queens,
            position.rooks,
            position.bishops,
            position.knights,
            position.pawns,
            board.halfmove,
            board.castling,
            position.ep,
            position.whiteToMove);

        if (result == TB_RESULT_FAILED) return false;

        wdl = convert_wdl(result);
        return true;
    }

    RootStatus probe_root(const Board &board, Move &move, int &wdl)
    {
        move = 0;
        if (!can_probe(board)) return RootStatus::Unavailable;

        const Position position = to_position(board);
        const unsigned result = tb_probe_root(
            position.white,
            position.black,
            position.kings,
            position.queens,
            position.rooks,
            position.bishops,
            position.knights,
            position.pawns,
            board.halfmove,
            board.castling,
            position.ep,
            position.whiteToMove,
            nullptr);

        if (result == TB_RESULT_FAILED) return RootStatus::Unavailable;
        if (result == TB_RESULT_CHECKMATE)
        {
            wdl = -2;
            return RootStatus::Checkmate;
        }
        if (result == TB_RESULT_STALEMATE)
        {
            wdl = 0;
            return RootStatus::Stalemate;
        }

        wdl = convert_wdl(TB_GET_WDL(result));
        if (!decode_root_move(result, board, move)) return RootStatus::Unavailable;

        return RootStatus::MoveFound;
    }
}
