#pragma once

#include "copium/example/ComponentHandlerBase.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

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
    }

    void Serialize(Entity entity, MetaFile& metaFile) const override
    {
      if(entity.HasComponent<Component>())
        metaFile.AddMetaClass(serializedName, MetaFileClass{});
    }

    void Deserialize(Entity entity, const MetaFileClass& metaClass) const override
    {
      entity.AddComponent<Component>();
    }

    void ComponentGui(Entity entity) const override
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

    bool IsFlagComponent() const override
    {
      return flagComponent;
    }

    void AddDefaultComponent(Entity entity) const override
    {
      Component component = Create(entity);
      entity.AddComponent<Component>(component);
    }

    bool HasComponent(Entity entity) const override
    {
      return entity.HasComponent<Component>();
    }

  protected:
    virtual void Gui(Component& component) const {}
    virtual Component Create(Entity entity) const { return Component{}; }

    void FlagComponentGui(Entity entity) const
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

    void DataComponentGui(Entity entity) const
    {
      if (!entity.HasComponent<Component>())
        return;

      Component& component = entity.GetComponent<Component>();
      if (ImGui::CollapsingHeader(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
      {
        Gui(component);
        if (ImGui::Button(std::string("Delete " + name).c_str()))
          entity.RemoveComponent<Component>();
        ImGui::NewLine();
      }
    }

    void EntityGui(const std::string& name, Entity* entity) const
    {
      ImGui::Text(name.c_str());
      ImGui::SameLine();
      std::string str;
      if (*entity)
        str = entity->GetComponent<NameC>().name;
      else
        str = "(drag and drop an entity)";
      ImGui::BeginDisabled();
      ImGui::InputText("##EntityGui", &str);
      ImGui::EndDisabled();
      if (ImGui::BeginDragDropTarget())
      {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_UUID"))
          *entity = GetEntity(entity->GetManager(), *(Uuid*)payload->Data);
        ImGui::EndDragDropTarget();
      }
      ImGui::SameLine();
      if (ImGui::Button("x"))
        entity->Invalidate();
    }
  };
}
