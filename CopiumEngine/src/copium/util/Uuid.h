#pragma once

#include <stdint.h>

#include <iostream>
#include <random>

namespace Copium
{
  class Uuid
  {
  private:
    uint64_t msb;
    uint64_t lsb;
    static std::random_device randomDevice;
    static std::mt19937 randomGenerator;
    static std::uniform_int_distribution<uint64_t> randomDistribution;

  public:
    Uuid();
    Uuid(uint64_t msb, uint64_t lsb);
    // Format: xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
    Uuid(const std::string& uuidString);

    std::string ToString() const;

    bool operator==(const Uuid& rhs) const;
    bool operator!=(const Uuid& rhs) const;
    bool operator<(const Uuid& rhs) const;

    friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid);

  private:
    uint8_t HexToDec(char c) const;
    char DecToHex(uint8_t byte) const;
  };
}
