#pragma once

#include "CharacterCore.h"
#include "ResourceManager.h"
#include "Window.h"
#include "GUI/InventorySlot.h"
#include "GUI/ItemDescriptionWindow.h"
#include "GUI/TexturedTabBar.h"
#include "GUI/InventoryEquipment.h"
#include "GUI/SelectableWindow.h"

class LevelInterface;
class MapInterface;
class MerchantInterface;
class ScrollBar;
class ScrollHelper;
class WorldInterface;

// the inventory, as displayed in a level or a map
// it takes its information directly from the character core
// the inventory also shows item description and equipped items.
class Inventory final : public SelectableWindow {
public:
	Inventory(LevelInterface* _interface);
	Inventory(MapInterface* _interface);
	~Inventory();

	void show();
	void hide();

	bool isVisible() const;

	void render(sf::RenderTarget& target);
	void renderAfterForeground(sf::RenderTarget& target);
	void update(const sf::Time& frameTime);

	void notifyChange(const std::string itemID);
	void startTrading(MerchantInterface* _interface);
	void stopTrading();

	InventoryEquipment* getEquipment() const;

private:
	CharacterCore* m_core;
	LevelInterface* m_levelInterface = nullptr;
	MapInterface* m_mapInterface = nullptr;
	MerchantInterface* m_merchantInterface = nullptr;
	WorldInterface* getInterface() const;

	bool m_isVisible = false;
	sf::Vector2f m_position;

	void init();
	void setPosition(const sf::Vector2f& position);
	void reload();
	void reloadGold();

	void clearAllSlots();
	void calculateSlotPositions() const;
	static bool isSlotInvisible(const InventorySlot* slot);

	sf::Text m_goldText;
	sf::Text m_selectedTabText;
	sf::Text m_emptyText;

	SlicedSprite m_scrollWindow;
	ScrollBar* m_scrollBar = nullptr;
	ScrollHelper *m_scrollHelper = nullptr;

	TexturedTabBar* m_tabBar;

	sf::Sprite m_goldSprite;

	std::map<std::string, InventorySlot*> m_equipmentItems;
	std::map<std::string, InventorySlot*> m_consumableItems;
	std::map<std::string, InventorySlot*> m_documentItems;
	std::map<std::string, InventorySlot*> m_questItems;
	std::map<std::string, InventorySlot*> m_keyItems;
	std::map<std::string, InventorySlot*> m_miscItems;
	ButtonGroup* m_buttonGroup = nullptr;

	ItemType m_currentTab;
	// first is the id, the second is VOID when it is no equiment slot and an Item Type when it is an equipment slot
	std::pair<std::string, ItemType> m_selectedSlotId;
	void selectTab(ItemType type);
	void reloadButtonGroup(bool keepSelectedButton = true);
	// item type shall be VOID for not-equipment-slots
	void selectSlot(const std::string& selectedSlotId, ItemType type);
	void deselectCurrentSlot();
	InventorySlot* getSelectedSlot() const;

	void handleMapRightClick(const InventorySlot* clicked);
	void handleLevelRightClick(const InventorySlot* clicked);

	void handleLevelDoubleClick(const InventorySlot* clicked) const;
	void handleMapDoubleClick(const InventorySlot* clicked) const;

	// used for drag & drop handling
	SlotClone* m_currentClone = nullptr;
	bool m_hasDraggingStarted = false;
	bool m_isEquipmentSlotDragged = false;
	bool m_isSelectedByButtonGroup = false;
	bool m_isDragging = false;
	// the mouse has to move this distance while pressed to spawn a clone.
	const float DRAG_DISTANCE = 10.f;
	sf::Vector2f m_startMousePosition;
	void handleDragAndDrop();
	void stopDragging();
	
	void removeEquipmentItem();
	void handleMapDrag() const;
	void handleLevelDrag();
	void handleMapDrop();
	void handleLevelDrop();

	ItemDescriptionWindow* m_descriptionWindow = nullptr;
	void showDescription(const Item* item, bool isEquipmentOrigin) const;
	void hideDescription() const;
	void showDocument(const Item* item) const;
	void convertItem(const Item* item) const;
	void learnSpell(const Item* item) const;

protected:
	void updateWindowSelected() override;
	void updateButtonActions();
	void updateButtonActionLevel(const InventorySlot* slot);
	void updateButtonActionMap(const InventorySlot* slot);

private:
	InventoryEquipment* m_equipment = nullptr;

	static const int SLOT_COUNT_X;
	static const int SLOT_COUNT_Y;
	
	static const sf::Vector2f BUTTON_SIZE;

	static const float ICON_MARGIN;
	static const float WINDOW_MARGIN;

	static const float INVENTORY_LEFT;
	static const float SCROLL_WINDOW_LEFT;
	static const float SCROLL_WINDOW_TOP;
	static const float SCROLL_WINDOW_WIDTH;
	static const float SCROLL_WINDOW_HEIGHT;

	static const float INVENTORY_WIDTH;

	std::map<ItemType, std::map<std::string, InventorySlot*>*> m_typeMap;
};