#include <iostream>
#include <string>
#include <vector>

#include <chess/board.h>
#include <chess/board_impl.h>

#include <chess/algo_factory.h>
#include <algo_linear/algoLinear.h>
#include <algo_linear/algo_dumbo.h>

#include "CliAlgo.h"

namespace {


	static std::string WhiteAlgoFieldName = "WhiteAlgo";
	static std::string BlackAlgoFieldName = "BlackAlgo";
	static std::string TerminalColorsFieldName = "TerminalColors";
	static std::string UnicodeFieldName = "Unicode";

	inline std::string getColorName(space::Color color)
	{
		switch (color)
		{
		case space::Color::White:
			return "White";
		case space::Color::Black:
			return "Black";
		default:
			throw std::runtime_error("Unrecognized color value " + std::to_string(static_cast<int>(color)));
		}
	}

	inline space::Color getOppositeColor(space::Color color) {
		switch (color)
		{
		case space::Color::Black:
			return space::Color::White;
		case space::Color::White:
			return space::Color::Black;
		default:
			throw std::runtime_error("Unrecognized color value " + std::to_string(static_cast<int>(color)));
		}
	}

	nlohmann::json parseConfig(int argc, char const* const* const argv, space::AlgoFactory& algoFactory)
	{
		algoFactory.registerAlgoType<space::CliAlgo>();
		algoFactory.registerAlgoType<space::AlgoLinearDepthTwoExt>();
		algoFactory.registerAlgoType<space::AlgoDumbo>();
		nlohmann::json result;
		for (int iarg = 1; iarg < argc; ++iarg)
		{
			std::string arg = argv[iarg];
			if (arg == "--configFile")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--configFile'");
				std::ifstream fin(argv[iarg]);
				fin >> result;
			}
			else if (arg == "--blackAlgo")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--blackAlgo'");
				result[BlackAlgoFieldName][space::AlgoFactory::AlgoNameField] = argv[iarg];
			}
			else if (arg == "--whiteAlgo")
			{
				if (++iarg >= argc)
					throw std::runtime_error("invalid command line arguments: expected filename after '--whiteAlgo'");
				result[WhiteAlgoFieldName][space::AlgoFactory::AlgoNameField] = argv[iarg];
			}
			else if (arg == "--color")
			{
				result[TerminalColorsFieldName] = true;
			}
			else if (arg == "--unicode")
			{
				result[UnicodeFieldName] = true;
			}
			else if (arg == "--help" || arg == "-h")
			{
				std::cout << "Space chess command line game engine.\n\t"
					<< argv[0] << " [--configFile <json config file>]" 
					<< "\n\t\t[--blackAlgo <blackAlgoName>] [--whiteAlgo <whiteAlgoName>]"
					<< "\n\t\t[--color] [--unicode]"
					<< "\n\t\t[--help|-h]"
					<< std::endl;
				std::exit(0);
			}

		}
		if (!result.contains(BlackAlgoFieldName))
			result[BlackAlgoFieldName][space::AlgoFactory::AlgoNameField] = space::CliAlgo::algoName;
		if (!result.contains(WhiteAlgoFieldName))
			result[WhiteAlgoFieldName][space::AlgoFactory::AlgoNameField] = space::AlgoLinearDepthTwoExt::algoName;
		return result;
	}

	struct Config {
		bool terminal_colors;
		bool unicode;
	};
}

int main(int argc, char const * const * const argv) {
	auto board = space::BoardImpl::getStartingBoard();
	space::AlgoFactory algoFactory;
	nlohmann::json config = parseConfig(argc, argv, algoFactory);
	auto blackAlgo = algoFactory.tryCreateAlgo(config[BlackAlgoFieldName]).value();
	auto whiteAlgo = algoFactory.tryCreateAlgo(config[WhiteAlgoFieldName]).value();

	auto terminal_colors = config.contains(TerminalColorsFieldName);
	auto unicode = config.contains(UnicodeFieldName);

	bool recursiveError = false;
	int moveCounter = 0;

	while (true)
	{   
		std::cout 
			<< "#" << moveCounter 
			<<  "  " << getColorName(board->whoPlaysNext()) << " to play"
			<< std::endl;
		std::cout << board->as_string(terminal_colors, unicode, space::Color::White);
		auto algo = board->whoPlaysNext() == space::Color::White ? whiteAlgo : blackAlgo;
		try {

			if (board->isCheckMate()) {
				std::cout
					<< "Check mate!\n"
					<< getColorName(getOppositeColor(board->whoPlaysNext())) << " wins!"
					<< std::endl;
				return 0;
			}
			if (board->isStaleMate()) {
				std::cout
					<< getColorName(board->whoPlaysNext()) << " is stuck on stale mate!\n"
					<< "It's a draw!"
					<< std::endl;
				return 0;
			}

			auto nextMove = algo->getNextMove(board);
			auto validMoves = board->getValidMoves();
			auto validMoveIt = validMoves.find(nextMove);
			if (validMoveIt == validMoves.cend())
				throw std::runtime_error(getColorName(board->whoPlaysNext()) + " played invalid move.");
			std::cout 
				<< "Move: "
				<< space::moveToString(nextMove, board) 
				<< std::endl;
			board = validMoveIt->second;
			++moveCounter;
		}
		catch (const std::bad_alloc&)
		{
			throw;
		}
		catch (const std::exception& e)
		{
			std::cout << "Error: " << e.what();
			if (recursiveError)
				throw;
			recursiveError = true;
		}
		recursiveError = false;
	}
	return 0;
}
