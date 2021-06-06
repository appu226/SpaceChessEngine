#include "algoLinear.h"

#include <common/base.h>
#include <chess/board.h>

#include <iostream>
#include <vector>
#include <stdexcept>
#include <string>
#include <algorithm>

namespace space {

	

	AlgoLinearDepthOne::AlgoLinearDepthOne(const std::vector<double> wtsVec)
	{
		space_assert(wtsVec.size() == 5,
			"Need 5 weights for algo");
		this->weights[PieceType::Pawn] = wtsVec[0];
		this->weights[PieceType::Rook] = wtsVec[1];
		this->weights[PieceType::Knight] = wtsVec[2];
		this->weights[PieceType::Bishop] = wtsVec[3];
		this->weights[PieceType::Queen] = wtsVec[4];
	}

	Move AlgoLinearDepthOne::getNextMove(IBoard::Ptr board)
	{
		 
		IBoard::MoveMap moveMap = board->getValidMoves();

		if (moveMap.size() == 0) {
			return Move();
		}

		bool high = board->whoPlaysNext() == Color::White;
		std::map<Move, Score> allScores;
		for (const auto& mb : moveMap) {
			allScores[mb.first] = this->getScore(mb.second);
		}

		auto cmp = [high](const ScorePair& p1, const ScorePair& p2) {
			return (high ? p1.second < p2.second : p1.second > p1.second);
		};

		auto best = *std::max_element(allScores.begin(), allScores.end(), cmp);

		Move bestMove = best.first;
		
		return bestMove;

	}

	const std::string AlgoLinearDepthOne::algoName = "AlgoLinearDepthOne";
	IAlgo::Ptr AlgoLinearDepthOne::createFromJson(const nlohmann::json& config)
	{
		throw std::runtime_error("AlgoLinearDepthOne::createFromJson not yet implemented");
	}

	AlgoLinearDepthOne::Score AlgoLinearDepthOne::getScore(IBoard::Ptr board)
	{
		auto signum = [](Color c) { return c == Color::White ? 1 : -1; };
		Score s = 0;

		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> p = board->getPiece({ i, j });
				if (p.has_value()) {
					s += signum(p.value().color) * this->weights[p.value().pieceType];
				}
			}
		return s;
	}



// Depth Two algos


	AlgoLinearDepthTwoExt::AlgoLinearDepthTwoExt(std::size_t _breadth, const std::vector<double>& wtsVec)
	{
		space_assert(_breadth > 0, "Breadth must be positive");
		space_assert(wtsVec.size() == 5, "Need 5 weights");
		this->breadth = _breadth;
		this->weights[PieceType::Pawn] = wtsVec[0];
		this->weights[PieceType::Rook] = wtsVec[1];
		this->weights[PieceType::Knight] = wtsVec[2];
		this->weights[PieceType::Bishop] = wtsVec[3];
		this->weights[PieceType::Queen] = wtsVec[4];		
	}


	AlgoLinearDepthTwoExt::Score AlgoLinearDepthTwoExt::getLinearScore(IBoard::Ptr board)
	{
		Score s = 0;
		for (int i = 0; i < 8; i++)
			for (int j = 0; j < 8; j++) {
				std::optional<Piece> pMaybe = board->getPiece({ i, j });
				if (!pMaybe.has_value()) {
					continue;
				}
				Piece p = pMaybe.value();
				int sign = p.color == Color::White ? 1 : -1;
				if (p.pieceType == PieceType::Pawn) {
					s += sign * (1 + this->weights[p.pieceType] * (sign == 1 ? (i - 1) : (6 - i)));
				}
				else {
					s += sign * this->weights[p.pieceType];
				}
			}
		return s;
	}

	std::vector<AlgoLinearDepthTwoExt::ScoreTriple> AlgoLinearDepthTwoExt::getAllScores(IBoard::Ptr board)
	{
		IBoard::MoveMap moveMap = board->getValidMoves();
		std::vector<ScoreTriple> allScores;
		for (const auto& mb : moveMap) {
			allScores.push_back(std::make_tuple(mb.first,
				mb.second,
				this->getLinearScore(mb.second)));
		}

		return allScores;
	}

	AlgoLinearDepthTwoExt::ScorePair AlgoLinearDepthTwoExt::findBestLinearMove(IBoard::Ptr board)
	{
		std::vector<ScoreTriple> allScores = this->getAllScores(board);
		int direction = colorToSign(board->whoPlaysNext());;

		if (allScores.size() == 0) {
			return std::make_pair(Move(), AlgoLinearDepthTwoExt::scoreMax * direction * -1.0 );
		}

		auto cmp = [direction](const ScoreTriple& a, const ScoreTriple& b) {
			return direction * (std::get<2>(a) - std::get<2>(b)) < 0;
		};

		ScoreTriple best = *std::max_element(allScores.begin(),
			allScores.end(),
			cmp);

		return std::make_pair(std::get<0>(best), std::get<2>(best));
	}



	Move AlgoLinearDepthTwoExt::getNextMove(IBoard::Ptr board)
	{
		std::vector<ScoreTriple> allScores = this->getAllScores(board);

		if (allScores.size() == 0) {
			return Move();
		}

		int direction = colorToSign(board->whoPlaysNext());

		auto cmp = [direction](const ScoreTriple& a, const ScoreTriple& b) {
			return direction * (std::get<2>(a) - std::get<2>(b)) > 0;
		}; // a better than b


		std::sort(allScores.begin(), allScores.end(), cmp); // better moves first
		allScores.resize(std::min(allScores.size(), this->breadth));

		std::map<Move, Score> allScores2;
		for (const auto& v : allScores) {
			ScorePair bestDepth2 = this->findBestLinearMove(std::get<1>(v));
			allScores2[std::get<0>(v)] = std::get<1>(bestDepth2);
		}

		auto cmp2 = [direction](const ScorePair& p1, const ScorePair& p2) {
			return direction * (p1.second - p2.second) < 0;
		};// <

		auto best = *std::max_element(allScores2.begin(), allScores2.end(), cmp2);

		Move bestMove = best.first;		

		return bestMove;

	}

	const std::string AlgoLinearDepthTwoExt::algoName = "AlgoLinearDepthTwoExt";
	const std::string AlgoLinearDepthTwoExt::PawnWeightFieldName = "PawnWeight";
	const std::string AlgoLinearDepthTwoExt::RookWeightFieldName = "RookWeight";
	const std::string AlgoLinearDepthTwoExt::KnightWeightFieldName = "KnightWeight";
	const std::string AlgoLinearDepthTwoExt::BishopWeightFieldName = "BishopWeight";
	const std::string AlgoLinearDepthTwoExt::QueenWeightFieldName = "QueenWeight";
	const std::string AlgoLinearDepthTwoExt::BreadthFieldName = "Breadth";
	IAlgo::Ptr AlgoLinearDepthTwoExt::createFromJson(const nlohmann::json& config)
	{
		double pawnWt = config.value(PawnWeightFieldName, 1.0);
		double rookWt = config.value(RookWeightFieldName, 9.0);
		double knightWt = config.value(KnightWeightFieldName, 7.0);
		double bishopWt = config.value(BishopWeightFieldName, 7.0);
		double queenWt = config.value(QueenWeightFieldName, 15.0);
		int breadth = config.value(BreadthFieldName, 6);

		std::vector<double> wts = { pawnWt, rookWt, knightWt, bishopWt, queenWt };
		return std::make_shared<AlgoLinearDepthTwoExt>(breadth, wts);
	}

}
