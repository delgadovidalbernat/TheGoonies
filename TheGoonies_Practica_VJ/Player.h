#ifndef _PLAYER_INCLUDE
#define _PLAYER_INCLUDE


#include "Sprite.h"
#include "TileMap.h"


// Player is basically a Sprite that represents the player. As such it has
// all properties it needs to track its movement, jumping, and collisions.


class Player
{

public:

	virtual void init(const glm::ivec2& tileMapPos, ShaderProgram& shaderProgram);
	virtual void update(int deltaTime);
	virtual void render();
	void restart();

	void setTileMap(TileMap* tileMap);
	void setPosition(const glm::vec2& pos);

	bool getBClimbing();

	bool getbCanJump();
	void setbCanJump(bool value);

	void setbCanFall(bool value);

	glm::ivec2 getPosPlayer();
	TileMap* getMap();

	float getHealth();
	void setHealth(float life);
	void addHealth(float amountHealth);

	float getExperiencie();

	bool enemyContact(glm::fvec2 enemyPos);
	void punchIfPossible(Enemigo& enemy, float amount);

	bool getTieneLlave();
	void setTieneLlave(bool value);

	bool getTieneHyperShoes();
	void setTieneHyperShoes(bool value);

	bool getTieneChubasquero();
	void setTieneChubasquero(bool value);

	bool getTieneBlueBook();
	void setTieneBlueBook(bool value);

	bool getTieneGreenBook();
	void setTieneGreenBook(bool value);

	bool getTieneYellowBook();
	void setTieneYellowBook(bool value);

	void setVelocidad(float velocdad);


protected:

	bool tieneBlueBook;
	bool tieneGreenBook;
	bool tieneYellowBook;
	bool tieneChubasquero;
	bool tieneHyperShoes;
	bool tieneLlave;
	bool bhitting;
	bool bCanHit;
	bool bJumping;
	bool bCanJump;
	bool bClimbing;
	bool bCanClimb;
	bool bCanFall;
	glm::ivec2 tileMapDispl;
	glm::fvec2 posPlayer;
	int jumpAngle, startY;
	Texture spritesheet;
	Sprite* sprite;
	TileMap* map;
	float health;
	float experience;
	float velocidad;

	int timeHitting;
	int delayToHitAgain;

	int estado;

};


#endif // _PLAYER_INCLUDE


