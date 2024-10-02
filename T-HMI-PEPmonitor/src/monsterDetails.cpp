#include "monsterDetails.h"

uint8_t getMonsterSafariRarity(uint16_t monsterId) {
  char resBuffer[1024];
  getIniSection(String(MONSTERCATCHER_INI_PATH), String("[monster_") + String(monsterId) + String("]"), (char*)resBuffer, 1024);
  return atoi(getIniValueFromSection(resBuffer, "safariRarity").c_str());
}

uint16_t getSafariMonster(uint8_t targetRarity) {
  uint16_t id = 0;
  targetRarity = _min(3, _max(1, targetRarity));
  while (id == 0) {
    id = random(1,TOTAL_MONSTER_NUMBER+1);
    uint8_t monsterRarity = getMonsterSafariRarity(id);
    if (monsterRarity == 0 || monsterRarity>targetRarity) {
      id = 0;
    }
  }
  return id;
}

AttackFunctionType getAttackFunctionFromIdentifier(String attackFunctionIdentifier) {
  Serial.print("Getting attack for ");
  Serial.println(attackFunctionIdentifier);
  if (attackFunctionIdentifier == "confusion") {
    return attackFunction_confusion;
  } else if (attackFunctionIdentifier = "ember") {
    return attackFunction_ember;
  } else if (attackFunctionIdentifier = "throwFastAnim") {
    return attackFunction_throwFastAnim;
  } else if (attackFunctionIdentifier = "scratch") {
    return attackFunction_scratch;
  } else if (attackFunctionIdentifier = "lightning") {
    return attackFunction_lightning;
  } else if (attackFunctionIdentifier = "catch") {
    return attackFunction_catch;
  } else if (attackFunctionIdentifier = "rareCandy") {
    return attackFunction_rareCandy;
  }
  return NULL;
}

void loadAttackData(AttackData* attackData, String attackIdentifier) {
  char resBuffer[1024];
  getIniSection(String(MONSTERCATCHER_INI_PATH), String("[attack_") + attackIdentifier + String("]"), (char*)resBuffer, 1024);
  attackData->identifier = attackIdentifier;
  attackData->animFrames = atoi(getIniValueFromSection(resBuffer, "animFrames").c_str());
  attackData->imagePath = getIniValueFromSection(resBuffer, "imagePath");
  attackData->attackFunction = getAttackFunctionFromIdentifier(getIniValueFromSection(resBuffer, "attackIdentifier"));
  Serial.println("Attack data");
  Serial.println(attackData->identifier);
  Serial.println(attackData->animFrames);
  Serial.println(attackData->imagePath);
}

void loadMonsterData(MonsterData* monsterData, uint16_t monsterId) {
  Serial.println("LOAD MONSTER DATA");
  String attackIdentifier;
  {
    char resBuffer[1024];
    getIniSection(String(MONSTERCATCHER_INI_PATH), String("[monster_") + String(monsterId) + String("]"), (char*)resBuffer, 1024);
    monsterData->id = monsterId;
    monsterData->name = getIniValueFromSection(resBuffer, "name");
    monsterData->imagePath = getIniValueFromSection(resBuffer, "imagePath");
    String isBasicMonster = getIniValueFromSection(resBuffer, "basicMonster");
    monsterData->isBasicMonster = (isBasicMonster == "True") || (isBasicMonster == "true") || (isBasicMonster == "1");
    monsterData->evolvesTo = atoi(getIniValueFromSection(resBuffer, "evolvesTo").c_str());
    monsterData->safariRarity = atoi(getIniValueFromSection(resBuffer, "safariRarity").c_str());
    attackIdentifier = getIniValueFromSection(resBuffer, "attackIdentifier");
  }
  loadAttackData(&(monsterData->attack), attackIdentifier);
}