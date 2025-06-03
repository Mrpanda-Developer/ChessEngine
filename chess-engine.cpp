#include <iostream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <algorithm>

using namespace std;

enum class Piece { EMPTY, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum class Color { WHITE, BLACK, NONE };

struct Square {
    Piece piece = Piece::EMPTY;
    Color color = Color::NONE;
    bool hasMoved = false; // Voor rokeren en pion double-move
};

class Board {
public:
    Square grid[8][8];
    Color turn = Color::WHITE;
    pair<int, int> enPassantTarget = {-1, -1}; // (x, y) coordinaat of (-1, -1) als er geen is


    Board() { setupBoard(); }

    void setupBoard() {
        // Zwarte stukken
        grid[0][0] = {Piece::ROOK, Color::BLACK};
        grid[0][1] = {Piece::KNIGHT, Color::BLACK};
        grid[0][2] = {Piece::BISHOP, Color::BLACK};
        grid[0][3] = {Piece::QUEEN, Color::BLACK};
        grid[0][4] = {Piece::KING, Color::BLACK};
        grid[0][5] = {Piece::BISHOP, Color::BLACK};
        grid[0][6] = {Piece::KNIGHT, Color::BLACK};
        grid[0][7] = {Piece::ROOK, Color::BLACK};
        for (int i = 0; i < 8; ++i) grid[1][i] = {Piece::PAWN, Color::BLACK};

        // Witte stukken
        grid[7][0] = {Piece::ROOK, Color::WHITE};
        grid[7][1] = {Piece::KNIGHT, Color::WHITE};
        grid[7][2] = {Piece::BISHOP, Color::WHITE};
        grid[7][3] = {Piece::QUEEN, Color::WHITE};
        grid[7][4] = {Piece::KING, Color::WHITE};
        grid[7][5] = {Piece::BISHOP, Color::WHITE};
        grid[7][6] = {Piece::KNIGHT, Color::WHITE};
        grid[7][7] = {Piece::ROOK, Color::WHITE};
        for (int i = 0; i < 8; ++i) grid[6][i] = {Piece::PAWN, Color::WHITE};
    }

    void printBoard() {
        cout << "  a b c d e f g h\n";
        for (int y = 0; y < 8; ++y) {
            cout << 8 - y << " ";
            for (int x = 0; x < 8; ++x) {
                char c = '.';
                if (grid[y][x].color == Color::WHITE) {
                    c = getPieceChar(grid[y][x].piece, true);
                } else if (grid[y][x].color == Color::BLACK) {
                    c = getPieceChar(grid[y][x].piece, false);
                }
                cout << c << " ";
            }
            cout << 8 - y << "\n";
        }
        cout << "  a b c d e f g h\n";
    }

    char getPieceChar(Piece p, bool isWhite) {
        static unordered_map<Piece, char> whitePieces = {
            {Piece::PAWN, 'P'}, {Piece::KNIGHT, 'N'}, {Piece::BISHOP, 'B'},
            {Piece::ROOK, 'R'}, {Piece::QUEEN, 'Q'}, {Piece::KING, 'K'}
        };
        static unordered_map<Piece, char> blackPieces = {
            {Piece::PAWN, 'p'}, {Piece::KNIGHT, 'n'}, {Piece::BISHOP, 'b'},
            {Piece::ROOK, 'r'}, {Piece::QUEEN, 'q'}, {Piece::KING, 'k'}
        };
        if (p == Piece::EMPTY) return '.';
        return isWhite ? whitePieces[p] : blackPieces[p];
    }

