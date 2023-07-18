#pragma once

#include "copium/example/ComponentHandler.h"
#include "copium/example/Components.h"

#include <misc/cpp/imgui_stdlib.h>

namespace Copium
{
  class NameComponentHandler : public ComponentHandler<NameC>
  {
  public:
    NameComponentHandler() : ComponentHandler{"Name", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      NameC name;
      name.name = metaClass.GetValue("name");
      entity.AddComponent<NameC>(name);
    }

  protected:
    void Gui(NameC& name) override
    {
      ImGui::InputText("Name##Name", &name.name);
    }

    NameC Create(Entity entity) override
    {
      return NameC{String::Format("Entity %d", entity.GetId())};
    }
  };

  class TransformComponentHandler : public ComponentHandler<TransformC>
  {
  public:
    TransformComponentHandler() : ComponentHandler{"Transform", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      TransformC transform;
      transform.position = ReadVec2Opt(metaClass, "position", glm::vec2{0.0f, 0.0f});
      transform.size = ReadVec2Opt(metaClass, "size", glm::vec2{1.0f, 1.0f});
      entity.AddComponent<TransformC>(transform);
    }

  protected:
    void Gui(TransformC& transform) override
    {
      ImGui::DragFloat2("Position", (float*)&transform.position);
      ImGui::DragFloat2("Size", (float*)&transform.size);
    }

    TransformC Create(Entity entity) override
    {
      return TransformC{glm::vec2{0, 0}, glm::vec2{1, 1}};
    }
  };

  class TextureComponentHandler : public ComponentHandler<TextureC>
  {
  public:
    TextureComponentHandler() : ComponentHandler{"Texture", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      TextureC texture;
      texture.asset = AssetRef{AssetManager::LoadAsset(Uuid{metaClass.GetValue("texture-uuid")})};
      texture.texCoord1 = ReadVec2Opt(metaClass, "tex-coord", glm::vec2{0.0f, 0.0f});
      texture.texCoord2 = texture.texCoord1 + ReadVec2Opt(metaClass, "tex-size", glm::vec2{1.0f, 1.0f} - texture.texCoord1);
      entity.AddComponent<TextureC>(texture);
    }

  protected:
    void Gui(TextureC& texture) override
    {
      Asset& asset = AssetManager::GetAsset<Texture2D>(texture.asset);
      ImGui::Text("Asset: %s", asset.GetName().c_str());
      ImGui::DragFloat2("Tex Coord 1", (float*)&texture.texCoord1, 0.01, 0.0f, 1.0f);
      ImGui::DragFloat2("Tex Coord 2", (float*)&texture.texCoord2, 0.01, 0.0f, 1.0f);
    }

    TextureC Create(Entity entity) override
    {
      return TextureC{AssetRef{AssetManager::DuplicateAsset(Vulkan::GetEmptyTexture2D())}, glm::vec2{0, 0}, glm::vec2{1, 1}};
    }
  };

  class TextComponentHandler : public ComponentHandler<TextC>
  {
  public:
    TextComponentHandler() : ComponentHandler{"Text", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      char* endPtr;
      TextC text;
      text.font = AssetRef{AssetManager::LoadAsset(Uuid{metaClass.GetValue("font")})};
      text.text = metaClass.GetValue("text");
      text.fontSize = std::strtof(metaClass.GetValue("font-size").c_str(), &endPtr);
      entity.AddComponent<TextC>(text);
    }

  protected:
    void Gui(TextC& text) override
    {
      Asset& asset = AssetManager::GetAsset<Font>(text.font);
      ImGui::Text(asset.GetName().c_str());
      ImGui::InputTextMultiline("Text##Text", &text.text);
      ImGui::DragFloat("Font Size", &text.fontSize);
    }

    TextC Create(Entity entity) override
    {
      return TextC{AssetRef{AssetManager::LoadAsset<Font>("font.meta")}, "", 20.0f};
    }
  };

