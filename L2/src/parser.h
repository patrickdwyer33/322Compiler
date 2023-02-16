#pragma once

#include <L2.h>

namespace Parser{
  L2::Program parse_file (char *fileName);
  L2::Program parse_spill_file(char *fileName);
  L2::Program parse_function_file(char *fileName);
}