    // Controleer of een zet geldig is volgens schaakregels
    bool isValidMove(int fromX, int fromY, int toX, int toY, Color player) {
        if (fromX < 0 || fromX > 7 || fromY < 0 || fromY > 7 ||
            toX < 0 || toX > 7 || toY < 0 || toY > 7) return false;
        if (grid[fromY][fromX].color != player) return false;
        if (grid[toY][toX].color == player) return false; // Eigen stuk slaat niet

        Piece piece = grid[fromY][fromX].piece;
        int dx = toX - fromX, dy = toY - fromY;

        // Specifieke regels per stuk
       switch (piece) {
    case Piece::PAWN: {
        int dir = (player == Color::WHITE) ? -1 : 1;
        if (dx == 0) {
            if (dy == dir && grid[toY][toX].piece == Piece::EMPTY) return true;
            if (dy == 2 * dir && !grid[fromY][fromX].hasMoved &&
                grid[fromY + dir][fromX].piece == Piece::EMPTY &&
                grid[toY][toX].piece == Piece::EMPTY) return true;
        } else if (abs(dx) == 1 && dy == dir) {
            if (grid[toY][toX].color == (player == Color::WHITE ? Color::BLACK : Color::WHITE)) return true;
            if (toX == enPassantTarget.first && toY == enPassantTarget.second) return true;
        }
        return false;
    }

    case Piece::KNIGHT:
        return (abs(dx) == 2 && abs(dy) == 1) || (abs(dx) == 1 && abs(dy) == 2);

    case Piece::BISHOP: {
        if (abs(dx) != abs(dy)) return false;
        int stepX = dx > 0 ? 1 : -1;
        int stepY = dy > 0 ? 1 : -1;
        for (int x = fromX + stepX, y = fromY + stepY; x != toX; x += stepX, y += stepY) {
            if (grid[y][x].piece != Piece::EMPTY) return false;
        }
        return true;
    }

    case Piece::ROOK: {
        if (dx != 0 && dy != 0) return false;
        if (dx == 0) {
            int step = dy > 0 ? 1 : -1;
            for (int y = fromY + step; y != toY; y += step) {
                if (grid[y][fromX].piece != Piece::EMPTY) return false;
            }
        } else {
            int step = dx > 0 ? 1 : -1;
            for (int x = fromX + step; x != toX; x += step) {
                if (grid[fromY][x].piece != Piece::EMPTY) return false;
            }
        }
        return true;
    }

    case Piece::QUEEN: {
        if (dx == 0 || dy == 0) {
            // Rookbeweging
            if (dx == 0) {
                int step = dy > 0 ? 1 : -1;
                for (int y = fromY + step; y != toY; y += step) {
                    if (grid[y][fromX].piece != Piece::EMPTY) return false;
                }
            } else {
                int step = dx > 0 ? 1 : -1;
                for (int x = fromX + step; x != toX; x += step) {
                    if (grid[fromY][x].piece != Piece::EMPTY) return false;
                }
            }
            return true;
        } else if (abs(dx) == abs(dy)) {
            int stepX = dx > 0 ? 1 : -1;
            int stepY = dy > 0 ? 1 : -1;
            for (int x = fromX + stepX, y = fromY + stepY; x != toX; x += stepX, y += stepY) {
                if (grid[y][x].piece != Piece::EMPTY) return false;
            }
            return true;
        }
        return false;
    }

    case Piece::KING: {
        if (abs(dx) <= 1 && abs(dy) <= 1) return true;
        if (!grid[fromY][fromX].hasMoved && dy == 0 && abs(dx) == 2) {
            int rookX = dx > 0 ? 7 : 0;
            if (grid[fromY][rookX].piece == Piece::ROOK && !grid[fromY][rookX].hasMoved) {
                int step = dx > 0 ? 1 : -1;
                for (int x = fromX + step; x != rookX; x += step) {
                    if (grid[fromY][x].piece != Piece::EMPTY) return false;
                }
                return true;
            }
        }
        return false;
    }

    default:
        return false;

    }
    }


    // Voer een zet uit (inclusief speciale regels)
    bool makeMove(int fromX, int fromY, int toX, int toY, Color player) {
        
        
        if (!isValidMove(fromX, fromY, toX, toY, player)) return false;
        if (grid[fromY][fromX].piece == Piece::PAWN && abs(toY - fromY) == 2) {
            // Zet een en passant doel
            enPassantTarget = {toX, toY};
        } else {
            enPassantTarget = {-1, -1}; // Reset als geen en passant
        }
        
        // En passant
    if (grid[fromY][fromX].piece == Piece::PAWN &&
        toX == enPassantTarget.first && toY == enPassantTarget.second &&
        grid[toY][toX].piece == Piece::EMPTY) {
        int capturedY = (grid[fromY][fromX].color == Color::WHITE) ? toY + 1 : toY - 1;
        grid[capturedY][toX] = {Piece::EMPTY, Color::NONE};
        }
        // Rokeren
        if (grid[fromY][fromX].piece == Piece::KING && abs(toX - fromX) == 2) {
            int rookX = toX > fromX ? 7 : 0;
            int newRookX = toX > fromX ? 5 : 3;
            grid[fromY][newRookX] = grid[fromY][rookX];
            grid[fromY][rookX] = {Piece::EMPTY, Color::NONE};
        }

        // Pionpromotie
        if (grid[fromY][fromX].piece == Piece::PAWN && (toY == 0 || toY == 7)) {
            grid[toY][toX] = {Piece::QUEEN, player}; // Altijd promoveren tot dame (voor nu)
        } else {
            grid[toY][toX] = grid[fromY][fromX];
        }

        grid[fromY][fromX] = {Piece::EMPTY, Color::NONE};
        grid[toY][toX].hasMoved = true;
        turn = (turn == Color::WHITE) ? Color::BLACK : Color::WHITE;
        return true;
    }
};


int evaluateBoard(const Board& board, Color player) {
    unordered_map<Piece, int> pieceValues = {
        {Piece::PAWN, 100}, {Piece::KNIGHT, 320}, {Piece::BISHOP, 330},
        {Piece::ROOK, 500}, {Piece::QUEEN, 900}, {Piece::KING, 20000}
    };

    int score = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const Square& sq = board.grid[y][x];
            if (sq.piece != Piece::EMPTY) {
                int val = pieceValues[sq.piece];
                score += (sq.color == player) ? val : -val;
            }
        }
    }
    return score;
}

