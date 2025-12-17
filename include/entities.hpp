#pragma once
#include <string>
#include <iostream>

enum NPCType {
    KhightType,
    DragonType,
    PegasType,
    Unknown
};

std::string typeToString(NPCType t);

std::istream& operator>>(std::istream& is, NPCType& t);
