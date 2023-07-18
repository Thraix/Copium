#pragma once

#include "copium/example/ComponentHandlerBase.h"

#include <imgui.h>

namespace Copium
{
  template <typename Component>
  class ComponentHandler : public ComponentHandlerBase
  {
  protected:
    std::string name;
    std::string serializedName;
    bool flagComponent;
  public:
    ComponentHandler(const std::string& name, bool flagComponent)
      : name{name}, flagComponent{flagComponent}
    {
      serializedName = name;
      serializedName.erase(std::remove(serializedName.begin(), serializedName.end(), ' '), serializedName.end());
      CP_INFO("%s", serializedName.c_str());
    }

    void Serialize(Entity entity, MetaFile& metaFile) override
    {
      if(entity.HasComponent<Component>())
        metaFile.AddMetaClass(serializedName, MetaFileClass{});
    }

    void Deserialize(Entity entity, const MetaFileClass& metaClass) override
    {
      entity.AddComponent<Component>();
    }

    void ComponentGui(Entity entity) override
    {
      if (flagComponent)
        FlagComponentGui(entity);
      else
        DataComponentGui(entity);
    }

    const std::string& GetName() const override
    {
      return name;
    }

    const std::string& GetSerializedName() const override
    {
      return serializedName;
    }

  protected:
    virtual void Gui(Component& component) {}
    virtual Component Create(Entity entity) { return Component{}; }

    void FlagComponentGui(Entity entity)
    {
      bool shouldHaveComponent = entity.HasComponent<Component>();
      ImGui::Checkbox(name.c_str(), &shouldHaveComponent);
      if(shouldHaveComponent == entity.HasComponent<Component>())
        return;

      if (shouldHaveComponent)
        entity.AddComponent<Component>();
      else
        entity.RemoveComponent<Component>();
    }

    void DataComponentGui(Entity entity)
    {
      if (entity.HasComponent<Component>())
      {
        Component& component = entity.GetComponent<Component>();
        if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
          Gui(component);
          if (ImGui::Button(std::string("Delete " + name).c_str()))
            entity.RemoveComponent<Component>();
        }
      }
      else
      {
        // TODO: These buttons should probably be in a context menu when you right-click the entity instead
        if (ImGui::Button(std::string("Add " + name).c_str()))
          entity.AddComponent<Component>(Create(entity));
      }
    }

  };
}