  class StaticColliderComponentHandler : public ComponentHandler<StaticColliderC>
  {
  public:
    StaticColliderComponentHandler() : ComponentHandler{"Static Collider", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      StaticColliderC staticCollider;
      staticCollider.resolveCollision = ReadBoolOpt(metaClass, "resolve-collision", true);
      entity.AddComponent<StaticColliderC>(staticCollider);
    }

  protected:
    void Gui(StaticColliderC& staticCollider) override
    {
      ImGui::Checkbox("Resolve Collision##StaticCollider", &staticCollider.resolveCollision);
    }

    StaticColliderC Create(Entity entity) override
    {
      return StaticColliderC{false};
    }
  };

  class DynamicColliderComponentHandler : public ComponentHandler<DynamicColliderC>
  {
  public:
    DynamicColliderComponentHandler() : ComponentHandler{"Dynamic Collider", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      DynamicColliderC dynamicCollider;
      dynamicCollider.resolveCollision = ReadBoolOpt(metaClass, "resolve-collision", true);
      dynamicCollider.colliderOffset = ReadVec2Opt(metaClass, "collider-offset", glm::vec2{0.0f, 0.0f});
      dynamicCollider.colliderSize = ReadVec2Opt(metaClass, "collider-size", glm::vec2{1.0f, 1.0f});
      entity.AddComponent<DynamicColliderC>(dynamicCollider);
    }

  protected:
    void Gui(DynamicColliderC& dynamicCollider) override
    {
      ImGui::Checkbox("Resolve Collision##DynamicCollider", &dynamicCollider.resolveCollision);
      ImGui::DragFloat2("Collider Offset", (float*)&dynamicCollider.colliderOffset, 0.01);
      ImGui::DragFloat2("Collider Size", (float*)&dynamicCollider.colliderSize, 0.01);
    }

    DynamicColliderC Create(Entity entity) override
    {
      return DynamicColliderC{true, glm::vec2{0, 0}, glm::vec2{1, 1}};
    }
  };

  class PlayerComponentHandler : public ComponentHandler<PlayerC>
  {
  public:
    PlayerComponentHandler() : ComponentHandler{"Player", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      PlayerC player;
      player.camera = GetEntity(entity.GetManager(), Uuid{metaClass.GetValue("camera-uuid")});
      entity.AddComponent<PlayerC>(player);
    }

  protected:
    void Gui(PlayerC& player) override
    {
      if (player.camera)
        ImGui::Text("Camera: %s", player.camera.GetComponent<NameC>().name.c_str());
      else
        ImGui::Text("No camera attached");
    }

    PlayerC Create(Entity entity) override
    {
      return PlayerC{};
    }
  };

  class CameraComponentHandler : public ComponentHandler<CameraC>
  {
  private:
    BoundingBox* viewport;
  public:
    CameraComponentHandler(BoundingBox* viewport) : ComponentHandler{"Camera", false}, viewport{viewport} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      float aspect = viewport->GetSize().x / viewport->GetSize().y;

      CameraC camera;
      camera.staticBoundingBox = ReadBoolOpt(metaClass, "static-bounding-box", false);
      camera.uiCamera = ReadBoolOpt(metaClass, "ui-camera", false);
      if (camera.uiCamera)
        camera.projection = BoundingBox(0, 0, Vulkan::GetSwapChain().GetExtent().width, Vulkan::GetSwapChain().GetExtent().height);
      else
        camera.projection = BoundingBox(-aspect, -1.0f, aspect, 1.0f);
      entity.AddComponent<CameraC>(camera);
    }

  protected:
    void Gui(CameraC& camera) override
    {
      ImGui::Checkbox("Static", &camera.staticBoundingBox);
      ImGui::Checkbox("Ui camera", &camera.uiCamera); // TODO: If this changes, the bounding box should be modified if it is not static
    }

