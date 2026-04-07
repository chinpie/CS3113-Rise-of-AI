#include "Scene.h"

class GameOverScreen : public Scene
{
private:
public:
    GameOverScreen();
    GameOverScreen(Vector2 origin, const char *bgHexCode);
    ~GameOverScreen();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};