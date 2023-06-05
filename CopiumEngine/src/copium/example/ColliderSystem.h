#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/example/CollideSignal.h"

#include <queue>

namespace Copium
{
  class ColliderSystem : public System<DynamicColliderC, TransformC>
  {
    std::queue<CollideSignal> signals;

    bool Overlap(TransformC& firstTransform, TransformC& secondTransform)
    {
      if (firstTransform.position.x >= secondTransform.position.x + secondTransform.size.x)
        return false;
      if (firstTransform.position.y >= secondTransform.position.y + secondTransform.size.y)
        return false;
      if (firstTransform.position.x + firstTransform.size.x <= secondTransform.position.x)
        return false;
      if (firstTransform.position.y + firstTransform.size.y <= secondTransform.position.y)
        return false;
      return true;
    }

    void CollideCheckDynamic(Entity firstEntity, TransformC& firstTransform, Entity secondEntity, TransformC& secondTransform)
    {
      if (firstEntity.GetId() >= secondEntity.GetId())
        return;
      if (!Overlap(firstTransform, secondTransform))
        return;
      signals.emplace(firstEntity, secondEntity);
    }

    void CollideCheckStatic(Entity firstEntity, DynamicColliderC& dynamicCollider, TransformC& firstTransform, Entity secondEntity, StaticColliderC& secondStaticCollider, TransformC& secondTransform)
    {
      if (!Overlap(firstTransform, secondTransform))
        return;

      signals.emplace(firstEntity, secondEntity);
      if (!secondStaticCollider.resolveCollision)
        return;


      if (firstTransform.position.x != dynamicCollider.oldPosition.x)
      {
        TransformC transform = firstTransform;
        transform.position.y = dynamicCollider.oldPosition.y;

        if (Overlap(transform, secondTransform))
        {
          if (dynamicCollider.oldPosition.x < firstTransform.position.x)
            firstTransform.position.x = secondTransform.position.x - firstTransform.size.x;
          else
            firstTransform.position.x = secondTransform.position.x + secondTransform.size.x;
          if (firstEntity.HasComponent<PhysicsC>())
            firstEntity.GetComponent<PhysicsC>().velocity.x = 0;
        }
      }

      if (firstTransform.position.y != dynamicCollider.oldPosition.y)
      {
        TransformC transform = firstTransform;
        transform.position.x = dynamicCollider.oldPosition.x;

        if (Overlap(transform, secondTransform))
        {
          if (dynamicCollider.oldPosition.y < firstTransform.position.y)
            firstTransform.position.y = secondTransform.position.y - firstTransform.size.y;
          else
            firstTransform.position.y = secondTransform.position.y + secondTransform.size.y;
          if (firstEntity.HasComponent<PhysicsC>())
            firstEntity.GetComponent<PhysicsC>().velocity.y = 0;
        }
      }
    }

    void RunEntity(Entity entity, DynamicColliderC& dynamicCollider, TransformC& transform) override
    {
      manager->Each<DynamicColliderC, TransformC>(
        [&](EntityId otherEntity, DynamicColliderC& otherDynamicCollider, TransformC& otherTransform) 
        { 
          CollideCheckDynamic(entity, transform, Entity{manager, otherEntity}, otherTransform); 
        }
      );
      manager->Each<StaticColliderC, TransformC>(
        [&](EntityId otherEntity, StaticColliderC& otherStaticCollider, TransformC& otherTransform) 
        { 
          CollideCheckStatic(entity, dynamicCollider, transform, Entity{manager, otherEntity}, otherStaticCollider, otherTransform); 
        }
      );
      dynamicCollider.oldPosition = transform.position;
      while (!signals.empty())
      {
        manager->UpdateSystems(signals.front());
        signals.pop();
      }
    }
  };
}