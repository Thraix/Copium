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

    bool Overlap(TransformC& firstTransform, DynamicColliderC& dynamicCollider, TransformC& secondTransform)
    {
      glm::vec2 firstPosition1 = firstTransform.position + dynamicCollider.colliderOffset * firstTransform.size;
      glm::vec2 firstPosition2 = firstTransform.position + (dynamicCollider.colliderOffset + dynamicCollider.colliderSize) * firstTransform.size;
      if (firstPosition1.x >= secondTransform.position.x + secondTransform.size.x)
        return false;
      if (firstPosition1.y >= secondTransform.position.y + secondTransform.size.y)
        return false;
      if (firstPosition2.x <= secondTransform.position.x)
        return false;
      if (firstPosition2.y <= secondTransform.position.y)
        return false;
      return true;
    }

    void CollideCheckDynamic(Entity firstEntity, DynamicColliderC& dynamicCollider, TransformC& firstTransform, Entity secondEntity, TransformC& secondTransform)
    {
      if (firstEntity.GetId() >= secondEntity.GetId())
        return;
      if (!Overlap(firstTransform, dynamicCollider, secondTransform))
        return;
      signals.emplace(firstEntity, secondEntity, false, 0, 0);
    }

    void CollideCheckStatic(Entity firstEntity, DynamicColliderC& dynamicCollider, TransformC& firstTransform, Entity secondEntity, StaticColliderC& secondStaticCollider, TransformC& secondTransform)
    {
      if (!Overlap(firstTransform, dynamicCollider, secondTransform))
        return;

      int xDir = 0;
      if (firstTransform.position.x != dynamicCollider.oldPosition.x)
      {
        TransformC transform = firstTransform;
        transform.position.y = dynamicCollider.oldPosition.y;

        if (Overlap(transform, dynamicCollider, secondTransform))
        {
          xDir = dynamicCollider.oldPosition.x < firstTransform.position.x ? 1 : -1;
          if (secondStaticCollider.resolveCollision)
          {
            if (dynamicCollider.oldPosition.x < firstTransform.position.x)
              firstTransform.position.x = secondTransform.position.x - firstTransform.size.x * (dynamicCollider.colliderOffset.x + dynamicCollider.colliderSize.x);
            else
              firstTransform.position.x = secondTransform.position.x + secondTransform.size.x - firstTransform.size.x * dynamicCollider.colliderOffset.x;
            if (firstEntity.HasComponent<PhysicsC>())
              firstEntity.GetComponent<PhysicsC>().velocity.x = 0;
          }
        }
      }

      int yDir = 0;
      if (firstTransform.position.y != dynamicCollider.oldPosition.y)
      {
        TransformC transform = firstTransform;
        transform.position.x = dynamicCollider.oldPosition.x;

        if (Overlap(transform, dynamicCollider, secondTransform))
        {
          yDir = dynamicCollider.oldPosition.y < firstTransform.position.y ? 1 : -1;
          if (secondStaticCollider.resolveCollision)
          {
            if (dynamicCollider.oldPosition.y < firstTransform.position.y)
              firstTransform.position.y = secondTransform.position.y - firstTransform.size.y * (dynamicCollider.colliderOffset.y + dynamicCollider.colliderSize.y);
            else
              firstTransform.position.y = secondTransform.position.y + secondTransform.size.y - firstTransform.size.y * dynamicCollider.colliderOffset.y;
            if (firstEntity.HasComponent<PhysicsC>())
              firstEntity.GetComponent<PhysicsC>().velocity.y = 0;
          }
        }
      }
      signals.emplace(firstEntity, secondEntity, secondStaticCollider.resolveCollision, xDir, yDir);
    }

    void RunEntity(Entity entity, DynamicColliderC& dynamicCollider, TransformC& transform) override
    {
      manager->Each<DynamicColliderC, TransformC>(
        [&](EntityId otherEntity, DynamicColliderC& otherDynamicCollider, TransformC& otherTransform) 
        { 
          CollideCheckDynamic(entity, dynamicCollider, transform, Entity{manager, otherEntity}, otherTransform);
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