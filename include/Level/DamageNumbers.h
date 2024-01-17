#pragma once

#include "global.h"
#include "ResourceManager.h"
#include "GlobalResource.h"

enum class DamageNumberType {
	Damage,
	DamageOverTime,
	Heal,
	HealOverTime
};


struct DamageNumberData final {
	bool active;
	float startPosition;
	float time;
	sf::Text* text;
};

class DamageNumbers final {
public:
	DamageNumbers(bool isAlly);
	~DamageNumbers();

	void update(const sf::Time& frameTime);
	void render(sf::RenderTarget& target);

	void emitNumber(int value, const sf::Vector2f& position, DamageNumberType type, bool critical);
	void emitString(std::string str, const sf::Vector2f& position, DamageNumberType type);

private:
	static const int MAX_NUMBERS;
	static const float START_OFFSET;
	static const float DISTANCE;
	static const float TIME;

	float offset;

	std::vector<DamageNumberData> m_data;
	int m_nextIndex;
	bool m_isAlly;
};