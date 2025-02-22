#include "World/AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(sf::Time frameTime, bool paused, bool looped) :
	m_animation(NULL), m_frameTime(frameTime), m_currentFrame(0), m_isPaused(paused), m_isLooped(looped), m_texture(NULL) {

}

void AnimatedSprite::setAnimation(const Animation* animation) {
	m_animation = animation;
	m_frameTime = animation->getFrameTime();
	m_texture = m_animation->getSpriteSheet();
	m_currentFrame = 0;
	setFrame(m_currentFrame);
	setLooped(m_animation->isLooped());
	play();
}

void AnimatedSprite::setRandomStartingFrame() {
	if (!m_animation || m_animation->getSize() < 2) return;
	m_currentFrame = rand() % m_animation->getSize();
	setFrame(m_currentFrame);
}

void AnimatedSprite::setFrameTime(sf::Time time) {
	m_frameTime = time;
}

void AnimatedSprite::play() {
	m_isPaused = false;
}

void AnimatedSprite::play(Animation* animation) {
	if (getAnimation() != animation)
		setAnimation(animation);
	play();
}

void AnimatedSprite::pause() {
	m_isPaused = true;
}

void AnimatedSprite::stop() {
	m_isPaused = true;
	m_currentFrame = 0;
	setFrame(m_currentFrame);
}

void AnimatedSprite::gotoFirstFrame() {
	m_currentFrame = 0;
	setFrame(m_currentFrame);
}

void AnimatedSprite::setLooped(bool looped) {
	m_isLooped = looped;
}

void AnimatedSprite::setFlippedX(bool flipped) {
	m_isFlippedX = flipped;
	if (flipped) {
		setScale(-1.f, getScale().y);
	}
	else {
		setScale(1.f, getScale().y);
	}

}

void AnimatedSprite::setFlippedY(bool flipped) {
	m_isFlippedY = flipped;
	if (flipped) {
		setScale(getScale().x, -1.f);
	}
	else {
		setScale(getScale().x, 1.f);
	}
}

void AnimatedSprite::reset() {
	setColor(COLOR_WHITE);
	setRotation(0.f);
	setFlippedX(false);
	setFlippedY(false);
	setScale(sf::Vector2f(1.f, 1.f));
	setOrigin(0.f, 0.f);
}

void AnimatedSprite::setColor(const sf::Color& color) {
	// Update the vertices' color
	m_vertices[0].color = color;
	m_vertices[1].color = color;
	m_vertices[2].color = color;
	m_vertices[3].color = color;
}

const Animation* AnimatedSprite::getAnimation() const {
	return m_animation;
}

sf::FloatRect AnimatedSprite::getLocalBounds() const {
	sf::IntRect rect = m_animation->getFrame(m_currentFrame);

	float width = static_cast<float>(std::abs(rect.width));
	float height = static_cast<float>(std::abs(rect.height));

	return sf::FloatRect(0.f, 0.f, width, height);
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const {
	return getTransform().transformRect(getLocalBounds());
}

bool AnimatedSprite::isLooped() const {
	return m_isLooped;
}

bool AnimatedSprite::isPlaying() const {
	return !m_isPaused;
}

bool AnimatedSprite::isFlippedX() const {
	return m_isFlippedX;
}

bool AnimatedSprite::isFlippedY() const {
	return m_isFlippedY;
}

sf::Time AnimatedSprite::getFrameTime() const {
	return m_frameTime;
}

std::size_t AnimatedSprite::getCurrentFrame() const {
	return static_cast<int>(m_currentFrame);
}

void AnimatedSprite::setFrame(size_t newFrame, bool resetTime) {
	if (m_animation) {
		//calculate new vertex positions and texture coordiantes
		sf::IntRect rect = m_animation->getFrame(newFrame);

		m_vertices[0].position = sf::Vector2f(0.f, 0.f);
		m_vertices[1].position = sf::Vector2f(0.f, static_cast<float>(rect.height));
		m_vertices[2].position = sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
		m_vertices[3].position = sf::Vector2f(static_cast<float>(rect.width), 0.f);

		float left = static_cast<float>(rect.left) + 0.0001f;
		float right = left + static_cast<float>(rect.width);
		float top = static_cast<float>(rect.top);
		float bottom = top + static_cast<float>(rect.height);

		m_vertices[0].texCoords = sf::Vector2f(left, top);
		m_vertices[1].texCoords = sf::Vector2f(left, bottom);
		m_vertices[2].texCoords = sf::Vector2f(right, bottom);
		m_vertices[3].texCoords = sf::Vector2f(right, top);
	}

	if (resetTime)
		m_currentTime = sf::Time::Zero;
}

void AnimatedSprite::update(sf::Time deltaTime) {
	// if not paused and we have a valid animation
	if (!m_isPaused && m_animation) {
		// add delta time
		m_currentTime += deltaTime;

		// if current time is bigger then the frame time advance one frame
		if (m_currentTime >= m_frameTime) {
			// reset time, but keep the remainder
			m_currentTime = sf::microseconds(m_currentTime.asMicroseconds() % m_frameTime.asMicroseconds());

			// get next Frame index
			if (m_currentFrame + 1 < m_animation->getSize())
				m_currentFrame++;
			else {
				// animation has ended
				if (!m_isLooped) {
					m_isPaused = true;
				}
				else {
					m_currentFrame = 0; // reset to start
				}

			}

			// set the current frame, not reseting the time
			setFrame(m_currentFrame, false);
		}
	}
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const {
	if (m_animation && m_texture) {
		states.transform *= getTransform();
		states.texture = m_texture;
		target.draw(m_vertices, 4, sf::Quads, states);
	}
}