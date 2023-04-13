#pragma once

#include <stdexcept>

namespace Copium
{
	class RuntimeException
	{
  private:
    std::string errorMessage;
	public:
    RuntimeException(const std::string& str);

    const std::string& GetErrorMessage() const;
	};
}