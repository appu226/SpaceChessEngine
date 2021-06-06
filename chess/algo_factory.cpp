#include "algo_factory.h"


namespace space {

	std::string AlgoFactory::AlgoNameField = "AlgoName";

	bool AlgoFactory::registerAlgoMachine(const std::string& algoName, AlgoMachine algoMachine)
	{
		return m_algoMachines.emplace(algoName, algoMachine).second;
	}

	std::optional<IAlgo::Ptr> AlgoFactory::tryCreateAlgo(const nlohmann::json& config) const
	{
		auto algoNameIt = config.find(AlgoNameField);
		if (config.end() == algoNameIt)
			return std::optional<IAlgo::Ptr>();
		auto algoName = algoNameIt->get<std::string>();
		auto algoIt = m_algoMachines.find(algoName);
		if (algoIt == m_algoMachines.end())
			return std::optional<IAlgo::Ptr>();
		return (*algoIt->second)(config);
	}

	std::vector<std::string> AlgoFactory::getAlgoNames() const
	{
		std::vector<std::string> result;
		result.reserve(m_algoMachines.size());
		for (const auto & kv: m_algoMachines)
			result.push_back(kv.first);
		return result;
	}

} // end namespace space
