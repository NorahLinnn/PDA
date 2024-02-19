#ifndef PRINTUTILS_H
#define PRINTUTILS_H

#include "../Parser/Parser.h"

void printLibraryCell(const LibCell& cell);
void printTechnology(const Tech& tech);
void printCell(const Cell& cell);
void printNet(const Net& net);
void printDie(const Die& die);
void printParsedData(const ParsedData& data);
void printmincut(const ParsedData& data) ;


#endif
