#pragma once

#include "global.h"
#include "CharacterCore.h"
#include "ResourceManager.h"

#include "GUI/Inventory.h"
#include "GUI/CharacterInfo.h"
#include "GUI/Spellbook.h"
#include "GUI/QuestLog.h"
#include "GUI/GUITabBar.h"
#include "GUI/MapOverlay.h"
#include "GUI/QuickSlotBar.h"

class WorldScreen;

enum class GUIElement {
	VOID = -1,
	Character = 0,
	Inventory,
	Spellbook,
	Journal,
	Map
};

// abstract class for an interface in level or map
class WorldInterface {
public:
	explicit WorldInterface(WorldScreen* screen);
	virtual ~WorldInterface() = default;

	// reloads the inventory for the items that have changed. if the string equals "gold", reloads gold
	virtual void reloadInventory(const std::string& changeditemID);
	// reloads the quest log
	virtual void reloadQuestLog();
	// reload the character info
	virtual void reloadCharacterInfo();
	// reload the spellbook
	virtual void reloadSpellBook();
	// reload the map overlay (waypoints)
	virtual void reloadMapWaypoints();
	// reload the level overlay
	virtual void reloadLevelOverlay();
	// opens the map overlay and jumps to a quest marker
	void jumpToQuestMarker(const std::string& questId, const std::vector<QuestMarkerData>& data);
	// opens the quest log and jumps to a quest
	void jumpToQuestLog(const std::string& questId);
	// show inventory directly
	void showInventory();

	// an consumable item has been dropped. forward to quick slot bar
	void notifyConsumableDrop(const SlotClone* item) const;
	// an item should be equipped in quick slot bar. forward to quick slot bar
	void equipConsumable(const std::string& itemID) const;
	// an item should be equipped in a specific slot in the quick slot bar.
	void equipConsumable(const std::string& itemID, int slotId) const;
	// highlight quickslots
	void highlightQuickslots(bool highlight) const;

	virtual void render(sf::RenderTarget& target);
	virtual void renderAfterForeground(sf::RenderTarget& target);
	virtual void update(const sf::Time& frameTime);
	virtual void hideAll();
	// returns whether any gui overlay is open (inventory, spellbook, etc.)
	virtual bool isGuiOverlayVisible() const;

	CharacterCore* getCore() const;
	virtual WorldScreen* getScreen() const;
	Inventory* getInventory() const;

protected:
	void loadGuiSidebar();
	void loadMapSidebar(bool isLevel);

protected:
	WorldScreen* m_screen;
	CharacterCore* m_core;

	GUITabBar* m_guiSidebar = nullptr;
	GUITabBar* m_mapSidebar = nullptr;
	Inventory* m_inventory = nullptr;
	CharacterInfo* m_characterInfo = nullptr;
	Spellbook* m_spellbook = nullptr;
	QuestLog* m_questLog = nullptr;
	MapOverlay* m_mapOverlay = nullptr;
	QuickSlotBar* m_quickSlotBar = nullptr;
	GUIElement m_selectedElement = GUIElement::Character;

	template<typename G>
	void updateGuiElement(const sf::Time& frameTime, G* guiElement, GUIElement type);

	template<typename G>
	void showGuiElement(G* guiElement, GUIElement type);
	void showGuiElement(GUIElement type);
	void connectGuiElements(GUIElement type);

	static Key getKeyFromGuiElement(GUIElement e);
};