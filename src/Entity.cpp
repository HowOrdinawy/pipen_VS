#include "gameStuff.hpp"

namespace game
{
    void Entity::addComponent(std::string name, Component *comp)
    {
        components[name] = comp;
    }

    Component * Entity::getComponent(std::string name)
    {
        return components[name];
    }
}