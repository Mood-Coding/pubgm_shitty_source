#pragma once
// <ActorName, DisplayName>
extern std::map<std::string, std::string> ActorDisplayName;


extern std::map<std::string, bool> IsItem;

// <ActorName, Color filter>
extern std::map<std::string, byte> ActorColorFilterID;

// <Color filter, Color>
extern std::map<int, unsigned int> ColorFilter;