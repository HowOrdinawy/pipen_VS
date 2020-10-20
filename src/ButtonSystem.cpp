#include "gameStuff.hpp"

namespace game{
    void ButtonSystem::pressButtonAt(int x, int y, Scene *scene)
    {
        for (int i = layers.size() - 1; i >= 0; i--)
        {
            if (layers[i].mouseOnButton(x, y)) {
                layers[i].pressButtonAt(x, y, scene);
            }
        }
    }
}