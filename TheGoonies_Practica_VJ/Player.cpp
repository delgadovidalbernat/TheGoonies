#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "Player.h"
#include "Game.h"
#include "SoundPlayer.h"


#define JUMP_ANGLE_STEP 4
#define JUMP_HEIGHT 70
#define FALL_STEP 4


enum PlayerAnims
{
	STAND_LEFT, STAND_RIGHT, MOVE_LEFT, MOVE_RIGHT, JUMP_LEFT, JUMP_RIGHT, CLIMB, HIT_LEFT, HIT_RIGHT
};

enum PlayerStat
{

	NORMAL, DAMAGED, DEADD
};


void Player::init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram)
{
	health = 100.f;
	experience = 0.f;
	timeHitting = 0;
	delayToHitAgain = 0;
	velocidad = 2.f;

	tieneChubasquero = false;
	tieneHyperShoes = false;
	tieneLlave = false;
	bCanHit = true;
	bhitting = false;
	bClimbing = false;
	bJumping = false;
	bCanClimb = false;
	bCanJump = true;

	estado = NORMAL;
	
	spritesheet.loadFromFile("images/player.png", TEXTURE_PIXEL_FORMAT_RGBA);
	sprite = Sprite::createSprite(glm::ivec2(32, 32), glm::vec2(0.5, 1.f / 6.f), &spritesheet, &shaderProgram);
	sprite->setNumberAnimations(9);

	sprite->setAnimationSpeed(STAND_LEFT, 9);
	sprite->addKeyframe(STAND_LEFT, glm::vec2(0.f, 0.f));

	sprite->setAnimationSpeed(STAND_RIGHT, 9);
	sprite->addKeyframe(STAND_RIGHT, glm::vec2(0.5f, 0.f));

	sprite->setAnimationSpeed(MOVE_LEFT, 9);
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, 0.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, (1.f / 6.f) * 2.f));
	sprite->addKeyframe(MOVE_LEFT, glm::vec2(0.f, (1.f / 6.f) * 2.f));

	sprite->setAnimationSpeed(MOVE_RIGHT, 9);
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.5, 0.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.5, 1.f / 6.f));
	sprite->addKeyframe(MOVE_RIGHT, glm::vec2(0.5, (1.f / 6.f) * 2.f));

	sprite->setAnimationSpeed(JUMP_LEFT, 9);
	sprite->addKeyframe(JUMP_LEFT, glm::vec2(0., 0.5f));

	sprite->setAnimationSpeed(JUMP_RIGHT, 9);
	sprite->addKeyframe(JUMP_RIGHT, glm::vec2(0.5, 0.5f));

	sprite->setAnimationSpeed(HIT_LEFT, 8);
	sprite->addKeyframe(HIT_LEFT, glm::vec2(0., (1.f / 6.f) * 4));

	sprite->setAnimationSpeed(HIT_RIGHT, 8);
	sprite->addKeyframe(HIT_RIGHT, glm::vec2(0.5f, (1.f / 6.f) * 4));

	sprite->setAnimationSpeed(CLIMB, 8);
	sprite->addKeyframe(CLIMB, glm::vec2(0., (1.f / 6.f) * 5));
	sprite->addKeyframe(CLIMB, glm::vec2(0.5f, (1.f / 6.f) * 5));


	sprite->changeAnimation(0);
	tileMapDispl = tileMapPos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

}

