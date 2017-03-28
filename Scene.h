#pragma once

#include <vector>
#include <iterator>
#include "Drawable.h"

typedef std::vector<Drawable*>::iterator DrawableIterator;

class Scene{

	std::vector<Drawable*> objects;

public:
	Scene();

	void addObject(Drawable *object);
	bool removeObject(Drawable *object);

	void clearScene();

	DrawableIterator objectBegin();
	DrawableIterator objectEnd();
};