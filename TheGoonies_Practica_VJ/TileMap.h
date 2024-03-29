#ifndef _TILE_MAP_INCLUDE
#define _TILE_MAP_INCLUDE


#include <glm/glm.hpp>



#include "Friend.h"
#include "Jaula.h"
#include "Llave.h"
#include "BlueBook.h"
#include "GreenBook.h"
#include "YellowBook.h"
#include "HyperShoes.h"
#include "portal.h"
#include "Texture.h"
#include "ShaderProgram.h"
#include "Trampa.h"
class Enemigo;
class Friend;
class Jaula;
class Llave;
class Trampa;

// Class Tilemap is capable of loading a tile map from a text file in a very
// simple format (see level01.txt for an example). With this information
// it builds a single VBO that contains all tiles. As a result the render
// method draws the whole map independently of what is visible.


class TileMap
{

public:
	// Tile maps can only be created inside an OpenGL context
	static TileMap* createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);

	TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program);
	~TileMap();

	void render() const;
	void update(float deltaTime);
	void free();

	int getTileSize() const { return tileSize; }

	bool collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const;
	bool collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, float* posY) const;
	bool collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, float* posY) const;

	bool canClimbUp(const glm::ivec2& pos, const glm::ivec2& size, float* posY, float& positionClimb) const;
	bool canClimbDown(const glm::ivec2& pos, const glm::ivec2& size, float* posY, float& positionClimb) const;


	vector<Enemigo*> getEnemys();
	vector<portal*> getPortals();

	vector<Item*> getItems();

	vector<Trampa*> getTrampas();

	Friend* getAmigo();
	Jaula* getJaula();

	void setPosPlayer(glm::ivec2 posPlayer);

private:
	bool loadLevel(const string& levelFile);
	void prepareArrays(const glm::vec2& minCoords, ShaderProgram& program);

private:
	GLuint vao;
	GLuint vbo;
	GLint posLocation, texCoordLocation;
	glm::ivec2 position, mapSize, tilesheetSize, posPlayer;
	int tileSize, blockSize;
	Texture tilesheet;
	glm::vec2 tileTexSize;
	int* map;
	vector<Enemigo*> enemys;
	vector<portal*> portals;

	Jaula* jaula;
	vector<Item*> items;

	vector<Trampa*> trampa;
};


#endif // _TILE_MAP_INCLUDE


