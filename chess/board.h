#pragma once

#include <memory>
#include <optional>
#include <map>

namespace space {
	enum class Color { White, Black };
	struct Position {
		int rank;
		int file;
	};
	struct Move {
		int sourceRank;
		int sourceFile;
		int destinationRank;
		int destinationFile;
	};
	enum class PieceType { Pawn, EnPessantCapturablePawn, Rook, Knight, Bishop, Queen, King, None };
	struct Piece {
		PieceType pieceType;
		Color color;
	};
	class IBoard {
	public:
		using Ptr = std::shared_ptr<IBoard>;
		using uint = std::size_t;
		virtual Color whoPlaysNext() const = 0;
		virtual std::optional<Piece> getPiece(Position position) const = 0;
		
		// (Left and right from that player's point of view)
		virtual bool canCastleLeft(Color color) const = 0;
		virtual bool canCastleRight(Color color) const = 0;

		virtual bool isStaleMate() const = 0;
		virtual bool isCheckMate() const = 0;
		virtual std::optional<Ptr> updateBoard(Move move) const = 0;
		virtual std::map<Move, Ptr> getPossibleMoves() const = 0;
	};

}
