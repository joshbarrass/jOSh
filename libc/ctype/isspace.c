#include <ctype.h>

int isspace(const int c) {
  switch (c) {
  case 0x20:
  case 0xc:
  case 0xa:
  case 0xd:
  case 0x9:
  case 0xb:
    return 1;
  }
  return 0;
}
