#include "algo_dumbo.h"
#include "algo_dumbo_impl.h"
#include <chess/board.h>
#include <chess/board_impl.h>
#include <bitset>
#include <stdexcept>
#include <set>


namespace space {

	space::AlgoDumboConfig::AlgoDumboConfig() :
		maxDepth(5),
		maxNumStates(1000 * 1000),
		maxScore(10*1000),
		pawnScore(1),
		rookScore(5),
		knightScore(3),
		bishopScore(3),
		queenScore(8),
		validMoveScore(1)
	{ }

	space::AlgoDumboConfig::AlgoDumboConfig(const nlohmann::json& config)
	{
		maxDepth = config.value(AlgoDumbo::MaxDepthFieldName, 5);
		maxNumStates = config.value(AlgoDumbo::MaxNumStatesFieldName, 1000 * 1000.0);
		maxScore = config.value(AlgoDumbo::MaxScoreFieldName, 10 * 1000.0);
		pawnScore = config.value(AlgoDumbo::PawnScoreFieldName, 1.0);
		rookScore = config.value(AlgoDumbo::RookScoreFieldName, 5.0);
		knightScore = config.value(AlgoDumbo::KnightScoreFieldName, 3.0);
		bishopScore = config.value(AlgoDumbo::BishopScoreFieldName, 3.0);
		queenScore = config.value(AlgoDumbo::QueenScoreFieldName, 8.0);
		validMoveScore = config.value(AlgoDumbo::ValidMoveScoreFieldName, 1.0);
	}


	Move AlgoDumbo::getNextMove(IBoard::Ptr board) {

		using namespace algo_dumbo_impl;

		if (board->isCheckMate())
			throw std::runtime_error("Cannot find next move for a board on check mate.");
		if (board->isStaleMate())
			throw std::runtime_error("Cannot find next move for a board on stale mate.");

		std::vector<std::pair<Move, State> > movesAndStates;
		StateScores stateScores;
		StateSet stateSet;
		
		for (const auto& moveXboard: board->getValidMoves()) {
			State state = boardToState(*moveXboard.second);
			movesAndStates.push_back(std::make_pair(moveXboard.first, state));
			addState(stateScores, stateSet, state, m_config.maxDepth);
		}

		if (movesAndStates.empty())
			throw std::runtime_error("Assertion error: if no checkmate and stalemate, then there should have been valid moves.");

		exploreStates(
				stateScores,
				stateSet,
				0,
				board->whoPlaysNext(),
				m_config);

		double bestScore = getScore(stateScores, movesAndStates.front().second, 0);
		Move bestMove = movesAndStates.front().first;
		Comparator scoreIsLess = getComparatorForColor(board->whoPlaysNext());
		for (const auto & moveAndState: movesAndStates)
		{
			double score = getScore(stateScores, moveAndState.second, 0);
			if (scoreIsLess(bestScore, score))
			{
				bestScore = score;
				bestMove = moveAndState.first;
			}
		}
		
		return bestMove;

	}

	const std::string AlgoDumbo::algoName = "AlgoDumbo";
	const std::string AlgoDumbo::MaxDepthFieldName = "MaxDepth";
	const std::string AlgoDumbo::MaxNumStatesFieldName = "MaxNumStates";
	const std::string AlgoDumbo::MaxScoreFieldName = "MaxScore";
	const std::string AlgoDumbo::PawnScoreFieldName = "PawnScore";
	const std::string AlgoDumbo::RookScoreFieldName = "RookScore";
	const std::string AlgoDumbo::KnightScoreFieldName = "KnightScore";
	const std::string AlgoDumbo::BishopScoreFieldName = "BishopScore";
	const std::string AlgoDumbo::QueenScoreFieldName = "QueenScore";
	const std::string AlgoDumbo::ValidMoveScoreFieldName = "ValidMoveScore";
	IAlgo::Ptr AlgoDumbo::createFromJson(const nlohmann::json& config)
	{
		return std::make_shared<AlgoDumbo>(AlgoDumboConfig(config));
	}


} // end namespace space






