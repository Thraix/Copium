#include "copium/ecs/Entity.h"

namespace Copium
{
  Entity::Entity()
    : manager{nullptr}, id{INVALID_ENTITY}
  {}

  Entity::Entity(ECSManager* manager)
    : manager{manager}, id{INVALID_ENTITY}
  {}

  Entity::Entity(ECSManager* manager, EntityID id)
    : manager{manager}, id{id}
  {}

  Entity::operator EntityID() const
  {
    return id;
  }

  void Entity::operator=(EntityID entityId)
  {
    id = entityId;
  }

  bool Entity::operator==(const Entity& entity)
  {
    return id == entity.id;
  }

  bool Entity::operator!=(const Entity& entity)
  {
    return id != entity.id;
  }

  Entity::operator bool() const
  {
    if (id == INVALID_ENTITY)
      return false;
    if (manager)
      return manager->ValidEntity(id);
    return false;
  }

  void Entity::Invalidate()
  {
    id = INVALID_ENTITY;
  }

  void Entity::Destroy()
  {
    if (*this)
      manager->DestroyEntity(id);
  }

  void Entity::SetID(EntityID aId)
  {
    id = aId;
  }

  EntityID Entity::GetID() const
  {
    return id;
  }

  ECSManager* Entity::GetManager() const
  {
    return manager;
  }

  Entity Entity::Create(ECSManager* manager)
  {
    return {manager, manager->CreateEntity()};
  }
}