void Player::update(int deltaTime)
{
	sprite->update(deltaTime);

	if (Game::instance().getKey(32) && bCanHit && (sprite->animation() != JUMP_LEFT && sprite->animation() != JUMP_RIGHT))
	{

		bhitting = true;
		bCanHit = false;
	}

	if (!bClimbing)
	{
		bCanJump = true;
		bCanFall = true;
	}


	if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
	{
		if (sprite->animation() == JUMP_RIGHT)
		{

			sprite->changeAnimation(JUMP_LEFT);
		}

		if (sprite->animation() != MOVE_LEFT && sprite->animation() != JUMP_RIGHT && sprite->animation() != JUMP_LEFT && sprite->animation() != CLIMB)
			sprite->changeAnimation(MOVE_LEFT);

		if (!bhitting)
			posPlayer.x -= velocidad;

		if (map->collisionMoveLeft(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x += velocidad;

			if (sprite->animation() != CLIMB)
				sprite->changeAnimation(STAND_LEFT);
		}
	}
	else if (Game::instance().getSpecialKey(GLUT_KEY_RIGHT))
	{

		if (sprite->animation() == JUMP_LEFT)
		{

			sprite->changeAnimation(JUMP_RIGHT);
		}

		if (sprite->animation() != MOVE_RIGHT && sprite->animation() != JUMP_RIGHT && sprite->animation() != JUMP_LEFT && sprite->animation() != CLIMB)
			sprite->changeAnimation(MOVE_RIGHT);

		if (!bhitting)
			posPlayer.x += velocidad;

		if (map->collisionMoveRight(posPlayer, glm::ivec2(32, 32)))
		{
			posPlayer.x -= velocidad;

			if (sprite->animation() != CLIMB)
				sprite->changeAnimation(STAND_RIGHT);
		}
	}
	else
	{
		if (sprite->animation() == MOVE_LEFT)
			sprite->changeAnimation(STAND_LEFT);
		else if (sprite->animation() == MOVE_RIGHT)
			sprite->changeAnimation(STAND_RIGHT);
	}

	if ((sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT) && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != CLIMB)
	{
		float aux;
		if (!map->canClimbUp(posPlayer, glm::ivec2(32, 32), &posPlayer.y, aux))
		{
			sprite->changeAnimation(JUMP_LEFT);
		}
		
	}
	else if ((sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT) && Game::instance().getSpecialKey(GLUT_KEY_UP) && sprite->animation() != CLIMB)
	{

		sprite->changeAnimation(JUMP_RIGHT);
	}

	if (bClimbing && sprite->animation() != CLIMB)
	{
		sprite->changeAnimation(CLIMB);

	}
	if (bClimbing && sprite->animation() == CLIMB && (!Game::instance().getSpecialKey(GLUT_KEY_UP) && !Game::instance().getSpecialKey(GLUT_KEY_DOWN)))
	{
		sprite->changeAnimation(CLIMB);
	}
	
	if (sprite->animation() == CLIMB && !bCanClimb)
	{
		if (Game::instance().getSpecialKey(GLUT_KEY_LEFT))
		{
			sprite->changeAnimation(JUMP_LEFT);
		}else if(GLUT_KEY_RIGHT)
		{

			sprite->changeAnimation(JUMP_RIGHT);
		}

		bClimbing = false;
		
	}

	if((sprite->animation() == MOVE_LEFT || sprite->animation() == STAND_LEFT) && !bCanClimb && !map->collisionMoveDown(glm::fvec2(posPlayer.x, posPlayer.y+1), glm::ivec2(32, 32), &posPlayer.y))
	{

		sprite->changeAnimation(JUMP_LEFT);
		bClimbing = false;
	}

	if ((sprite->animation() == MOVE_RIGHT || sprite->animation() == STAND_RIGHT) && !bCanClimb && !map->collisionMoveDown(glm::fvec2(posPlayer.x, posPlayer.y + 1), glm::ivec2(32, 32), &posPlayer.y))
	{

		sprite->changeAnimation(JUMP_RIGHT);
		bClimbing = false;
	}
	
	float aux;
	glm::fvec2 posPlayerForChangeAnimationClimbUp(posPlayer.x, posPlayer.y - (.5f * map->getTileSize()));
	if (!map->canClimbUp(posPlayerForChangeAnimationClimbUp, glm::ivec2(32, 32), &posPlayer.y, aux) && sprite->animation() == CLIMB)
	{
		sprite->changeAnimation(STAND_LEFT);
	}

	if (((sprite->animation() == JUMP_RIGHT || sprite->animation() == JUMP_LEFT)))
	{
		bCanClimb = false;
	}
	

	if (bhitting)
	{
		if (!bClimbing) {

			if (sprite->animation() == STAND_LEFT || sprite->animation() == MOVE_LEFT)
				sprite->changeAnimation(HIT_LEFT);
			else if (sprite->animation() != HIT_LEFT)
				sprite->changeAnimation(HIT_RIGHT);
		}

	}
	else if (sprite->animation() == HIT_LEFT)
	{

		sprite->changeAnimation(STAND_LEFT);
	}
	else if (sprite->animation() == HIT_RIGHT)
	{

		sprite->changeAnimation(STAND_RIGHT);
	}

	if (bJumping)
	{


		jumpAngle += JUMP_ANGLE_STEP;
		if (jumpAngle == 180)
		{
			bJumping = false;
			posPlayer.y = startY;
		}
		else
		{
			posPlayer.y = int(startY - JUMP_HEIGHT * sin(3.14159f * jumpAngle / 180.f));
			if (jumpAngle > 90)
				bJumping = !map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y);
		}
		if (map->collisionMoveUp(posPlayer, glm::ivec2(32, 32), &posPlayer.y))
		{
			bJumping = false;

		}
	}
	else
	{
		
		float positionClimb;
		if (map->canClimbUp(posPlayer, glm::ivec2(32, 32), &posPlayer.y, positionClimb) && Game::instance().getSpecialKey(GLUT_KEY_UP))
		{
			//En este caso no puede saltar pero si puede escalar(andar verticalmente)
			bJumping = false;
			bClimbing = true;
			bCanClimb = true;
			
			if (positionClimb != 0)
			{
				posPlayer.x = positionClimb * map->getTileSize();
			}

			posPlayer.y -= 2;
			startY = posPlayer.y;

		}
		else if ((Game::instance().getSpecialKey(GLUT_KEY_DOWN)) && map->canClimbDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y, positionClimb))
		{

			bJumping = false;
			bClimbing = true;
			bCanClimb = true;
			bCanFall = true;
			
			if (positionClimb != 0)
			{
				posPlayer.x = positionClimb * map->getTileSize();
			}

			posPlayer.y += 2;
			startY = posPlayer.y;
		}
		else
		{
			float positionClimb;
			if ((!map->canClimbUp(posPlayer, glm::ivec2(32, 32), &posPlayer.y, positionClimb) || !bCanClimb) && bCanFall)
			{
				posPlayer.y += FALL_STEP;
				
			}

			if (map->collisionMoveDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y))
			{
				
				bCanClimb = true;

				if (Game::instance().getSpecialKey(GLUT_KEY_UP) && bCanJump)
				{

					bJumping = true;
					jumpAngle = 0;
					startY = posPlayer.y;

				}

				if (sprite->animation() == JUMP_RIGHT)
				{
					sprite->changeAnimation(STAND_RIGHT);

				}
				else if (sprite->animation() == JUMP_LEFT)
				{
					sprite->changeAnimation(STAND_LEFT);
				}
			}

		}
	}

	

	if (!map->canClimbUp(posPlayer, glm::ivec2(32, 32), &posPlayer.y, aux) && !map->canClimbDown(posPlayer, glm::ivec2(32, 32), &posPlayer.y, aux))
	{
		bCanClimb = false;
	}else
	{
		bCanClimb = true;
	}

	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));

	if (health <= 0.f)
	{
		Game::instance().setBLose(true);
		Game::instance().setNeedToRestart(true);
		SoundPlayer::instance().stopAllSongs();
		SoundPlayer::instance().play2DSong("die", false);
		Sleep(1000);
	}

	if (timeHitting == 10)
	{
		bhitting = false;
		timeHitting = 0;
	}

	if (delayToHitAgain == 15)
	{
		bCanHit = true;
		delayToHitAgain = 0;
	}

	if (bhitting)
	{
		++timeHitting;

	}
	else if (!bCanHit)
	{

		++delayToHitAgain;
	}

	if (experience == 100.f)
	{
		if (health >= 75.f && health <= 100.f)
		{
			health = 100.f;
		}
		else
			health += 25.f;

		experience = 0.f;
	}

	if (tieneHyperShoes && velocidad != 3.f)
	{
		setVelocidad(3.f);
	}
}

