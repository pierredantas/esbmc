#include <ld-frontend/property/yaml_property_parser.h>
#include <yaml-cpp/yaml.h>

PropertyKind YamlPropertyParser::kind_from_string(const std::string &s)
{
  if (s == "mutual_exclusion")
    return PropertyKind::mutual_exclusion;
  if (s == "invariant")
    return PropertyKind::invariant;
  if (s == "response")
    return PropertyKind::response;
  if (s == "absence")
    return PropertyKind::absence;
  if (s == "reachability")
    return PropertyKind::reachability;
  throw LdPropertyParseError("Unknown property kind: '" + s + "'");
}

// A response property names a trigger, a response and exactly one limit:
// max_scans (consecutive scans) or max_ms (milliseconds, with --ld-cycle-time).
static void
parse_response(const YAML::Node &node, LdProperty &p, const std::string &path)
{
  const std::string where = path + ": response property '" + p.id + "' ";
  if (!node["trigger"])
    throw LdPropertyParseError(where + "missing 'trigger'");
  if (!node["response"])
    throw LdPropertyParseError(where + "missing 'response'");
  if (static_cast<bool>(node["max_scans"]) == static_cast<bool>(node["max_ms"]))
    throw LdPropertyParseError(
      where + "needs exactly one of 'max_scans' and 'max_ms'");
  p.trigger = node["trigger"].as<std::string>();
  p.response_var = node["response"].as<std::string>();
  p.deadline_in_ms = static_cast<bool>(node["max_ms"]);
  if (p.deadline_in_ms)
    p.max_ms = node["max_ms"].as<unsigned>();
  else
    p.max_scans = node["max_scans"].as<unsigned>();
}

std::vector<LdProperty> YamlPropertyParser::parse(const std::string &path)
{
  YAML::Node root;
  try
  {
    root = YAML::LoadFile(path);
  }
  catch (const YAML::Exception &e)
  {
    throw LdPropertyParseError(path + ": " + e.what());
  }

  if (!root["properties"])
    throw LdPropertyParseError(path + ": missing 'properties' key");

  std::vector<LdProperty> props;

  for (const auto &node : root["properties"])
  {
    LdProperty p;

    if (!node["id"])
      throw LdPropertyParseError(path + ": property missing 'id' field");
    p.id = node["id"].as<std::string>();

    if (!node["kind"])
      throw LdPropertyParseError(
        path + ": property '" + p.id + "' missing 'kind'");
    p.kind = kind_from_string(node["kind"].as<std::string>());

    if (node["description"])
      p.description = node["description"].as<std::string>();

    if (node["justification"])
      p.justification = node["justification"].as<std::string>();

    // Validate that bounded properties carry a justification.
    if (
      (p.kind == PropertyKind::response ||
       p.kind == PropertyKind::reachability) &&
      p.justification.empty())
    {
      throw LdPropertyParseError(
        path + ": property '" + p.id + "' of kind '" +
        node["kind"].as<std::string>() + "' requires a 'justification' field");
    }

    switch (p.kind)
    {
    case PropertyKind::mutual_exclusion:
      if (!node["variables"])
        throw LdPropertyParseError(
          path + ": mutual_exclusion property '" + p.id +
          "' missing 'variables'");
      for (const auto &v : node["variables"])
        p.variables.push_back(v.as<std::string>());
      if (p.variables.size() < 2)
        throw LdPropertyParseError(
          path + ": mutual_exclusion property '" + p.id +
          "' requires at least 2 variables");
      break;

    case PropertyKind::invariant:
    case PropertyKind::absence:
    case PropertyKind::reachability:
      if (!node["expression"])
        throw LdPropertyParseError(
          path + ": property '" + p.id + "' missing 'expression'");
      p.expression = node["expression"].as<std::string>();
      break;

    case PropertyKind::response:
      parse_response(node, p, path);
      break;
    }

    props.push_back(std::move(p));
  }

  return props;
}
