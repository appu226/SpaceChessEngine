#pragma once

#include "algo.h"
#include <nlohmann/json.hpp>
#include <optional>

namespace space {

	class AlgoFactory {
	public:
		using AlgoMachine = IAlgo::Ptr(*)(const nlohmann::json&);
		bool registerAlgoMachine(const std::string& algoName, AlgoMachine algoMachine);
		std::optional<IAlgo::Ptr> tryCreateAlgo(const nlohmann::json& config) const;
		std::vector<std::string> getAlgoNames() const;

		static std::string AlgoNameField;

		template<typename TAlgo>
		bool registerAlgoType()
		{
			return registerAlgoMachine(TAlgo::algoName, &TAlgo::createFromJson);
		}

	private:
		std::map<std::string, AlgoMachine> m_algoMachines;

	};

} // end namespace space
