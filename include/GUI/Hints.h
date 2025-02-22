#pragma once

#include "TextProvider.h"
#include "Enums/EnumNames.h"
#include "Controller/GamepadMappings/GamepadMappings.h"

inline std::string getHintTitle(const std::string& hintKey) {
	return g_textProvider->getText(hintKey, "hint");
}

inline std::string getKeyName(Key key) {
	auto& mainMap = g_resourceManager->getConfiguration().mainKeyMap;
	if (!g_inputController->isGamepadConnected() && contains(mainMap, key)) {
		return EnumNames::getKeyboardKeyName(mainMap.at(key));
	}

	if (key == Key::Move_Up) {
		return g_textProvider->getText("Up");
	}
	if (key == Key::Move_Down) {
		return g_textProvider->getText("Down");
	}
	if (key == Key::Move_Left) {
		return g_textProvider->getText("Left");
	}
	if (key == Key::Move_Right) {
		return g_textProvider->getText("Right");
	}

	auto& keymap = g_resourceManager->getConfiguration().gamepadKeyMap;

	if (key == Key::Inventory ||
		key == Key::CharacterInfo ||
		key == Key::Journal ||
		key == Key::Map ||
		key == Key::Spellbook) {
		return GamepadMappings::getKeyName(Key::Menu);
	}

	if (contains(keymap, key)) {
		return GamepadMappings::getKeyName(key);
	}
	
	if (contains(mainMap, key)) {
		return EnumNames::getKeyboardKeyName(mainMap.at(key));
	}

	return std::string("?");
}

inline std::string getHintDescription(const std::string& hintKey) {
	std::string hintText = (g_textProvider->getText("Press", "hint_desc")) + " ";

	if (hintKey == "Inventory") {
		hintText.append(getKeyName(Key::Inventory) + " ");
	}
	else if (hintKey == "Chop") {
		if (g_inputController->isGamepadConnected()) {
			hintText.append(getKeyName(Key::Attack) + " ");
		}
		else {
			hintText.append(getKeyName(Key::Chop) + " ");
		}
	}
	else if (hintKey == "Jump") {
		hintText.append(getKeyName(Key::Jump) + " ");
	}
	else if (hintKey == "Journal") {
		hintText.append(getKeyName(Key::Journal) + " ");
	}
	else if (hintKey == "Spellbook") {
		hintText.append(getKeyName(Key::Spellbook) + " ");
	}
	else if (hintKey == "CharacterInfo") {
		hintText.append(getKeyName(Key::CharacterInfo) + " ");
	}
	else if (hintKey == "Map" || hintKey == "Waypoint") {
		hintText.append(getKeyName(Key::Map) + " ");
	}
	else if (hintKey == "Pickup") {
		hintText.append(getKeyName(Key::Interact) + " ");
	}
	else if (hintKey == "Reload") {
		hintText.append(getKeyName(Key::BackToCheckpoint) + " ");
	}
	else if (hintKey == "LeaveLevel") {
		hintText.append(getKeyName(Key::Move_Up) + " ");
	}
	else if (hintKey == "Scout") {
		hintText.append(getKeyName(Key::Move_Up) + " ");
		hintText.append(g_textProvider->getText("And") + " ");
		hintText.append(getKeyName(Key::Move_Down) + " ");
	}
	else if (hintKey == "Highlight") {
		hintText.append(getKeyName(Key::ToggleTooltips) + " ");
	}
	else if (hintKey == "MapMove") {
		hintText.append(getKeyName(Key::Move_Up) + ", ");
		hintText.append(getKeyName(Key::Move_Left) + ", ");
		hintText.append(getKeyName(Key::Move_Down) + " ");
		hintText.append(g_textProvider->getText("And") + " ");
		hintText.append(getKeyName(Key::Move_Right) + " ");
	}
	else {
		hintText.clear();
	}
	hintText.append(g_textProvider->getText(hintKey, "hint_desc"));

	return hintText;
}