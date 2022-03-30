// caduinfo - given a CADU stream on stdin, prints information about its contents

#include <iostream>
#include "libcadu/libcadu.h"

int main() {
  CADU cadu;
  while (randomised::operator>>(std::cin, cadu)) {
    nonrandomised::operator<<(std::cout, cadu);
  }
}
