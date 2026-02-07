#include "copium/ecs/Entity.h"

namespace Copium
{
  Entity::Entity()
    : manager{nullptr},
      id{INVALID_ENTITY}
  {
  }

  Entity::Entity(ECSManager* manager)
    : manager{manager},
      id{INVALID_ENTITY}
  {
  }

  Entity::Entity(ECSManager* manager, EntityId id)
    : manager{manager},
      id{id}
  {
  }

  Entity::operator EntityId() const
  {
    return id;
  }

  void Entity::operator=(EntityId entityId)
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

  bool Entity::IsValid() const
  {
    if (id == INVALID_ENTITY)
      return false;
    if (manager)
      return manager->ValidEntity(id);
    return false;
  }

  Entity::operator bool() const
  {
    return IsValid();
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

  void Entity::SetId(EntityId entityId)
  {
    id = entityId;
  }

  EntityId Entity::GetId() const
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
