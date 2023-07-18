#include "copium/example/ComponentHandlerBase.h"

#include "copium/example/Components.h"

namespace Copium
{
  glm::vec2 ComponentHandlerBase::ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::vec2 vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    char* endPos;
    vec.x = std::strtof(value.c_str(), &endPos);
    vec.y = std::strtof(endPos, &endPos);
    return vec;
  }

  glm::ivec2 ComponentHandlerBase::ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::ivec2 vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    char* endPos;
    vec.x = std::strtof(value.c_str(), &endPos);
    vec.y = std::strtof(endPos, &endPos);
    return vec;
  }


  bool ComponentHandlerBase::ReadBoolOpt(const MetaFileClass& metaClass, const std::string& key, bool vec)
  {
    if (!metaClass.HasValue(key))
      return vec;

    const std::string& value = metaClass.GetValue(key);
    if (value == "true")
      return true;
    return false;
  }

  Entity ComponentHandlerBase::GetEntity(ECSManager* ecs, const Uuid& uuid) const
  {
    Entity entity{ecs, ecs->Find<UuidC>([&](EntityId entity, const UuidC& uuidArg) { return uuid == uuidArg.uuid; })};
    CP_ASSERT(entity, "Failed to find entity with Uuid=%s", uuid.ToString().c_str());

    return entity;
  }
}
