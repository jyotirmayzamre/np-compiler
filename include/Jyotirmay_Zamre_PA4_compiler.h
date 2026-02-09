#ifndef COMPILER_H
#define COMPILER_H


#include "opcodes.h"
#include "types.h"
#include "symbol.h"
#include "symbol_table.h"
#include "quad.h"
#include "quad_array.h"
#include "asm_emitter.h"


#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>

void cleanup(map<string, vector<Quad*>>& funcQuads);

#endif