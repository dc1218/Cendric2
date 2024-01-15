#pragma once

#include "global.h"
#include "World/GameObject.h"
#include "TextProvider.h"
#include "ResourceManager.h"

#include "GUI/BitmapText.h"
#include "GUI/SlicedSprite.h"

class TabButton : public virtual GameObject {
public:
	TabButton(const sf::FloatRect& box);
	virtual ~TabButton() {}

	void setActive(bool active);
	bool isActive() const;

	void onLeftJustPressed() override;
	void onLeftClick() override;
	void onMouseOver() override;
	void render(sf::RenderTarget& renderTarget) override;
	void update(const sf::Time& frameTime) override;

	void setOnClick(const std::function<void()>& agent);
	void setPosition(const sf::Vector2f& position) override;

	// position will be set automatically as the center of the button.

	// setting text using the text provider (translated)
	void setText(const std::string& text, const sf::Color& color, int charSize);
	void setText(const std::string& text, int charSize);
	void setText(const std::string& text);
	// setting raw text without text provider (not translated)
	void setTextRaw(const std::string& text, const sf::Color& color, int charSize);
	void setTextRaw(const std::string& text, int charSize);
	void setTextRaw(const std::string& text);

	void setCharacterSize(int size);
	void setTextColor(const sf::Color& color);
	void setBackgroundColor(const sf::Color& color);
	void setHighlightColor(const sf::Color& color);
	
	bool isClicked() const;

	GameObjectType getConfiguredType() const override;

	static const float BORDER_OFFSET;
	static const float BOUNDING_BOX_OFFSET;
	static const float ALIGNMENT_OFFSET;

protected:
	bool m_isActive = false;
	bool m_isMouseOver = false;
	bool m_isPressed = false;
	bool m_isClicked = false;

	SlicedSprite m_border;
	sf::RectangleShape m_background;

	sf::FloatRect m_outerRect;
	sf::FloatRect m_innerRect;

	sf::Text m_text;

	sf::Color m_backgroundColor = COLOR_TRANS_BLACK;
	sf::Color m_activeColor = COLOR_TRANS_GREY;
	sf::Color m_highlightColor = COLOR_PURPLE;

private:
	std::function<void()> m_executeOnClick = NOP;
};