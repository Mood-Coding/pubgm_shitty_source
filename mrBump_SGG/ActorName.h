#pragma once
// <ActorName, DisplayName>
//extern std::map<std::string, std::string> ActorDisplayName;
extern std::unordered_map<std::string, std::string> ActorDisplayName;

// <ActorName, Color filter>
//extern std::map<std::string, byte> ActorColorFilterID;
extern std::unordered_map<std::string, byte> ActorColorFilterID;

// <Color filter, Color>
//extern std::map<int, unsigned int> ColorFilter;
extern std::unordered_map<int, unsigned int> ColorFilter;

// int <-> Display Name
extern std::unordered_map<int, std::string> TypeID;