    CameraC Create(Entity entity) override
    {
      return CameraC{BoundingBox{-1.0f, -1.0f, 1.0f, 1.0f}, false, false};
    }
  };

  class UuidComponentHandler : public ComponentHandler<UuidC>
  {
  public:
    UuidComponentHandler() : ComponentHandler{"Uuid", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      UuidC uuid;
      uuid.uuid = Uuid{metaClass.GetValue("uuid")};
      entity.AddComponent<UuidC>(uuid);
    }

  protected:
    void ComponentGui(Entity entity) override {}
  };

  class HealthComponentHandler : public ComponentHandler<HealthC>
  {
  public:
    HealthComponentHandler() : ComponentHandler{"Health", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      char* endPtr;
      HealthC health;
      health.max = std::strtol(metaClass.GetValue("health").c_str(), &endPtr, 10);
      health.current = health.max;
      entity.AddComponent<HealthC>(health);
    }

  protected:
    void Gui(HealthC& health) override
    {
      ImGui::DragInt("Max Health", &health.max);
    }

    HealthC Create(Entity entity) override
    {
      return HealthC{10, 10};
    }
  };

  class PhysicsComponentHandler : public ComponentHandler<PhysicsC>
  {
  public:
    PhysicsComponentHandler() : ComponentHandler{"Physics", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      char* endPtr;
      PhysicsC physics;
      physics.mass = std::strtof(metaClass.GetValue("mass").c_str(), &endPtr);
      entity.AddComponent<PhysicsC>(physics);
    }

  protected:
    void Gui(PhysicsC& physics) override
    {
      ImGui::DragFloat("Mass", &physics.mass);
    }

    PhysicsC Create(Entity entity) override
    {
      return PhysicsC{10.0f};
    }
  };

  class AnimationComponentHandler : public ComponentHandler<AnimationC>
  {
  public:
    AnimationComponentHandler() : ComponentHandler{"Animation", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      char* endPtr;
      AnimationC animation;
      animation.sheetCoord = ReadVec2Opt(metaClass, "sheet-coord", glm::ivec2{0, 0});
      animation.sheetSize = ReadVec2Opt(metaClass, "sheet-size", glm::ivec2{1, 1});
      animation.images = std::strtol(metaClass.GetValue("images").c_str(), &endPtr, 10);
      animation.horizontal = ReadBoolOpt(metaClass, "horizontal", true);
      animation.time = std::strtof(metaClass.GetValue("time").c_str(), &endPtr);
      entity.AddComponent<AnimationC>(animation);
    }

  protected:
    void Gui(AnimationC& animation) override
    {
      ImGui::DragInt2("Sheet Size", (int*)&animation.sheetSize);
      ImGui::DragInt2("Sheet Coord", (int*)&animation.sheetCoord, 1, 0, std::max(animation.sheetSize.x, animation.sheetSize.y));
      ImGui::DragInt("Images", &animation.images);
      ImGui::DragFloat("Frame time", &animation.time);
      ImGui::Checkbox("Horizontal", &animation.horizontal);
    }

    AnimationC Create(Entity entity) override
    {
      return AnimationC{glm::ivec2{0, 0}, glm::ivec2{1, 1}, 1, true, 1.0f};
    }
  };

  class DebugComponentHandler : public ComponentHandler<DebugC>
  {
  public:
    DebugComponentHandler() : ComponentHandler{"Debug", false} {}

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      DebugC debug;
      debug.playerEntity = GetEntity(entity.GetManager(), Uuid{metaClass.GetValue("player-uuid")});
      entity.AddComponent<DebugC>(debug);
    }

  protected:
    void Gui(DebugC& debug) override
    {
      if (debug.playerEntity)
        ImGui::Text("Player: %s", debug.playerEntity.GetComponent<NameC>().name.c_str());
      else
        ImGui::Text("No player attached");
    }

    DebugC Create(Entity entity) override
    {
      return DebugC{};
    }
  };
}
