#include "games.h"
#include "gameMonsterCatcher.h"
#include "gameRacing.h"
#include "gameLua.h"

String currentGamePath;
GameConfig gameConfig;
void initGames(String gamePath, String* errorMessage) {
  currentGamePath = gamePath;
  readGameConfig(gamePath, &gameConfig, errorMessage);
  if (gameConfig.templateName == "monster") {
    initGames_monsterCatcher(gamePath, &gameConfig, errorMessage);
  } else if (gameConfig.templateName == "race") {
    initGames_racing(gamePath, &gameConfig, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    initGames_lua(gamePath, &gameConfig, errorMessage);
  } else {
    errorMessage->concat("Invalid game template ");
    errorMessage->concat(gameConfig.templateName);
  }
}

String getRandomWinScreenPathForCurrentGame(String* errorMessage) {
  return getRandomWinScreenPath(currentGamePath, errorMessage);
}

void drawShortBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawShortBlowGame_monsterCatcher(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawShortBlowGame_racing(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    drawShortBlowGame_lua(display, blowData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void drawEqualBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawEqualBlowGame_monsterCatcher(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawEqualBlowGame_racing(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    drawEqualBlowGame_lua(display, blowData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void drawLongBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawLongBlowGame_monsterCatcher(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawLongBlowGame_racing(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    drawLongBlowGame_lua(display, blowData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void drawTrampolineGame(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawTrampolineGame_monsterCatcher(display, jumpData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawTrampolineGame_racing(display, jumpData, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    drawTrampolineGame_lua(display, jumpData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void displayProgressionMenu(DISPLAY_T *display, String *errorMessage) {
  if (gameConfig.templateName == "monster") {
    displayProgressionMenu_monsterCatcher(display, errorMessage);
  } else if (gameConfig.templateName == "race") {
    displayProgressionMenu_racing(display, errorMessage);
  } else if (gameConfig.templateName == "lua") {
    displayProgressionMenu_lua(display, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}
