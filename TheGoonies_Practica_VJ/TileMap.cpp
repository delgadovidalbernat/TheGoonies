#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "TileMap.h"

#include "Chubasquero.h"
#include "Enemigo.h"
#include "EnemigoBasico.h"
#include "EnemigoComplejo.h"


using namespace std;


TileMap* TileMap::createTileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program)
{
	TileMap* map = new TileMap(levelFile, minCoords, program);

	return map;
}


TileMap::TileMap(const string& levelFile, const glm::vec2& minCoords, ShaderProgram& program)

{
	for (auto e : enemys)
		e = NULL;

	jaula = NULL;

	loadLevel(levelFile);
	prepareArrays(minCoords, program);

}

TileMap::~TileMap()
{
	if (map != NULL)
		delete map;

	if (jaula != NULL)
	{
		delete jaula;
	}
}


void TileMap::render() const
{
	glEnable(GL_TEXTURE_2D);
	tilesheet.use();
	glBindVertexArray(vao);
	glEnableVertexAttribArray(posLocation);
	glEnableVertexAttribArray(texCoordLocation);
	glDrawArrays(GL_TRIANGLES, 0, 6 * mapSize.x * mapSize.y);
	glDisable(GL_TEXTURE_2D);

	if (jaula != NULL)
	{
		jaula->render();
	}

	for (auto p : portals)
	{
		if (p != NULL)
		{

			p->render();
		}
	}

	for (auto e : enemys)
	{

		if (e != NULL)
			e->render();
	}

	for (auto i : items)
	{
		if (i != NULL)
		{

			i->render();
		}
	}

	for (auto t : trampa)
	{
		if (t != NULL)
		{

			t->render();
		}
	}
}

void TileMap::update(float deltaTime)
{
	for (auto e : enemys)
	{
		if (e != NULL)
		{

			if (e->typeOf() == "EnemigoBasico")
			{
				e->update(deltaTime);

			}
			else
			{
				e->setPosTarget(posPlayer);
				e->update(deltaTime);

			}

		}

	}

	
	for (auto p : portals)
	{
		if (p != NULL)
		{

			p->update(deltaTime);
		}
	}

	if (jaula != NULL)
	{
		jaula->update(deltaTime);
	}

	for (auto i : items)
	{
		if (i != NULL)
		{

			i->update(deltaTime);
		}
	}

	for (auto t : trampa)
	{
		if (t != NULL)
		{

			t->update(deltaTime);
		}
	}
}

void TileMap::free()
{
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	for (auto e : enemys) {

		e->free();

	}

	while (!enemys.empty())
	{
		enemys.pop_back();
	}

	while (!items.empty())
	{
		items.pop_back();
	}

	while (!trampa.empty())
	{
		trampa.pop_back();
	}
}

bool TileMap::loadLevel(const string& levelFile)
{
	ifstream fin;
	string line, tilesheetFile;
	stringstream sstream;
	char tile;

	fin.open(levelFile.c_str());
	if (!fin.is_open())
		return false;
	getline(fin, line);
	if (line.compare(0, 7, "TILEMAP") != 0)
		return false;
	getline(fin, line);
	sstream.str(line);
	sstream >> mapSize.x >> mapSize.y;
	getline(fin, line);
	sstream.str(line);
	sstream >> tileSize >> blockSize;
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetFile;
	tilesheet.loadFromFile(tilesheetFile, TEXTURE_PIXEL_FORMAT_RGBA);
	tilesheet.setWrapS(GL_CLAMP_TO_EDGE);
	tilesheet.setWrapT(GL_CLAMP_TO_EDGE);
	tilesheet.setMinFilter(GL_NEAREST);
	tilesheet.setMagFilter(GL_NEAREST);
	getline(fin, line);
	sstream.str(line);
	sstream >> tilesheetSize.x >> tilesheetSize.y;
	tileTexSize = glm::vec2(1.f / tilesheetSize.x, 1.f / tilesheetSize.y);

	map = new int[mapSize.x * mapSize.y];
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			fin.get(tile);
			if (tile == ' ')
				//es un vector no una matriz, por ese motivo cada fila de la matriz que representa el mapa 
				//es contigua en este vector
				map[j * mapSize.x + i] = 0;
			else
				map[j * mapSize.x + i] = tile - int('0');
		}
		fin.get(tile);
#ifndef _WIN32
		fin.get(tile);
#endif
	}
	fin.close();

	return true;
}

