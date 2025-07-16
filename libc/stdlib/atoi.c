#include <stdlib.h>

long long int atoll(const char *s) {

}

long int atol(const char *s) {
  return (long int)atoll(s);
}

int atoi(const char *s) {
  return (int)atoll(s);
}
