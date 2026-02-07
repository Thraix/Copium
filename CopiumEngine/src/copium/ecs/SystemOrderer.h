#pragma once

#include <typeindex>
#include <vector>

namespace Copium
{
  class SystemPool;

  class SystemOrderer
  {
  public:
    SystemOrderer(std::type_index systemId, SystemPool* systemPool);

    template <typename System>
    void Before()
    {
      orderQueue.emplace_back(OrderOperation::Before, typeid(System));
    }

    template <typename System>
    void After()
    {
      orderQueue.emplace_back(OrderOperation::After, typeid(System));
    }

  private:
    enum class OrderOperation
    {
      Before,
      After
    };

    std::type_index systemId;
    SystemPool* systemPool = nullptr;
    std::vector<std::pair<OrderOperation, std::type_index>> orderQueue;

  private:
    friend class SystemPool;

    void CommitOrdering();
    void CommitBefore(const std::type_index& otherSystemId);
    void CommitAfter(const std::type_index& otherSystemId);
  };
}
