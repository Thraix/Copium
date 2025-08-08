#pragma once

#include <string>

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