//0 es una celda sin colision, 1,2,3,4 representa bloques, 5 representa enemigo basico, 9 representa escaleras
void TileMap::prepareArrays(const glm::vec2& minCoords, ShaderProgram& program)
{
	int tile, nTiles = 0;
	glm::vec2 posTile, texCoordTile[2], halfTexel;
	vector<float> vertices;

	halfTexel = glm::vec2(0.5f / tilesheet.width(), 0.5f / tilesheet.height());
	for (int j = 0; j < mapSize.y; j++)
	{
		for (int i = 0; i < mapSize.x; i++)
		{
			tile = map[j * mapSize.x + i];

			//49 representa a
			if (tile == 49)
			{

				enemys.push_back(new EnemigoBasico());
				int LastEnemy = enemys.size() - 1;
				enemys[LastEnemy]->init(glm::ivec2(32, 16), program);
				enemys[LastEnemy]->setPosition(glm::vec2(i * getTileSize(), (j - 1) * getTileSize()));
				enemys[LastEnemy]->setTileMap(this);
				map[j * mapSize.x + i] = 0;

			}//representa la b
			else if (tile == 50) {

				enemys.push_back(new EnemigoComplejo);
				int LastEnemy = enemys.size() - 1;
				enemys[LastEnemy]->init(glm::ivec2(32, 16), program);
				enemys[LastEnemy]->setPosition(glm::vec2(i * getTileSize(), (j - 1) * getTileSize()));
				enemys[LastEnemy]->setTileMap(this);
				map[j * mapSize.x + i] = 0;

			}
			else if (tile == 9)
			{

				jaula = new Jaula;
				jaula->init(glm::ivec2(32, 16), program);
				jaula->setPosition(glm::vec2(((i-2) * getTileSize())-10, (j-3) * getTileSize()));
				jaula->setTileMap(this);
				map[j * mapSize.x + i] = 0;

			}else if (tile == 63) //representa la o
			{

				//aqui poner un portal de vuelta
				glm::vec2 position((i * getTileSize()) - 10, (j - 5) * getTileSize());

				portals.push_back(new portal);
				int LastPortal = portals.size() - 1;
				portals[LastPortal]->init(Previous, position, glm::ivec2(32, 16), program);
				map[j * mapSize.x + i] = 0;
				
			}else if (tile == 64) //representa la p
			{

				//aqui poner un portal de ida
				glm::vec2 position((i * getTileSize()) - 10, (j - 5) * getTileSize());

				portals.push_back(new portal);
				int LastPortal = portals.size() - 1;
				portals[LastPortal]->init(Posterior, position, glm::ivec2(32, 16), program);
				map[j * mapSize.x + i] = 0;
				
			}else if (tile == 60) // representa la l (Llave)
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize() );
				items.push_back(new Llave);
				int LastItem = items.size() - 1;
				items[LastItem]->init( glm::ivec2(32, 16), program, "images/llave.png" );
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;
				
			}
			else if (tile == 55) //representa g y la gota
			{

				glm::vec2 position((i * getTileSize() - 3), (j - 0) * getTileSize());
				trampa.push_back(new Trampa);
				int LastTrampa = trampa.size() - 1;
				
				trampa[LastTrampa]->init(glm::ivec2(32, 16), program, "images/gota.png");
				trampa[LastTrampa]->initPosition(position);
				trampa[LastTrampa]->setTileMap(this);
				map[j * mapSize.x + i] = 0;
				
			} else if (tile == 56) //representa la h de HyperShoes
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize());
				items.push_back(new HyperShoes);
				int LastItem = items.size() - 1;
				items[LastItem]->init(glm::ivec2(32, 16), program, "images/HyperShoes.png");
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;
				
			}else if (tile == 51)//representa la c de chubasquero
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize());
				items.push_back(new Chubasquero);
				int LastItem = items.size() - 1;
				items[LastItem]->init(glm::ivec2(32, 16), program, "images/chubasquero.png");
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;
				
			}else if (tile == 69)//representa la u, un libro azul
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize());
				items.push_back(new BlueBook);
				int LastItem = items.size() - 1;
				items[LastItem]->init(glm::ivec2(32, 16), program, "images/bluebook.png");
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;

			}
			else if (tile == 73)//representa la y, un libro amarillo
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize());
				items.push_back(new YellowBook);
				int LastItem = items.size() - 1;
				items[LastItem]->init(glm::ivec2(32, 16), program, "images/yellowbook.png");
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;

			}
			else if (tile == 53)//representa la e, un libro verde
			{

				glm::vec2 position((i * getTileSize()) - 10, (j - 1) * getTileSize());
				items.push_back(new GreenBook);
				int LastItem = items.size() - 1;
				items[LastItem]->init(glm::ivec2(32, 16), program, "images/greenbook.png");
				items[LastItem]->setPosition(position);
				map[j * mapSize.x + i] = 0;

			}
			else
				if (tile != 0)
				{
					// Non-empty tile
					nTiles++;
					posTile = glm::vec2(minCoords.x + i * tileSize, minCoords.y + j * tileSize);
					texCoordTile[0] = glm::vec2(float((tile - 1) % 4) / tilesheetSize.x, float((tile - 1) / 4) / tilesheetSize.y);
					texCoordTile[1] = texCoordTile[0] + tileTexSize;
					texCoordTile[0] += halfTexel;
					texCoordTile[1] -= halfTexel;
					// First triangle
					vertices.push_back(posTile.x); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
					// Second triangle
					vertices.push_back(posTile.x); vertices.push_back(posTile.y);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[0].y);
					vertices.push_back(posTile.x + blockSize); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[1].x); vertices.push_back(texCoordTile[1].y);
					vertices.push_back(posTile.x); vertices.push_back(posTile.y + blockSize);
					vertices.push_back(texCoordTile[0].x); vertices.push_back(texCoordTile[1].y);
				}
		}
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 24 * nTiles * sizeof(float), &vertices[0], GL_STATIC_DRAW);
	posLocation = program.bindVertexAttribute("position", 2, 4 * sizeof(float), 0);
	texCoordLocation = program.bindVertexAttribute("texCoord", 2, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

// Collision tests for axis aligned bounding boxes.
// Method collisionMoveDown also corrects Y coordinate if the box is
// already intersecting a tile below.

bool TileMap::collisionMoveLeft(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x, y0, y1;

	x = pos.x / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 7)
			return true;
	}

	return false;
}

