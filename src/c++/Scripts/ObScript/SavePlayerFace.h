#pragma once

#include "Shared/SharedFunctions.h"
#include "Shared/NPCFileUtils.h"

namespace ObScript
{
	class SavePlayerFace
	{
	public:
		static void Install()
		{
			const auto functions = RE::SCRIPT_FUNCTION::GetScriptFunctions();
			const auto it = std::find_if(
				functions.begin(),
				functions.end(),
				[&](auto&& a_elem)
				{
					return _stricmp(a_elem.functionName, "SavePCFace") == 0;
				});

			if (it != functions.end())
			{
				static std::array params{
					RE::SCRIPT_PARAMETER{ "String", RE::SCRIPT_PARAM_TYPE::kChar, false },
				};

				*it = RE::SCRIPT_FUNCTION{ LONG_NAME.data(), SHORT_NAME.data(), it->output };
				it->helpString = HelpString().data();
				it->referenceFunction = false;
				it->paramCount = static_cast<std::uint16_t>(params.size());
				it->parameters = params.data();
				it->executeFunction = Execute;

				logger::debug("Registered SavePCFace."sv);
			}
			else
			{
				logger::warn("WARNING: Failed to register SavePCFace."sv);
			}
		}

	private:
		static bool Execute(
			const RE::SCRIPT_PARAMETER* a_parameters,
			const char* a_compiledParams,
			RE::TESObjectREFR* a_refObject,
			RE::TESObjectREFR* a_container,
			RE::Script* a_script,
			RE::ScriptLocals* a_scriptLocals,
			float&,
			std::uint32_t& a_offset)
		{

			std::array<char, 0x200> rawComment{ '\0' };
			auto result = RE::Script::ParseParameters(
				a_parameters,
				a_compiledParams,
				a_offset,
				a_refObject,
				a_container,
				a_script,
				a_scriptLocals,
				rawComment.data());

			if (!result)
			{
				return true;
			}

			if (rawComment[0] == '\0')
			{
				return true;
			}

			RE::Actor* a_actor = RE::PlayerCharacter::GetSingleton();
			RE::TESNPC* npc = a_actor->GetNPC();

			RE::NPCFile npcFile{};
			std::span<RE::BGSHeadPart*> headParts = npc->GetHeadParts();

			npcFile.bodyMorphRegionsA = RE::GetBodyMorphRegionsFromNPC(npc);					// Body Morph Regions
			logger::debug("Body Morph Regions");
			npcFile.facialBoneMorphIntensity = npc->GetFacialBoneMorphIntensity();				// Facial Bone Morph Intensity
			logger::debug("Facial Bone Morph Intensity");
			npcFile.facialBoneRegionsA = GetFacialBoneRegionsFromNPC(npc);						// Facial Bone Regions
			logger::debug("Facial Bone Regions");
			npcFile.headPartEyebrows = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kEyebrows);		//	Eyebrows Head Part
			npcFile.headPartEyes = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kEyes);				//	Eyes Head Part
			npcFile.headPartFace = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kFace);				//	Face Head Part
			npcFile.headPartFacial_Hair = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kFacialHair);	//	Facial Hair Head Part
			npcFile.headPartHair = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kHair);				//	Hair Head Part
			npcFile.headPartHead_Rear = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kHeadRear);		//	Head Rear Head Part
			npcFile.headPartTeeth = GetSpecificHeadPartOfType(headParts, npc->numHeadParts, RE::BGSHeadPart::HeadPartType::kTeeth);				//	Teeth Head Part
			logger::debug("Head Parts");
			npcFile.morphSlidersA = GetMorphSliderValuesFromNPC(npc);							//Morph Sliders
			logger::debug("Morph Sliders");
			npcFile.tintEntriesA = GetTintEntriesFromNPC(npc, npc->GetSex());					// Tint Entries
			logger::debug("Tint Entries");

			RE::NPCFileExport::ExportNPCFile(npcFile, rawComment.data());

			return true;
		}
		

		[[nodiscard]] static const std::string& HelpString()
		{
			static auto help = []()
			{
				std::string buf;
				buf += "Exports the Player's face as "sv;
				buf += "a .npc file for use in importing"sv;
				buf += "into the Creation Kit."sv;
				return buf;
			}();
			return help;
		}

		static constexpr auto LONG_NAME = "SavePCFace"sv;
		static constexpr auto SHORT_NAME = "spf"sv;

		inline static char _delim{ '\t' };

		inline static std::ofstream _file;
	};
}