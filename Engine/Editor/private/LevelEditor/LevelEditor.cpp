#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

static std::string float_3_to_string(const GameEngine::Core::array<float, 3>& float_arr)
{
	std::string float_str = std::to_string(float_arr[0]);
	for (size_t i = 1; i < 3; ++i)
		float_str = float_str + ", " + std::to_string(float_arr[i]);

	return float_str;
}

static void string_to_float_3(const std::string& str, GameEngine::Core::array<float, 3>& float_arr)
{
	std::istringstream inputStr(str);
	size_t count = 0;
	char currentChar;

	while (count < 3)
	{
		if (inputStr >> float_arr[count++])
		{
			if (!(inputStr >> currentChar && currentChar == ','))
				return;
		}
		else
		{
			break;
		}
	}
}

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world)
		{
			m_World = world.get_world();
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
			{
				SyncEntity(levelObject);
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
		}

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (World::LevelObject::Component& component : levelObject.GetComponents())
						{
							if (!std::strcmp(component.first.c_str(), "Position"))
							{
								Core::array<float, 3> floatBuffer;
								string_to_float_3(component.second, floatBuffer);
								ImGui::InputFloat3(component.first.c_str(), floatBuffer.data);
								component.second = float_3_to_string(floatBuffer);
							}
							else
								ImGui::InputText(component.first.c_str(), &component.second);
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("Add entity"))
			{
				m_CreatedObjCnt += 1;

				World::LevelObject* obj = new World::LevelObject();
				obj->SetName(std::format("NewObject{}", m_CreatedObjCnt).c_str());
				obj->AddComponent("Position", "0.000000,0.000000,0.000000");
				obj->AddComponent("GeometryPtr", "Cube");
				m_Level->AddLevelObject(*obj);

				SyncEntity(*obj);
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::SyncEntity(World::LevelObject& entity)
		{
			flecs::entity flecs_entity = m_World.entity(entity.GetName().c_str());

			World::LevelObject::ComponentList& componentList = entity.GetComponents();

			World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "Position");
				}
			);

			World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "GeometryPtr");
				}
			);

			if (positionAttribute != componentList.end() &&
				geometryAttribute != componentList.end())
			{
				assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second));

				flecs_entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &positionAttribute->second });

				// Can be set to 0 since it doesn't matter now, will be updated by the system
				flecs_entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
				flecs_entity.set(GeometryPtr{
					reinterpret_cast<RenderCore::Geometry*>(
						World::WorldParser::GetCustomComponents()[geometryAttribute->second]
						)
					});
			}
		}
	}
}