#pragma once
#include <chess/algo.h>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace space {
	class CliAlgo : public IAlgo {
	public:
		using Ptr = std::shared_ptr<CliAlgo>;
		Move getNextMove(IBoard::Ptr board) override;
		CliAlgo(std::istream& inputStream, std::ostream& outputStream);
		CliAlgo(const nlohmann::json& config);


		static IAlgo::Ptr create(std::istream& inputStream, std::ostream& outputStream)
		{
			return std::make_shared<CliAlgo>(inputStream, outputStream);
		}

		static IAlgo::Ptr createFromJson(const nlohmann::json& config) {
			return std::make_shared<CliAlgo>(config);
		}

		static const std::string algoName;
		static const std::string inputFileField;
		static const std::string outputFileField;

	private:
		std::ifstream m_fileInput;
		std::ofstream m_fileOutput;
		std::istream& m_inputStream;
		std::ostream& m_outputStream;

	};
}
