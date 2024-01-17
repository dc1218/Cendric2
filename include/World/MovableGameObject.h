#pragma once

#include "global.h"
#include "World/AnimatedGameObject.h"
#include "ResourceManager.h"

class MovingParent;

// A movable game object with physics. Abstract class
class MovableGameObject : public virtual AnimatedGameObject {
public:
	MovableGameObject();
	virtual ~MovableGameObject() { delete m_debugInfo; }

	void updateFirst(const sf::Time& frameTime) override;
	void update(const sf::Time& frameTime) override;

	void renderAfterForeground(sf::RenderTarget& target) override;
	
	void calculateNextPosition(const sf::Time& frameTime, sf::Vector2f& nextPos) const; 
	void calculateNextVelocity(const sf::Time& frameTime, sf::Vector2f& nextVel) const;
	virtual void calculateUnboundedVelocity(const sf::Time& frameTime, sf::Vector2f& nextVel) const;

	void setAcceleration(const sf::Vector2f& acceleration);
	void setAccelerationX(float accelerationX);
	void setAccelerationY(float accelerationY);
	void addAccelerationX(float accelerationX);
	void addAccelerationY(float accelerationY);
	void setVelocity(const sf::Vector2f& velocity);
	void setVelocityX(float velocityX);
	void setVelocityY(float velocityY);
	// the moving parent of a movable game object is used to update the relative velocity
	// it is used by moving parents (platforms, boats -> which are parent to themselves)
	virtual void setMovingParent(MovingParent* parent);
	
	const sf::Vector2f& getVelocity() const;
	const sf::Vector2f& getAcceleration() const;

	MovingParent* getMovingParent() const;
	void lockRelativeVelocityX();
	void lockRelativeVelocityY();

protected:
	virtual void updateRelativeVelocity(const sf::Time& frameTime);
	virtual bool collides(const sf::Vector2f& nextPos) const { return false; }
	virtual float getConfiguredMaxVelocityYUp() const;
	virtual float getConfiguredMaxVelocityYDown() const;
	virtual float getConfiguredMaxVelocityX() const;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
	virtual void boundVelocity(sf::Vector2f& vel) const;
	MovingParent* m_movingParent = nullptr;
	bool m_isLockedRelativeVelocityX = false;
	bool m_isLockedRelativeVelocityY = false;

	// debug info
	sf::Text* m_debugInfo = nullptr;
};