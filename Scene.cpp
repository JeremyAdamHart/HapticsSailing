#include "Scene.h"

using namespace std;

Scene::Scene(){}

void Scene::addObject(Drawable* object){ objects.push_back(object); }

bool Scene::removeObject(Drawable* object){
	for (int i = 0; i < objects.size(); i++){
		if (object == objects[i]){
			objects[i] = objects.back();
			objects.pop_back();

			return true;
		}
	}

	return false;
}

void Scene::clearScene(){ objects.clear(); }

DrawableIterator Scene::objectBegin(){ return objects.begin(); }
DrawableIterator Scene::objectEnd(){ return objects.end(); }