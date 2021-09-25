#pragma once
// ActorName -> DisplayName
extern std::unordered_map<std::string, std::string> ActorDisplayName;

// ActorName -> Color filter ID
extern std::unordered_map<std::string, byte> ActorColorFilterID;

// Color filter ID -> Color
extern std::unordered_map<int, unsigned int> ColorFilter;

// TypeSpecificID -> Display Name
extern std::unordered_map<int, std::string> DisplayName;