vector<pair<pair<int, int>, pair<int, int>>> generateMoves(Board& board, Color player) {
    vector<pair<pair<int, int>, pair<int, int>>> moves;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (board.grid[y][x].color == player) {
                for (int y2 = 0; y2 < 8; ++y2) {
                    for (int x2 = 0; x2 < 8; ++x2) {
                        if (board.isValidMove(x, y, x2, y2, player)) {
                            moves.push_back({{x, y}, {x2, y2}});
                        }
                    }
                }
            }
        }
    }
    return moves;
}

Board simulateMove(Board board, int fromX, int fromY, int toX, int toY, Color player) {
    board.makeMove(fromX, fromY, toX, toY, player);
    return board;
}

int minimax(Board board, int depth, bool maximizing, Color player) {
    if (depth == 0) return evaluateBoard(board, player);

    Color current = maximizing ? player : (player == Color::WHITE ? Color::BLACK : Color::WHITE);
    auto moves = generateMoves(board, current);

    if (moves.empty()) return evaluateBoard(board, player); // Stalemate/schaakmat-situaties

    int best = maximizing ? -numeric_limits<int>::max() : numeric_limits<int>::max();

    for (auto& move : moves) {
        Board nextBoard = simulateMove(board, move.first.first, move.first.second, move.second.first, move.second.second, current);
        int score = minimax(nextBoard, depth - 1, !maximizing, player);
        best = maximizing ? max(best, score) : min(best, score);
    }

    return best;
}

// Vind beste zet voor AI
pair<pair<int, int>, pair<int, int>> findBestMove(Board& board, Color player, int depth) {
    auto moves = generateMoves(board, player);
    int bestScore = -numeric_limits<int>::max();
    pair<pair<int, int>, pair<int, int>> bestMove = moves[0];

    for (auto& move : moves) {
        Board nextBoard = simulateMove(board, move.first.first, move.first.second, move.second.first, move.second.second, player);
        int score = minimax(nextBoard, depth - 1, false, player);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    return bestMove;
}




int main() {
    Board board;
    board.printBoard();

    string kleurKeuze;
    Color spelerKleur;

    cout << "Speel je met wit of zwart? (wit/zwart): ";
    cin >> kleurKeuze;

    if (kleurKeuze == "wit") {
        spelerKleur = Color::WHITE;
    } else if (kleurKeuze == "zwart") {
        spelerKleur = Color::BLACK;
    } else {
        cout << "Ongeldige kleurkeuze. Standaard: wit.\n";
        spelerKleur = Color::WHITE;
    }

    while (true) {
        if (board.turn == spelerKleur) {
            cout << "\nAI suggestie voor jou (" << (spelerKleur == Color::WHITE ? "wit" : "zwart") << ")...\n";
            auto [besteZetVan, besteZetNaar] = findBestMove(board, spelerKleur, 3);
            cout << "Beste zet volgens AI: " << char('a' + besteZetVan.first)
                 << (8 - besteZetVan.second) << " naar "
                 << char('a' + besteZetNaar.first) << (8 - besteZetNaar.second) << "\n";

            // Speler zet
            cout << (board.turn == Color::WHITE ? "Wit" : "Zwart") << " aan zet. Voer zet in (bijv. e2e4): ";
            string move;
            cin >> move;

            if (move.size() < 4) {
                cout << "Ongeldige notatie.\n";
                continue;
            }

            int fromX = move[0] - 'a';
            int fromY = 8 - (move[1] - '0');
            int toX = move[2] - 'a';
            int toY = 8 - (move[3] - '0');

            if (!board.makeMove(fromX, fromY, toX, toY, board.turn)) {
                cout << "Ongeldige zet!\n";
                continue;
            }
            board.printBoard();
        } else {
            cout << "\nAI denkt voor " << (board.turn == Color::WHITE ? "wit" : "zwart") << "...\n";
            auto [from, to] = findBestMove(board, board.turn, 3);
            board.makeMove(from.first, from.second, to.first, to.second, board.turn);
            board.printBoard();
        }
    }
}