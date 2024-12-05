#include "games.h"

String currentGamePath;
GameConfig gameConfig;
void initGames(String gamePath, String* errorMessage) {
  currentGamePath = gamePath;
  readGameConfig(gamePath, &gameConfig, errorMessage);
  if (gameConfig.templateName == "monster") {
    initGames_monsterCatcher(gamePath, &gameConfig, errorMessage);
  } else if (gameConfig.templateName == "race") {
    initGames_racing(gamePath, &gameConfig, errorMessage);
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
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void drawLongBlowGame(DISPLAY_T* display, BlowData* blowData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawLongBlowGame_monsterCatcher(display, blowData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawLongBlowGame_racing(display, blowData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void drawTrampolineGame(DISPLAY_T* display, JumpData* jumpData, String* errorMessage) {
  if (gameConfig.templateName == "monster") {
    drawTrampolineGame_monsterCatcher(display, jumpData, errorMessage);
  } else if (gameConfig.templateName == "race") {
    drawTrampolineGame_racing(display, jumpData, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}

void displayProgressionMenu(DISPLAY_T *display, String *errorMessage) {
  if (gameConfig.templateName == "monster") {
    displayProgressionMenu_monsterCatcher(display, errorMessage);
  } else if (gameConfig.templateName == "race") {
    displayProgressionMenu_racing(display, errorMessage);
  } else {
    errorMessage->concat("No game loaded!");
  }
}
