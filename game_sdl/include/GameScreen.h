#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <string>

class Game;

class GameScreen {
public:
    GameScreen(Game* game);
    virtual ~GameScreen() = default;

    virtual void loop() = 0;
    virtual void deinit() = 0;

protected:
    Game* game;
};

// Screen implementations
class TitleScreen : public GameScreen {
public:
    TitleScreen(Game* game);
    void loop() override;
    void deinit() override;
};

class ModeSelectionScreen : public GameScreen {
public:
    ModeSelectionScreen(Game* game);
    void loop() override;
    void deinit() override;
};

class ComboTrialScreen : public GameScreen {
public:
    ComboTrialScreen(Game* game);
    void loop() override;
    void deinit() override;

private:
    void loadObjects();
};

#endif // GAMESCREEN_H
