#pragma once


#include "chess/board.h"
#include "chess/algo.h"
#include "chess/fen.h"
#include "chess/algo_factory.h"

#include "common/base.h"

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace space {
	
	class AlgoLinearDepthOne: public IAlgo {
	public:
		using Score = double;
		using ScorePair = std::pair<Move, Score>;
		AlgoLinearDepthOne(const std::vector<double> wtsVec);
		Move getNextMove(IBoard::Ptr board) override;

		static const std::string algoName;
		static IAlgo::Ptr createFromJson(const nlohmann::json& config);

	protected:
		std::map<PieceType, double> weights;
		Score getScore(IBoard::Ptr board);

   };

	class AlgoLinearDepthTwoExt : public IAlgo {
	public:
		using Score = double;
		using ScorePair = std::pair<Move, Score>;
		using ScoreTriple = std::tuple<Move, IBoard::Ptr, Score>;

		AlgoLinearDepthTwoExt(std::size_t _breadth, const std::vector<double>& wtsVec);
		Move getNextMove(IBoard::Ptr board) override;

		static const std::string algoName;
		static IAlgo::Ptr createFromJson(const nlohmann::json& config);

		static const std::string PawnWeightFieldName;
		static const std::string RookWeightFieldName;
		static const std::string KnightWeightFieldName;
		static const std::string BishopWeightFieldName;
		static const std::string QueenWeightFieldName;
		static const std::string BreadthFieldName;

	protected:
		std::size_t breadth;
		std::map<PieceType, double> weights;
		const Score scoreMax = 1e8;
		Score getLinearScore(IBoard::Ptr board);
		std::vector<ScoreTriple> getAllScores(IBoard::Ptr board);
		ScorePair findBestLinearMove(IBoard::Ptr board);

	};


	
}

