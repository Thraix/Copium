#include "copium/util/Uuid.h"

#include "copium/util/Common.h"

namespace Copium
{

  std::random_device Uuid::randomDevice{};
  std::mt19937 Uuid::randomGenerator{randomDevice()};
  std::uniform_int_distribution<uint64_t> Uuid::randomDistribution{std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()};

  Uuid::Uuid()
    : msb{randomDistribution(randomGenerator)}, lsb{randomDistribution(randomGenerator)}
  {}

  Uuid::Uuid(uint64_t msb, uint64_t lsb)
    : msb{msb}, lsb{lsb}
  {}

  Uuid::Uuid(const std::string& uuidString)
    : msb{0}, lsb{0}
  {
    CP_ASSERT(uuidString.size() == 36, "Invalid Uuid string size: %s", uuidString.c_str());
    for (int i = 0; i < 18; i++)
    {
      if (i == 8 || i == 13) // skip "-"
        continue;
      msb <<= 4;
      msb |= HexToDec(uuidString[i]);
    }
    for (int i = 19; i < 36; i++)
    {
      if (i == 23) // skip "-"
        continue;
      lsb <<= 4;
      lsb |= HexToDec(uuidString[i]);
    }
  }

  std::string Uuid::ToString() const 
  {
    std::string string;
    string.reserve(36);
    for (int i = 0; i < 16; i++)
    {
      if (i == 8 || i == 12) string.push_back('-');
      string.push_back(DecToHex((msb >> (60 - i * 4)) & 0xf));
    }
    string.push_back('-');
    for (int i = 0; i < 16; i++)
    {
      if (i == 4) string.push_back('-');
      string.push_back(DecToHex((lsb >> (60 - i * 4)) & 0xf));
    }
    return string;
  }

  bool Uuid::operator==(const Uuid& rhs)
  {
    return msb == rhs.msb && lsb == rhs.lsb;
  }

  bool Uuid::operator!=(const Uuid& rhs)
  {
    return !(*this == rhs);
  }

  bool Uuid::operator<(const Uuid& rhs)
  {
    if (msb != rhs.msb)
      return msb < rhs.msb;
    return lsb < rhs.lsb;
  }

  std::ostream& operator<<(std::ostream& os, const Uuid& uuid)
  {
    return os << uuid.ToString();
  }

  uint8_t Uuid::HexToDec(char c) const
  {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    CP_ABORT("Invalid char value: %c (%d)", c, (int)c);
  }

  char Uuid::DecToHex(uint8_t nibble) const
  {
    if (nibble >= 0 && nibble <= 9) return '0' + nibble;
    if (nibble >= 10 && nibble <= 15) return 'a' + nibble - 10;
    CP_ABORT("Invalid nibble value: %d", (int)nibble);
  }
}
