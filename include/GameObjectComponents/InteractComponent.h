#pragma once

#include "GameObjectComponents/TooltipComponent.h"

class MainCharacter;

// Used for objects which Cendric can interact with. These include a tooltip, as well.
class InteractComponent final : public TooltipComponent {
public:
	InteractComponent(std::string tooltip, AnimatedGameObject* parent, MainCharacter* mainChar);

	void update(const sf::Time& frameTime) override;
	void renderAfterForeground(sf::RenderTarget& renderTarget) override;
	void setPosition(const sf::Vector2f& pos) override;
	void onDisposed() override;

	void setOnInteract(const std::function<void()>& agent);
	void interact();
	void setInteractText(const std::string& textKey);
	void setInteractTextColor(const sf::Color& color);
	void setInteractRange(float range);
	void setFocused(bool focused);
	void setInteractable(bool interactable);

	float getDistanceToMainChar() const;
	bool isInteractable() const;

private:
	bool m_isInteractable;
	bool m_isFocused;
	float m_interactRange;
	MainCharacter* m_mainChar;
	sf::Text m_interactText;
	std::function<void()> m_executeOnInteract = NOP;
};