void Player::render()
{
	sprite->render();
}

void Player::restart()
{
	health = 100.f;
	experience = 0.f;
	setVelocidad(2.f);
	tieneLlave = false;
	tieneHyperShoes = false;
	tieneChubasquero = false;
	tieneBlueBook = false;
	tieneYellowBook = false;
	tieneGreenBook = false;
}

void Player::setTileMap(TileMap* tileMap)
{
	map = tileMap;
}

void Player::setPosition(const glm::vec2& pos)
{
	posPlayer = pos;
	sprite->setPosition(glm::vec2(float(tileMapDispl.x + posPlayer.x), float(tileMapDispl.y + posPlayer.y)));
}

bool Player::getBClimbing()
{
	return bClimbing;
}

bool Player::getbCanJump()
{
	return bCanJump;
}

void Player::setbCanJump(bool value)
{
	bCanJump = value;
}

void Player::setbCanFall(bool value)
{
	bCanFall = value;
}

glm::ivec2 Player::getPosPlayer()
{
	return posPlayer;
}

TileMap* Player::getMap()
{
	return map;
}


float Player::getHealth()
{

	return health;
}

void Player::setHealth(float life)
{
	health = life;
}

void Player::addHealth(float amountLife)
{
	health += amountLife;
}

float Player::getExperiencie()
{
	return experience;
}

