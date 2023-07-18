#pragma once

#include "copium/ecs/Entity.h"
#include "copium/util/MetaFile.h"
#include "copium/util/Uuid.h"

#include <glm/glm.hpp>

#include <string>

namespace Copium
{
  class ComponentHandlerBase
  {
  public:
    virtual void Serialize(Entity entity, MetaFile& metaFile) = 0;
    virtual void Deserialize(Entity entity, const MetaFileClass& metaClass) = 0;
    virtual void ComponentGui(Entity entity) = 0;
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetSerializedName() const = 0;

  protected:
    glm::vec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::vec2 vec);
    glm::ivec2 ReadVec2Opt(const MetaFileClass& metaClass, const std::string& key, glm::ivec2 vec);
    bool ReadBoolOpt(const MetaFileClass& metaClass, const std::string& key, bool vec);

    // TODO: I don't like this implementation at all.
    //       It shouldn't be the ComponentHandlerBases responsibility to know about entities
    //       But for now I don't know how to solve it in a cleaner way
    Entity GetEntity(ECSManager* ecs, const Uuid& uuid) const;
  };
}