#pragma once


#include <chess/algo.h>
#include <nlohmann/json.hpp>


namespace space {

	struct AlgoDumboConfig {
		int maxDepth;
		int maxNumStates;
		double maxScore;
		double pawnScore;
		double rookScore;
		double knightScore;
		double bishopScore;
		double queenScore;
		double validMoveScore;
		AlgoDumboConfig();
		AlgoDumboConfig(const nlohmann::json& config);
	};

	class AlgoDumbo: public IAlgo {
		public:
			using Ptr = std::shared_ptr<AlgoDumbo>;
			Move getNextMove(IBoard::Ptr board) override;

			AlgoDumbo(const AlgoDumboConfig& config): m_config(config) {}

			static const std::string algoName;
			static const std::string MaxDepthFieldName;
			static const std::string MaxNumStatesFieldName;
			static const std::string MaxScoreFieldName;
			static const std::string PawnScoreFieldName;
			static const std::string RookScoreFieldName;
			static const std::string KnightScoreFieldName;
			static const std::string BishopScoreFieldName;
			static const std::string QueenScoreFieldName;
			static const std::string ValidMoveScoreFieldName;
			static IAlgo::Ptr createFromJson(const nlohmann::json& config);

		private:
			AlgoDumboConfig m_config;

	};


} // end namespace space

