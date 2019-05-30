#include "Variables.hpp"
#include "../EnergyPlus/InputProcessing/IdfParser.hh"
#include "../EnergyPlus/InputProcessing/EmbeddedEpJSONSchema.hh"
#include "../EnergyPlus/DataStringGlobals.hh"
#include <iostream>
#include <fstream>

using json = nlohmann::json;

namespace EnergyPlus {
namespace FMI {

std::vector<std::string> zoneNames(const std::string & idfPath) {
  std::vector<std::string> result;

  std::ifstream input_stream(idfPath, std::ifstream::in);

  std::string input_file;
  std::string line;
  while (std::getline(input_stream, line)) {
    input_file.append(line + EnergyPlus::DataStringGlobals::NL);
  }
  
  IdfParser parser;
  const auto embeddedEpJSONSchema = EnergyPlus::EmbeddedEpJSONSchema::embeddedEpJSONSchema();
  const auto schema = json::from_cbor(embeddedEpJSONSchema.first, embeddedEpJSONSchema.second);
  const auto jsonidf = parser.decode(input_file, schema);

  const auto zones = jsonidf["Zone"];
  for( const auto & zone : zones.items() ) {
    result.push_back(zone.key());
  }

  std::sort(result.begin(), result.end());

  return result;
}

std::map<unsigned int, Variable> parseVariables(const std::string & idf) {
  std::map<unsigned int, Variable> result;

  const auto zones = zoneNames(idf);

  unsigned int i = 0;
  for (const auto & zone : zones) {
    {
      Variable var;
      var.type = VariableType::T;
      var.key = zone;

      var.scalar_attributes.emplace_back(std::make_pair("name",zone + "_T"));
      var.scalar_attributes.emplace_back(std::make_pair("valueReference", std::to_string(i)));
      var.scalar_attributes.emplace_back(std::make_pair("description","Temperature of the zone air"));
      var.scalar_attributes.emplace_back(std::make_pair("causality","input"));
      var.scalar_attributes.emplace_back(std::make_pair("variability","continuous"));

      var.real_attributes.emplace_back(std::make_pair("quantity","ThermodynamicTemperature"));
      var.real_attributes.emplace_back(std::make_pair("unit","degC"));
      var.real_attributes.emplace_back(std::make_pair("relativeQuantity","false"));
      var.real_attributes.emplace_back(std::make_pair("start","0.0"));

      result.emplace(i,std::move(var));
    }
    ++i;
    {
      Variable var;
      var.type = VariableType::T;
      var.key = zone;

      var.scalar_attributes.emplace_back(std::make_pair("name",zone + "_QConSen_flow"));
      var.scalar_attributes.emplace_back(std::make_pair("valueReference", std::to_string(i)));
      var.scalar_attributes.emplace_back(std::make_pair("description","Convective sensible heat added to the zone"));
      var.scalar_attributes.emplace_back(std::make_pair("causality","output"));
      var.scalar_attributes.emplace_back(std::make_pair("variability","continuous"));
      var.scalar_attributes.emplace_back(std::make_pair("initial","calculated"));

      var.real_attributes.emplace_back(std::make_pair("quantity","Power"));
      var.real_attributes.emplace_back(std::make_pair("unit","W"));
      var.real_attributes.emplace_back(std::make_pair("relativeQuantity","false"));

      result.emplace(i,std::move(var));
    }
    ++i;
    {
      Variable var;
      var.type = VariableType::AFLO;
      var.key = zone;

      var.scalar_attributes.emplace_back(std::make_pair("name",zone + "_AFlo"));
      var.scalar_attributes.emplace_back(std::make_pair("valueReference", std::to_string(i)));
      var.scalar_attributes.emplace_back(std::make_pair("description","Floor area"));
      var.scalar_attributes.emplace_back(std::make_pair("causality","local"));
      var.scalar_attributes.emplace_back(std::make_pair("variability","constant"));
      var.scalar_attributes.emplace_back(std::make_pair("initial","exact"));

      var.real_attributes.emplace_back(std::make_pair("quantity","Area"));
      var.real_attributes.emplace_back(std::make_pair("unit","m2"));
      var.real_attributes.emplace_back(std::make_pair("relativeQuantity","false"));
      var.real_attributes.emplace_back(std::make_pair("start","12.0"));

      result.emplace(i,std::move(var));
    }
    ++i;
    {
      Variable var;
      var.type = VariableType::V;
      var.key = zone;

      var.scalar_attributes.emplace_back(std::make_pair("name",zone + "_V"));
      var.scalar_attributes.emplace_back(std::make_pair("valueReference", std::to_string(i)));
      var.scalar_attributes.emplace_back(std::make_pair("description","Volume"));
      var.scalar_attributes.emplace_back(std::make_pair("causality","local"));
      var.scalar_attributes.emplace_back(std::make_pair("variability","constant"));
      var.scalar_attributes.emplace_back(std::make_pair("initial","exact"));

      var.real_attributes.emplace_back(std::make_pair("quantity","Volume"));
      var.real_attributes.emplace_back(std::make_pair("unit","m3"));
      var.real_attributes.emplace_back(std::make_pair("relativeQuantity","false"));
      var.real_attributes.emplace_back(std::make_pair("start","36.0"));

      result.emplace(i,std::move(var));
    }
    ++i;
    {
      Variable var;
      var.type = VariableType::MSENFAC;
      var.key = zone;

      var.scalar_attributes.emplace_back(std::make_pair("name",zone + "_mSenFac"));
      var.scalar_attributes.emplace_back(std::make_pair("valueReference", std::to_string(i)));
      var.scalar_attributes.emplace_back(std::make_pair("description","Factor for scaling sensible thermal mass of volume"));
      var.scalar_attributes.emplace_back(std::make_pair("causality","local"));
      var.scalar_attributes.emplace_back(std::make_pair("variability","constant"));
      var.scalar_attributes.emplace_back(std::make_pair("initial","exact"));

      var.real_attributes.emplace_back(std::make_pair("relativeQuantity","false"));
      var.real_attributes.emplace_back(std::make_pair("start","1.0"));

      result.emplace(i,std::move(var));
    }
    ++i;
  }

  return result;
}

}
}