//calcula en la altura del personmage desde el punto minimo del quadradao hasta la cabeza, y usa esa altura junto con la x
//para passar por filas en el vector que representa el mapa solo variando la altura, si alguna da distinto de 0 hay bloque
//y por lo tanto colision
bool TileMap::collisionMoveRight(const glm::ivec2& pos, const glm::ivec2& size) const
{
	int x, y0, y1;

	x = (pos.x + size.x - 1) / tileSize;
	y0 = pos.y / tileSize;
	y1 = (pos.y + size.y - 1) / tileSize;
	for (int y = y0; y <= y1; y++)
	{
		if (map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 7)
			return true;
	}

	return false;
}

bool TileMap::collisionMoveDown(const glm::ivec2& pos, const glm::ivec2& size, float* posY) const
{
	int x0, x1, y;
	//solo se divide por el hecho que el personaje lo tenen,mos en coordenadas de juego
	//mientras que al recorer el vector del mapa debemos estar en coordenadas de mapa
	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 1) / tileSize;//le suma a la posicion en coordenadas de pantalla lo que mide un cubo de suelo que es justo lo que
	//ocupa el personaje y luego lo divide entre lo que ocupa un tile para volver a coordenadas de juego
	y = (pos.y + size.y - 1) / tileSize;
	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 7)
		{
			if (*posY - tileSize * y + size.y <= 4)
			{
				*posY = tileSize * y - size.y;
				return true;
			}
		}
	}

	return false;
}

bool TileMap::collisionMoveUp(const glm::ivec2& pos, const glm::ivec2& size, float* posY) const
{
	int x0, x1, y, y0;

	x0 = pos.x / tileSize;
	x1 = (pos.x + size.x - 5) / tileSize;
	y = (pos.y) / tileSize;

	for (int x = x0; x <= x1; x++)
	{
		if (map[y * mapSize.x + x] > 0 && map[y * mapSize.x + x] <= 7)
		{

			return true;
		}
	}

	return false;
}

bool TileMap::canClimbUp(const glm::ivec2& pos, const glm::ivec2& size, float* posY, float& positionClimb) const
{

	int x0, x1, y, y0;

	x0 = pos.x / tileSize;
	x1 = ((pos.x + size.x - 5) / tileSize);
	y = (pos.y) / tileSize;

	for (int x = x0; x <= x1; x++)
	{
		if (map[(y + 2) * mapSize.x + x] == 8 || map[(y - 1) * mapSize.x + x] == 8)
		{
			if (map[(y - 1) * mapSize.x + x] != 8)
			{
				positionClimb = 0;
			}
			else
			{

				positionClimb = x - (.5f);
			}

			return true;
		}
	}

	return false;
}

bool TileMap::canClimbDown(const glm::ivec2& pos, const glm::ivec2& size, float* posY, float& positionClimb) const
{

	int x0, x1, y, y0;

	x0 = pos.x / tileSize;
	x1 = ((pos.x + size.x - 5) / tileSize);
	y = (pos.y) / tileSize;

	for (int x = x0; x <= x1; x++)
	{
		if (map[(y + 3) * mapSize.x + x] == 8 || map[(y + 1) * mapSize.x + x] == 8)
		{
			if (map[(y + 3) * mapSize.x + x] != 8)
			{
				positionClimb = 0;
			}
			else
			{

				positionClimb = x - .5f;
			}

			return true;
		}
	}

	return false;
}

vector<Enemigo*> TileMap::getEnemys()
{
	return enemys;
}

vector<portal*> TileMap::getPortals()
{

	return portals;
}

vector<Item*> TileMap::getItems()
{

	return items;
}

vector<Trampa*> TileMap::getTrampas()
{

	return trampa;
}


Friend* TileMap::getAmigo()
{
	if (jaula != NULL)
	{
		return jaula->getAmigo();
	}
	return NULL;
}

Jaula* TileMap::getJaula()
{
	return jaula;
}

void TileMap::setPosPlayer(glm::ivec2 posPlayer)
{

	this->posPlayer = posPlayer;
}






