bool Player::enemyContact(glm::fvec2 enemyPos)
{
	if (enemyPos.y == posPlayer.y)
	{

		if (sprite->animation() == HIT_LEFT)
		{
			float voulnerablePosition = 1.15 * map->getTileSize();
			if(enemyPos.x > (posPlayer.x - voulnerablePosition) && enemyPos.x <= (posPlayer.x))

				return true;
		}

		if (sprite->animation() == HIT_RIGHT)
		{
			float voulnerablePosition = 1.15 * map->getTileSize();
			if(enemyPos.x < (posPlayer.x + voulnerablePosition) && enemyPos.x >= (posPlayer.x))

				return true;
		}

		if (tieneGreenBook)
		{
			float voulnerablePosition = 1. * map->getTileSize();
			if (enemyPos.x < (posPlayer.x + voulnerablePosition) && enemyPos.x >= (posPlayer.x))

				return true;
		}
	}
	

	return false;
}

void Player::punchIfPossible(Enemigo& enemy, float amount)
{
	
	
	if (enemyContact(enemy.getPosPlayer()) && bhitting && enemy.getEstado() == ALIVE)
	{
		if ((enemy.getHealth() - amount) <= 0.f)
		{
			experience += 25.f;
			enemy.setEstado(DEAD);
			enemy.setPuedeCollisionar(false);
			enemy.setCanRender(false);
			HUD::instance().updateExperience(experience);
			SoundPlayer::instance().play2DSong("punch", false);
		}
		enemy.addHealth(-amount);
	}

	if (enemyContact(enemy.getPosPlayer()) && tieneGreenBook && enemy.getEstado() == ALIVE)
	{
		if ((enemy.getHealth() - amount) <= 0.f)
		{
			experience += 25.f;
			HUD::instance().updateExperience(experience);
			SoundPlayer::instance().play2DSong("punch", false);
		}
		enemy.addHealth(-amount);
		tieneGreenBook = false;
	}
}

bool Player::getTieneLlave()
{

	return tieneLlave;
}

void Player::setTieneLlave(bool value)
{

	tieneLlave = value;
}

bool Player::getTieneHyperShoes()
{

	return tieneHyperShoes;
}

void Player::setTieneHyperShoes(bool value)
{

	tieneHyperShoes = value;
}

bool Player::getTieneChubasquero()
{
	return tieneChubasquero;
}

void Player::setTieneChubasquero(bool value)
{
	tieneChubasquero = value;
}

bool Player::getTieneBlueBook()
{

	return tieneBlueBook;
}

void Player::setTieneBlueBook(bool value)
{
	tieneBlueBook = value;
}

bool Player::getTieneGreenBook()
{
	return tieneGreenBook;
}

void Player::setTieneGreenBook(bool value)
{

	tieneGreenBook = value;
}

bool Player::getTieneYellowBook()
{

	return tieneYellowBook;
}

void Player::setTieneYellowBook(bool value)
{

	tieneYellowBook = value;
}

void Player::setVelocidad(float velocidad)
{
	
	sprite->setAnimationSpeed(MOVE_LEFT, velocidad * 4.5);
	sprite->setAnimationSpeed(MOVE_RIGHT, velocidad * 4.5);
	
	this->velocidad = velocidad;
}
