#include "monsterDetails.h"

bool isBasicMonster(uint16_t monsterId) {
  return !!(monsterEvolvesTo[monsterId] & BASIC_MONSTER);
}

uint16_t getMonsterEvolvesTo(uint16_t monsterId) {
  if (monsterId == 133) {
    return random(134,137);
  }
  return (monsterEvolvesTo[monsterId] & ~BASIC_MONSTER) & ~SAFARI_MONSTER_RARITY_2;
}

uint8_t getMonsterSafariRarity(uint16_t monsterId) {
  uint8_t rarity = 0;
  switch (monsterEvolvesTo[monsterId] & SAFARI_MONSTER_RARITY_3) {
    case SAFARI_MONSTER_RARITY_1:
      rarity = 1;
      break;
    case SAFARI_MONSTER_RARITY_2:
      rarity = 2;
      break;
    case SAFARI_MONSTER_RARITY_3:
      rarity = 3;
      break;
  }
  return rarity;
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

const String monsterName[] = {
  "Missingno", //0000
  "Bisasam", //0001
  "Bisaknosp", //0002
  "Bisaflor", //0003
  "Glumanda", //0004
  "Glutexo", //0005
  "Glurak", //0006
  "Schiggy", //0007
  "Schillok", //0008
  "Turtok", //0009
  "Raupy", //0010
  "Safcon", //0011
  "Smettbo", //0012
  "Hornliu", //0013
  "Kokuna", //0014
  "Bibor", //0015
  "Taubsi", //0016
  "Tauboga", //0017
  "Tauboss", //0018
  "Rattfratz", //0019
  "Rattikarl", //0020
  "Habitak", //0021
  "Ibitak", //0022
  "Rettan", //0023
  "Arbok", //0024
  "Pikachu", //0025
  "Raichu", //0026
  "Sandan", //0027
  "Sandamer", //0028
  "Nidoran W", //0029
  "Nidorina", //0030
  "Nidoqueen", //0031
  "Nidoran M", //0032
  "Nidorino", //0033
  "Nidoking", //0034
  "Piepi", //0035
  "Pixi", //0036
  "Vulpix", //0037
  "Vulnona", //0038
  "Pummeluff", //0039
  "Knuddeluff", //0040
  "Zubat", //0041
  "Golbat", //0042
  "Myrapla", //0043
  "Duflor", //0044
  "Giflor", //0045
  "Paras", //0046
  "Parasek", //0047
  "Bluzuk", //0048
  "Omot", //0049
  "Digda", //0050
  "Digdri", //0051
  "Mauzi", //0052
  "Snobilikat", //0053
  "Enton", //0054
  "Entoron", //0055
  "Menki", //0056
  "Rasaff", //0057
  "Fukano", //0058
  "Arkani", //0059
  "Quapsel", //0060
  "Quaputzi", //0061
  "Quappo", //0062
  "Abra", //0063
  "Kadabra", //0064
  "Simsala", //0065
  "Machollo", //0066
  "Maschock", //0067
  "Machomei", //0068
  "Knofensa", //0069
  "Ultrigaria", //0070
  "Sarzenia", //0071
  "Tentacha", //0072
  "Tentoxa", //0073
  "Kleinstein", //0074
  "Georok", //0075
  "Geowaz", //0076
  "Ponita", //0077
  "Gallopa", //0078
  "Flegmon", //0079
  "Lahmus", //0080
  "Magnetilo", //0081
  "Magneton", //0082
  "Porenta", //0083
  "Dodu", //0084
  "Dodri", //0085
  "Jurob", //0086
  "Jugong", //0087
  "Sleima", //0088
  "Sleimok", //0089
  "Muschas", //0090
  "Austos", //0091
  "Nebulak", //0092
  "Alpollo", //0093
  "Gengar", //0094
  "Onix", //0095
  "Traumato", //0096
  "Hypno", //0097
  "Krabby", //0098
  "Kingler", //0099
  "Voltobal", //0100
  "Lektrobal", //0101
  "Owei", //0102
  "Kokowei", //0103
  "Tragosso", //0104
  "Knogga", //0105
  "Kicklee", //0106
  "Nockchan", //0107
  "Schlurp", //0108
  "Smogon", //0109
  "Smogmog", //0110
  "Rihorn", //0111
  "Rizeros", //0112
  "Chaneira", //0113
  "Tangela", //0114
  "Kangama", //0115
  "Seeper", //0116
  "Seemon", //0117
  "Goldini", //0118
  "Golking", //0119
  "Sterndu", //0120
  "Starmie", //0121
  "Pantimos", //0122
  "Sichlor", //0123
  "Rossana", //0124
  "Elektek", //0125
  "Magmar", //0126
  "Pinsir", //0127
  "Tauros", //0128
  "Karpador", //0129
  "Garados", //0130
  "Lapras", //0131
  "Ditto", //0132
  "Evoli", //0133
  "Aquana", //0134
  "Blitza", //0135
  "Flamara", //0136
  "Porygon", //0137
  "Amonitas", //0138
  "Amoroso", //0139
  "Kabuto", //0140
  "Kabutops", //0141
  "Aerodactyl", //0142
  "Relaxo", //0143
  "Arktos", //0144
  "Zapdos", //0145
  "Lavados", //0146
  "Dratini", //0147
  "Dragonir", //0148
  "Dragoran", //0149
  "Mewtu", //0150
  "Mew", //0151
  "Endivie", //0152
  "Lorblatt", //0153
  "Meganie", //0154
  "Feurigel", //0155
  "Igelavar", //0156
  "Tornupto", //0157
  "Karnimani", //0158
  "Tyracroc", //0159
  "Impergator", //0160
  "Wiesor", //0161
  "Wiesenior", //0162
  "Hoothoot", //0163
  "Noctuh", //0164
  "Ledyba", //0165
  "Ledian", //0166
  "Webarak", //0167
  "Ariados", //0168
  "Iksbat", //0169
  "Lampi", //0170
  "Lanturn", //0171
  "Pichu", //0172
  "Pii", //0173
  "Fluffeluff", //0174
  "Togepi", //0175
  "Togetic", //0176
  "Natu", //0177
  "Xatu", //0178
  "Voltilamm", //0179
  "Waaty", //0180
  "Ampharos", //0181
  "Blubella", //0182
  "Marill", //0183
  "Azumarill", //0184
  "Mogelbaum", //0185
  "Quaxo", //0186
  "Hoppspross", //0187
  "Hubelupf", //0188
  "Papungha", //0189
  "Griffel", //0190
  "Sonnkern", //0191
  "Sonnflora", //0192
  "Yanma", //0193
  "Felino", //0194
  "Morlord", //0195
  "Psiana", //0196
  "Nachtara", //0197
  "Kramurx", //0198
  "Laschoking", //0199
  "Traunfugil", //0200
  "Icognito", //0201
  "Woingenau", //0202
  "Girafarig", //0203
  "Tannza", //0204
  "Forstellka", //0205
  "Dummisel", //0206
  "Skorgla", //0207
  "Stahlos", //0208
  "Snubbull", //0209
  "Granbull", //0210
  "Baldorfish", //0211
  "Scherox", //0212
  "Pottrott", //0213
  "Skaraborn", //0214
  "Sniebel", //0215
  "Teddiursa", //0216
  "Ursaring", //0217
  "Schneckmag", //0218
  "Magcargo", //0219
  "Quiekel", //0220
  "Keifel", //0221
  "Corasonn", //0222
  "Remoraid", //0223
  "Octillery", //0224
  "Botogel", //0225
  "Mantax", //0226
  "Panzaeron", //0227
  "Hunduster", //0228
  "Hundemon", //0229
  "Seedraking", //0230
  "Phanpy", //0231
  "Donphan", //0232
  "Porygon2", //0233
  "Damhirplex", //0234
  "Farbeagle", //0235
  "Rabauz", //0236
  "Kapoera", //0237
  "Kussilla", //0238
  "Elekid", //0239
  "Magby", //0240
  "Miltank", //0241
  "Heiteira", //0242
  "Raikou", //0243
  "Entei", //0244
  "Suicune", //0245
  "Larvitar", //0246
  "Pupitar", //0247
  "Despotar", //0248
  "Lugia", //0249
  "Ho-Oh", //0250
  "Celebi", //0251
  "Geckarbor", //0252
  "Reptain", //0253
  "Gewaldro", //0254
  "Flemmli", //0255
  "Jungglut", //0256
  "Lohgock", //0257
  "Hydropi", //0258
  "Moorabbel", //0259
  "Sumpex", //0260
  "Fiffyen", //0261
  "Magnayen", //0262
  "Zigzachs", //0263
  "Geradaks", //0264
  "Waumpel", //0265
  "Schaloko", //0266
  "Papinella", //0267
  "Panekon", //0268
  "Pudox", //0269
  "Loturzel", //0270
  "Lombrero", //0271
  "Kappalores", //0272
  "Samurzel", //0273
  "Blanas", //0274
  "Tengulist", //0275
  "Schwalbini", //0276
  "Schwalboss", //0277
  "Wingull", //0278
  "Pelipper", //0279
  "Trasla", //0280
  "Kirlia", //0281
  "Guardevoir", //0282
  "Gehweiher", //0283
  "Maskeregen", //0284
  "Knilz", //0285
  "Kapilz", //0286
  "Bummelz", //0287
  "Muntier", //0288
  "Letarking", //0289
  "Nincada", //0290
  "Ninjask", //0291
  "Ninjatom", //0292
  "Flurmel", //0293
  "Krakeelo", //0294
  "Krawumms", //0295
  "Makuhita", //0296
  "Hariyama", //0297
  "Azurill", //0298
  "Nasgnet", //0299
  "Eneco", //0300
  "Enekoro", //0301
  "Zobiris", //0302
  "Flunkifer", //0303
  "Stollunior", //0304
  "Stollrak", //0305
  "Stolloss", //0306
  "Meditie", //0307
  "Meditalis", //0308
  "Frizelbliz", //0309
  "Voltenso", //0310
  "Plusle", //0311
  "Minun", //0312
  "Volbeat", //0313
  "Illumise", //0314
  "Roselia", //0315
  "Schluppuck", //0316
  "Schlukwech", //0317
  "Kanivanha", //0318
  "Tohaido", //0319
  "Wailmer", //0320
  "Wailord", //0321
  "Camaub", //0322
  "Camerupt", //0323
  "Qurtel", //0324
  "Spoink", //0325
  "Groink", //0326
  "Pandir", //0327
  "Knacklion", //0328
  "Vibrava", //0329
  "Libelldra", //0330
  "Tuska", //0331
  "Noktuska", //0332
  "Wablu", //0333
  "Altaria", //0334
  "Sengo", //0335
  "Vipitis", //0336
  "Lunastein", //0337
  "Sonnfel", //0338
  "Schmerbe", //0339
  "Welsar", //0340
  "Krebscorps", //0341
  "Krebutack", //0342
  "Puppance", //0343
  "Lepumentas", //0344
  "Liliep", //0345
  "Wielie", //0346
  "Anorith", //0347
  "Armaldo", //0348
  "Barschwa", //0349
  "Milotic", //0350
  "Formeo", //0351
  "Kecleon", //0352
  "Shuppet", //0353
  "Banette", //0354
  "Zwirrlicht", //0355
  "Zwirrklop", //0356
  "Tropius", //0357
  "Palimpalim", //0358
  "Absol", //0359
  "Isso", //0360
  "Schneppke", //0361
  "Firnontor", //0362
  "Seemops", //0363
  "Seejong", //0364
  "Walraisa", //0365
  "Perlu", //0366
  "Aalabyss", //0367
  "Saganabyss", //0368
  "Relicanth", //0369
  "Liebiskus", //0370
  "Kindwurm", //0371
  "Draschel", //0372
  "Brutalanda", //0373
  "Tanhel", //0374
  "Metang", //0375
  "Metagross", //0376
  "Regirock", //0377
  "Regice", //0378
  "Registeel", //0379
  "Latias", //0380
  "Latios", //0381
  "Kyogre", //0382
  "Groudon", //0383
  "Rayquaza", //0384
  "Jirachi", //0385
  "Deoxys", //0386
};


const String monsterImagePath[] = {
  "/gfx/monsters/missingno", //  invalid placeholder
  "/gfx/monsters/bulbasaur", //0001
  "/gfx/monsters/ivysaur", //0002
  "/gfx/monsters/venusaur", //0003
  "/gfx/monsters/charmander", //0004
  "/gfx/monsters/charmeleon", //0005
  "/gfx/monsters/charizard", //0006
  "/gfx/monsters/squirtle", //0007
  "/gfx/monsters/wartortle", //0008
  "/gfx/monsters/blastoise", //0009
  "/gfx/monsters/caterpie", //0010
  "/gfx/monsters/metapod", //0011
  "/gfx/monsters/butterfree", //0012
  "/gfx/monsters/weedle", //0013
  "/gfx/monsters/kakuna", //0014
  "/gfx/monsters/beedrill", //0015
  "/gfx/monsters/pidgey", //0016
  "/gfx/monsters/pidgeotto", //0017
  "/gfx/monsters/pidgeot", //0018
  "/gfx/monsters/rattata", //0019
  "/gfx/monsters/raticate", //0020
  "/gfx/monsters/spearow", //0021
  "/gfx/monsters/fearow", //0022
  "/gfx/monsters/ekans", //0023
  "/gfx/monsters/arbok", //0024
  "/gfx/monsters/pikachu", //0025
  "/gfx/monsters/raichu", //0026
  "/gfx/monsters/sandshrew", //0027
  "/gfx/monsters/sandslash", //0028
  "/gfx/monsters/nidoran_f", //0029
  "/gfx/monsters/nidorina", //0030
  "/gfx/monsters/nidoqueen", //0031
  "/gfx/monsters/nidoran_m", //0032
  "/gfx/monsters/nidorino", //0033
  "/gfx/monsters/nidoking", //0034
  "/gfx/monsters/clefairy", //0035
  "/gfx/monsters/clefable", //0036
  "/gfx/monsters/vulpix", //0037
  "/gfx/monsters/ninetales", //0038
  "/gfx/monsters/jigglypuff", //0039
  "/gfx/monsters/wigglytuff", //0040
  "/gfx/monsters/zubat", //0041
  "/gfx/monsters/golbat", //0042
  "/gfx/monsters/oddish", //0043
  "/gfx/monsters/gloom", //0044
  "/gfx/monsters/vileplume", //0045
  "/gfx/monsters/paras", //0046
  "/gfx/monsters/parasect", //0047
  "/gfx/monsters/venonat", //0048
  "/gfx/monsters/venomoth", //0049
  "/gfx/monsters/diglett", //0050
  "/gfx/monsters/dugtrio", //0051
  "/gfx/monsters/meowth", //0052
  "/gfx/monsters/persian", //0053
  "/gfx/monsters/psyduck", //0054
  "/gfx/monsters/golduck", //0055
  "/gfx/monsters/mankey", //0056
  "/gfx/monsters/primeape", //0057
  "/gfx/monsters/growlithe", //0058
  "/gfx/monsters/arcanine", //0059
  "/gfx/monsters/poliwag", //0060
  "/gfx/monsters/poliwhirl", //0061
  "/gfx/monsters/poliwrath", //0062
  "/gfx/monsters/abra", //0063
  "/gfx/monsters/kadabra", //0064
  "/gfx/monsters/alakazam", //0065
  "/gfx/monsters/machop", //0066
  "/gfx/monsters/machoke", //0067
  "/gfx/monsters/machamp", //0068
  "/gfx/monsters/bellsprout", //0069
  "/gfx/monsters/weepinbell", //0070
  "/gfx/monsters/victreebel", //0071
  "/gfx/monsters/tentacool", //0072
  "/gfx/monsters/tentacruel", //0073
  "/gfx/monsters/geodude", //0074
  "/gfx/monsters/graveler", //0075
  "/gfx/monsters/golem", //0076
  "/gfx/monsters/ponyta", //0077
  "/gfx/monsters/rapidash", //0078
  "/gfx/monsters/slowpoke", //0079
  "/gfx/monsters/slowbro", //0080
  "/gfx/monsters/magnemite", //0081
  "/gfx/monsters/magneton", //0082
  "/gfx/monsters/farfetch�d", //0083
  "/gfx/monsters/doduo", //0084
  "/gfx/monsters/dodrio", //0085
  "/gfx/monsters/seel", //0086
  "/gfx/monsters/dewgong", //0087
  "/gfx/monsters/grimer", //0088
  "/gfx/monsters/muk", //0089
  "/gfx/monsters/shellder", //0090
  "/gfx/monsters/cloyster", //0091
  "/gfx/monsters/gastly", //0092
  "/gfx/monsters/haunter", //0093
  "/gfx/monsters/gengar", //0094
  "/gfx/monsters/onix", //0095
  "/gfx/monsters/drowzee", //0096
  "/gfx/monsters/hypno", //0097
  "/gfx/monsters/krabby", //0098
  "/gfx/monsters/kingler", //0099
  "/gfx/monsters/voltorb", //0100
  "/gfx/monsters/electrode", //0101
  "/gfx/monsters/exeggcute", //0102
  "/gfx/monsters/exeggutor", //0103
  "/gfx/monsters/cubone", //0104
  "/gfx/monsters/marowak", //0105
  "/gfx/monsters/hitmonlee", //0106
  "/gfx/monsters/hitmonchan", //0107
  "/gfx/monsters/lickitung", //0108
  "/gfx/monsters/koffing", //0109
  "/gfx/monsters/weezing", //0110
  "/gfx/monsters/rhyhorn", //0111
  "/gfx/monsters/rhydon", //0112
  "/gfx/monsters/chansey", //0113
  "/gfx/monsters/tangela", //0114
  "/gfx/monsters/kangaskhan", //0115
  "/gfx/monsters/horsea", //0116
  "/gfx/monsters/seadra", //0117
  "/gfx/monsters/goldeen", //0118
  "/gfx/monsters/seaking", //0119
  "/gfx/monsters/staryu", //0120
  "/gfx/monsters/starmie", //0121
  "/gfx/monsters/mr_mime", //0122
  "/gfx/monsters/scyther", //0123
  "/gfx/monsters/jynx", //0124
  "/gfx/monsters/electabuzz", //0125
  "/gfx/monsters/magmar", //0126
  "/gfx/monsters/pinsir", //0127
  "/gfx/monsters/tauros", //0128
  "/gfx/monsters/magikarp", //0129
  "/gfx/monsters/gyarados", //0130
  "/gfx/monsters/lapras", //0131
  "/gfx/monsters/ditto", //0132
  "/gfx/monsters/eevee", //0133
  "/gfx/monsters/vaporeon", //0134
  "/gfx/monsters/jolteon", //0135
  "/gfx/monsters/flareon", //0136
  "/gfx/monsters/porygon", //0137
  "/gfx/monsters/omanyte", //0138
  "/gfx/monsters/omastar", //0139
  "/gfx/monsters/kabuto", //0140
  "/gfx/monsters/kabutops", //0141
  "/gfx/monsters/aerodactyl", //0142
  "/gfx/monsters/snorlax", //0143
  "/gfx/monsters/articuno", //0144
  "/gfx/monsters/zapdos", //0145
  "/gfx/monsters/moltres", //0146
  "/gfx/monsters/dratini", //0147
  "/gfx/monsters/dragonair", //0148
  "/gfx/monsters/dragonite", //0149
  "/gfx/monsters/mewtwo", //0150
  "/gfx/monsters/mew", //0151
  "/gfx/monsters/chikorita", //0152
  "/gfx/monsters/bayleef", //0153
  "/gfx/monsters/meganium", //0154
  "/gfx/monsters/cyndaquil", //0155
  "/gfx/monsters/quilava", //0156
  "/gfx/monsters/typhlosion", //0157
  "/gfx/monsters/totodile", //0158
  "/gfx/monsters/croconaw", //0159
  "/gfx/monsters/feraligatr", //0160
  "/gfx/monsters/sentret", //0161
  "/gfx/monsters/furret", //0162
  "/gfx/monsters/hoothoot", //0163
  "/gfx/monsters/noctowl", //0164
  "/gfx/monsters/ledyba", //0165
  "/gfx/monsters/ledian", //0166
  "/gfx/monsters/spinarak", //0167
  "/gfx/monsters/ariados", //0168
  "/gfx/monsters/crobat", //0169
  "/gfx/monsters/chinchou", //0170
  "/gfx/monsters/lanturn", //0171
  "/gfx/monsters/pichu", //0172
  "/gfx/monsters/cleffa", //0173
  "/gfx/monsters/igglybuff", //0174
  "/gfx/monsters/togepi", //0175
  "/gfx/monsters/togetic", //0176
  "/gfx/monsters/natu", //0177
  "/gfx/monsters/xatu", //0178
  "/gfx/monsters/mareep", //0179
  "/gfx/monsters/flaaffy", //0180
  "/gfx/monsters/ampharos", //0181
  "/gfx/monsters/bellossom", //0182
  "/gfx/monsters/marill", //0183
  "/gfx/monsters/azumarill", //0184
  "/gfx/monsters/sudowoodo", //0185
  "/gfx/monsters/politoed", //0186
  "/gfx/monsters/hoppip", //0187
  "/gfx/monsters/skiploom", //0188
  "/gfx/monsters/jumpluff", //0189
  "/gfx/monsters/aipom", //0190
  "/gfx/monsters/sunkern", //0191
  "/gfx/monsters/sunflora", //0192
  "/gfx/monsters/yanma", //0193
  "/gfx/monsters/wooper", //0194
  "/gfx/monsters/quagsire", //0195
  "/gfx/monsters/espeon", //0196
  "/gfx/monsters/umbreon", //0197
  "/gfx/monsters/murkrow", //0198
  "/gfx/monsters/slowking", //0199
  "/gfx/monsters/misdreavus", //0200
  "/gfx/monsters/unown", //0201
  "/gfx/monsters/wobbuffet", //0202
  "/gfx/monsters/girafarig", //0203
  "/gfx/monsters/pineco", //0204
  "/gfx/monsters/forretress", //0205
  "/gfx/monsters/dunsparce", //0206
  "/gfx/monsters/gligar", //0207
  "/gfx/monsters/steelix", //0208
  "/gfx/monsters/snubbull", //0209
  "/gfx/monsters/granbull", //0210
  "/gfx/monsters/qwilfish", //0211
  "/gfx/monsters/scizor", //0212
  "/gfx/monsters/shuckle", //0213
  "/gfx/monsters/heracross", //0214
  "/gfx/monsters/sneasel", //0215
  "/gfx/monsters/teddiursa", //0216
  "/gfx/monsters/ursaring", //0217
  "/gfx/monsters/slugma", //0218
  "/gfx/monsters/magcargo", //0219
  "/gfx/monsters/swinub", //0220
  "/gfx/monsters/piloswine", //0221
  "/gfx/monsters/corsola", //0222
  "/gfx/monsters/remoraid", //0223
  "/gfx/monsters/octillery", //0224
  "/gfx/monsters/delibird", //0225
  "/gfx/monsters/mantine", //0226
  "/gfx/monsters/skarmory", //0227
  "/gfx/monsters/houndour", //0228
  "/gfx/monsters/houndoom", //0229
  "/gfx/monsters/kingdra", //0230
  "/gfx/monsters/phanpy", //0231
  "/gfx/monsters/donphan", //0232
  "/gfx/monsters/porygon2", //0233
  "/gfx/monsters/stantler", //0234
  "/gfx/monsters/smeargle", //0235
  "/gfx/monsters/tyrogue", //0236
  "/gfx/monsters/hitmontop", //0237
  "/gfx/monsters/smoochum", //0238
  "/gfx/monsters/elekid", //0239
  "/gfx/monsters/magby", //0240
  "/gfx/monsters/miltank", //0241
  "/gfx/monsters/blissey", //0242
  "/gfx/monsters/raikou", //0243
  "/gfx/monsters/entei", //0244
  "/gfx/monsters/suicune", //0245
  "/gfx/monsters/larvitar", //0246
  "/gfx/monsters/pupitar", //0247
  "/gfx/monsters/tyranitar", //0248
  "/gfx/monsters/lugia", //0249
  "/gfx/monsters/ho-oh", //0250
  "/gfx/monsters/celebi", //0251
  "/gfx/monsters/treecko", //0252
  "/gfx/monsters/grovyle", //0253
  "/gfx/monsters/sceptile", //0254
  "/gfx/monsters/torchic", //0255
  "/gfx/monsters/combusken", //0256
  "/gfx/monsters/blaziken", //0257
  "/gfx/monsters/mudkip", //0258
  "/gfx/monsters/marshtomp", //0259
  "/gfx/monsters/swampert", //0260
  "/gfx/monsters/poochyena", //0261
  "/gfx/monsters/mightyena", //0262
  "/gfx/monsters/zigzagoon", //0263
  "/gfx/monsters/linoone", //0264
  "/gfx/monsters/wurmple", //0265
  "/gfx/monsters/silcoon", //0266
  "/gfx/monsters/beautifly", //0267
  "/gfx/monsters/cascoon", //0268
  "/gfx/monsters/dustox", //0269
  "/gfx/monsters/lotad", //0270
  "/gfx/monsters/lombre", //0271
  "/gfx/monsters/ludicolo", //0272
  "/gfx/monsters/seedot", //0273
  "/gfx/monsters/nuzleaf", //0274
  "/gfx/monsters/shiftry", //0275
  "/gfx/monsters/taillow", //0276
  "/gfx/monsters/swellow", //0277
  "/gfx/monsters/wingull", //0278
  "/gfx/monsters/pelipper", //0279
  "/gfx/monsters/ralts", //0280
  "/gfx/monsters/kirlia", //0281
  "/gfx/monsters/gardevoir", //0282
  "/gfx/monsters/surskit", //0283
  "/gfx/monsters/masquerain", //0284
  "/gfx/monsters/shroomish", //0285
  "/gfx/monsters/breloom", //0286
  "/gfx/monsters/slakoth", //0287
  "/gfx/monsters/vigoroth", //0288
  "/gfx/monsters/slaking", //0289
  "/gfx/monsters/nincada", //0290
  "/gfx/monsters/ninjask", //0291
  "/gfx/monsters/shedinja", //0292
  "/gfx/monsters/whismur", //0293
  "/gfx/monsters/loudred", //0294
  "/gfx/monsters/exploud", //0295
  "/gfx/monsters/makuhita", //0296
  "/gfx/monsters/hariyama", //0297
  "/gfx/monsters/azurill", //0298
  "/gfx/monsters/nosepass", //0299
  "/gfx/monsters/skitty", //0300
  "/gfx/monsters/delcatty", //0301
  "/gfx/monsters/sableye", //0302
  "/gfx/monsters/mawile", //0303
  "/gfx/monsters/aron", //0304
  "/gfx/monsters/lairon", //0305
  "/gfx/monsters/aggron", //0306
  "/gfx/monsters/meditite", //0307
  "/gfx/monsters/medicham", //0308
  "/gfx/monsters/electrike", //0309
  "/gfx/monsters/manectric", //0310
  "/gfx/monsters/plusle", //0311
  "/gfx/monsters/minun", //0312
  "/gfx/monsters/volbeat", //0313
  "/gfx/monsters/illumise", //0314
  "/gfx/monsters/roselia", //0315
  "/gfx/monsters/gulpin", //0316
  "/gfx/monsters/swalot", //0317
  "/gfx/monsters/carvanha", //0318
  "/gfx/monsters/sharpedo", //0319
  "/gfx/monsters/wailmer", //0320
  "/gfx/monsters/wailord", //0321
  "/gfx/monsters/numel", //0322
  "/gfx/monsters/camerupt", //0323
  "/gfx/monsters/torkoal", //0324
  "/gfx/monsters/spoink", //0325
  "/gfx/monsters/grumpig", //0326
  "/gfx/monsters/spinda", //0327
  "/gfx/monsters/trapinch", //0328
  "/gfx/monsters/vibrava", //0329
  "/gfx/monsters/flygon", //0330
  "/gfx/monsters/cacnea", //0331
  "/gfx/monsters/cacturne", //0332
  "/gfx/monsters/swablu", //0333
  "/gfx/monsters/altaria", //0334
  "/gfx/monsters/zangoose", //0335
  "/gfx/monsters/seviper", //0336
  "/gfx/monsters/lunatone", //0337
  "/gfx/monsters/solrock", //0338
  "/gfx/monsters/barboach", //0339
  "/gfx/monsters/whiscash", //0340
  "/gfx/monsters/corphish", //0341
  "/gfx/monsters/crawdaunt", //0342
  "/gfx/monsters/baltoy", //0343
  "/gfx/monsters/claydol", //0344
  "/gfx/monsters/lileep", //0345
  "/gfx/monsters/cradily", //0346
  "/gfx/monsters/anorith", //0347
  "/gfx/monsters/armaldo", //0348
  "/gfx/monsters/feebas", //0349
  "/gfx/monsters/milotic", //0350
  "/gfx/monsters/castform", //0351
  "/gfx/monsters/kecleon", //0352
  "/gfx/monsters/shuppet", //0353
  "/gfx/monsters/banette", //0354
  "/gfx/monsters/duskull", //0355
  "/gfx/monsters/dusclops", //0356
  "/gfx/monsters/tropius", //0357
  "/gfx/monsters/chimecho", //0358
  "/gfx/monsters/absol", //0359
  "/gfx/monsters/wynaut", //0360
  "/gfx/monsters/snorunt", //0361
  "/gfx/monsters/glalie", //0362
  "/gfx/monsters/spheal", //0363
  "/gfx/monsters/sealeo", //0364
  "/gfx/monsters/walrein", //0365
  "/gfx/monsters/clamperl", //0366
  "/gfx/monsters/huntail", //0367
  "/gfx/monsters/gorebyss", //0368
  "/gfx/monsters/relicanth", //0369
  "/gfx/monsters/luvdisc", //0370
  "/gfx/monsters/bagon", //0371
  "/gfx/monsters/shelgon", //0372
  "/gfx/monsters/salamence", //0373
  "/gfx/monsters/beldum", //0374
  "/gfx/monsters/metang", //0375
  "/gfx/monsters/metagross", //0376
  "/gfx/monsters/regirock", //0377
  "/gfx/monsters/regice", //0378
  "/gfx/monsters/registeel", //0379
  "/gfx/monsters/latias", //0380
  "/gfx/monsters/latios", //0381
  "/gfx/monsters/kyogre", //0382
  "/gfx/monsters/groudon", //0383
  "/gfx/monsters/rayquaza", //0384
  "/gfx/monsters/jirachi", //0385
  "/gfx/monsters/deoxys", //0386
};


const uint16_t monsterEvolvesTo[] = {
  0, // invalid placeholder
  2 | BASIC_MONSTER, // 0001, Bisasam
  3, // 0002, Bisaknosp
  0, // 0003, Bisaflor
  5 | BASIC_MONSTER, // 0004, Glumanda
  6, // 0005, Glutexo
  0, // 0006, Glurak
  8 | BASIC_MONSTER, // 0007, Schiggy
  9, // 0008, Schillok
  0, // 0009, Turtok
  11 | BASIC_MONSTER, // 0010, Raupy
  12, // 0011, Safcon
  0, // 0012, Smettbo
  14 | BASIC_MONSTER, // 0013, Hornliu
  15, // 0014, Kokuna
  0, // 0015, Bibor
  17 | BASIC_MONSTER, // 0016, Taubsi
  18, // 0017, Tauboga
  0, // 0018, Tauboss
  20 | BASIC_MONSTER, // 0019, Rattfratz
  0, // 0020, Rattikarl
  22 | BASIC_MONSTER, // 0021, Habitak
  0 | SAFARI_MONSTER_RARITY_2, // 0022, Ibitak
  24 | BASIC_MONSTER, // 0023, Rettan
  0, // 0024, Arbok
  26 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0025, Pikachu
  0, // 0026, Raichu
  28 | BASIC_MONSTER, // 0027, Sandan
  0, // 0028, Sandamer
  30 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0029, Nidoran♀
  31 | SAFARI_MONSTER_RARITY_2, // 0030, Nidorina
  0, // 0031, Nidoqueen
  33 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0032, Nidoran♂
  34 | SAFARI_MONSTER_RARITY_2, // 0033, Nidorino
  0, // 0034, Nidoking
  36 | BASIC_MONSTER, // 0035, Piepi
  0, // 0036, Pixi
  38 | BASIC_MONSTER, // 0037, Vulpix
  0, // 0038, Vulnona
  40 | BASIC_MONSTER, // 0039, Pummeluff
  0, // 0040, Knuddeluff
  42 | BASIC_MONSTER, // 0041, Zubat
  0, // 0042, Golbat
  44 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0043, Myrapla
  45 | SAFARI_MONSTER_RARITY_2, // 0044, Duflor
  0, // 0045, Giflor
  47 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0046, Paras
  0 | SAFARI_MONSTER_RARITY_2, // 0047, Parasek
  49 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0048, Bluzuk
  0 | SAFARI_MONSTER_RARITY_2, // 0049, Omot
  51 | BASIC_MONSTER, // 0050, Digda
  0, // 0051, Digdri
  53 | BASIC_MONSTER, // 0052, Mauzi
  0, // 0053, Snobilikat
  55 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0054, Enton
  0 | SAFARI_MONSTER_RARITY_2, // 0055, Entoron
  57 | BASIC_MONSTER, // 0056, Menki
  0, // 0057, Rasaff
  59 | BASIC_MONSTER, // 0058, Fukano
  0, // 0059, Arkani
  61 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0060, Quapsel
  62, // 0061, Quaputzi
  0, // 0062, Quappo
  64 | BASIC_MONSTER, // 0063, Abra
  65, // 0064, Kadabra
  0, // 0065, Simsala
  67 | BASIC_MONSTER, // 0066, Machollo
  68, // 0067, Maschock
  0, // 0068, Machomei
  70 | BASIC_MONSTER, // 0069, Knofensa
  71, // 0070, Ultrigaria
  0, // 0071, Sarzenia
  73 | BASIC_MONSTER, // 0072, Tentacha
  0, // 0073, Tentoxa
  75 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0074, Kleinstein
  76 | SAFARI_MONSTER_RARITY_2, // 0075, Georok
  0, // 0076, Geowaz
  78 | BASIC_MONSTER, // 0077, Ponita
  0, // 0078, Gallopa
  80 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0079, Flegmon
  0 | SAFARI_MONSTER_RARITY_2, // 0080, Lahmus
  82 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0081, Magnetilo
  0 | SAFARI_MONSTER_RARITY_2, // 0082, Magneton
  0  | BASIC_MONSTER, // 0083, Porenta
  85 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0084, Doduprusa
  0 | SAFARI_MONSTER_RARITY_2, // 0085, Dodri
  87 | BASIC_MONSTER, // 0086, Jurob
  0, // 0087, Jugong
  89 | BASIC_MONSTER, // 0088, Sleima
  0, // 0089, Sleimok
  91 | BASIC_MONSTER, // 0090, Muschas
  0, // 0091, Austos
  93 | BASIC_MONSTER, // 0092, Nebulak
  94, // 0093, Alpollo
  0, // 0094, Gengar
  0 | BASIC_MONSTER, // 0095, Onix
  97 | BASIC_MONSTER, // 0096, Traumato
  0, // 0097, Hypno
  99 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0098, Krabby
  0, // 0099, Kingler
  101 | BASIC_MONSTER, // 0100, Voltobal
  0, // 0101, Lektrobal
  103 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0102, Owei
  0, // 0103, Kokowei
  105 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0104, Tragosso
  0 | SAFARI_MONSTER_RARITY_2, // 0105, Knogga
  0 | BASIC_MONSTER, // 0106, Kicklee
  0 | BASIC_MONSTER, // 0107, Nockchan
  0 | BASIC_MONSTER, // 0108, Schlurp
  110 | BASIC_MONSTER, // 0109, Smogon
  0, // 0110, Smogmog
  112 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0111, Rihorn
  0, // 0112, Rizeros
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0113, Chaneira
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0114, Tangela
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0115, Kangama
  117 | BASIC_MONSTER, // 0116, Seeper
  230, // 0117, Seemon
  119 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0118, Goldini
  0 | SAFARI_MONSTER_RARITY_2, // 0119, Golking
  121 | BASIC_MONSTER, // 0120, Sterndu
  0, // 0121, Starmie
  0 | BASIC_MONSTER, // 0122, Pantimos
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0123, Sichlor
  0 | BASIC_MONSTER, // 0124, Rossana
  0 | BASIC_MONSTER, // 0125, Elektek
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0126, Magmar
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0127, Pinsir
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0128, Tauros
  130 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0129, Karpador
  0, // 0130, Garados
  0 | BASIC_MONSTER, // 0131, Lapras
  0 | BASIC_MONSTER, // 0132, Ditto
  0 | BASIC_MONSTER, // 0133, Evoli  SPECIAL: Random evolution to 134, 135, 136 determined in getMonsterEvolvesTo()
  0, // 0134, Aquana
  0, // 0135, Blitza
  0, // 0136, Flamara
  0 | BASIC_MONSTER, // 0137, Porygon
  139 | BASIC_MONSTER, // 0138, Amonitas
  0, // 0139, Amoroso
  141 | BASIC_MONSTER, // 0140, Kabuto
  0, // 0141, Kabutops
  0 | BASIC_MONSTER, // 0142, Aerodactyl
  0 | BASIC_MONSTER, // 0143, Relaxo
  0 | BASIC_MONSTER, // 0144, Arktos
  0 | BASIC_MONSTER, // 0145, Zapdos
  0 | BASIC_MONSTER, // 0146, Lavados
  148 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0147, Dratini
  149 | SAFARI_MONSTER_RARITY_3, // 0148, Dragonir
  0, // 0149, Dragoran
  0 | BASIC_MONSTER, // 0150, Mewtu
  0 | BASIC_MONSTER, // 0151, Mew
  0, // 0152, Endivie
  0, // 0153, Lorblatt
  0, // 0154, Meganie
  0, // 0155, Feurigel
  0, // 0156, Igelavar
  0, // 0157, Tornupto
  0, // 0158, Karnimani
  0, // 0159, Tyracroc
  0, // 0160, Impergator
  0, // 0161, Wiesor
  0, // 0162, Wiesenior
  164 | BASIC_MONSTER |  SAFARI_MONSTER_RARITY_3, // 0163, Hoothoot
  0, // 0164, Noctuh
  166 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0165, Ledyba
  0, // 0166, Ledian
  168 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0167, Webarak
  0, // 0168, Ariados
  0, // 0169, Iksbat
  0, // 0170, Lampi
  0, // 0171, Lanturn
  0, // 0172, Pichu
  0, // 0173, Pii
  0, // 0174, Fluffeluff
  0, // 0175, Togepi
  0, // 0176, Togetic
  178 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0177, Natu
  0, // 0178, Xatu
  180 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0179, Voltilamm
  181, // 0180, Waaty
  0, // 0181, Ampharos
  0, // 0182, Blubella
  184 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0183, Marill
  0, // 0184, Azumarill
  0, // 0185, Mogelbaum
  0, // 0186, Quaxo
  0, // 0187, Hoppspross
  0, // 0188, Hubelupf
  0, // 0189, Papungha
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0190, Griffel
  192 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0191, Sonnkern
  0, // 0192, Sonnflora
  0, // 0193, Yanma
  195 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0194, Felino
  0 | SAFARI_MONSTER_RARITY_3, // 0195, Morlord
  0, // 0196, Psiana
  0, // 0197, Nachtara
  0, // 0198, Kramurx
  0, // 0199, Laschoking
  0, // 0200, Traunfugil
  0, // 0201, Icognito
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0202, Woingenau
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0203, Girafarig
  205 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0204, Tannza
  0, // 0205, Forstellka
  0, // 0206, Dummisel
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0207, Skorgla
  0, // 0208, Stahlos
  210 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0209, Snubbull
  0, // 0210, Granbull
  0, // 0211, Baldorfish
  0, // 0212, Scherox
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0213, Pottrott
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0214, Skaraborn
  0, // 0215, Sniebel
  217 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0216, Teddiursa
  0, // 0217, Ursaring
  0, // 0218, Schneckmag
  0, // 0219, Magcargo
  0, // 0220, Quiekel
  0, // 0221, Keifel
  0, // 0222, Corasonn
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_2, // 0223, Remoraid
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0224, Octillery
  0, // 0225, Botogel
  0, // 0226, Mantax
  0, // 0227, Panzaeron
  229 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0228, Hunduster
  0, // 0229, Hundemon
  0, // 0230, Seedraking
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_1, // 0231, Phanpy
  0, // 0232, Donphan
  0, // 0233, Porygon2
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0234, Damhirplex
  0, // 0235, Farbeagle
  0, // 0236, Rabauz
  0, // 0237, Kapoera
  0, // 0238, Kussilla
  0, // 0239, Elekid
  0, // 0240, Magby
  0 | BASIC_MONSTER | SAFARI_MONSTER_RARITY_3, // 0241, Miltank
  0, // 0242, Heiteira
  0, // 0243, Raikou
  0, // 0244, Entei
  0, // 0245, Suicune
  0, // 0246, Larvitar
  0, // 0247, Pupitar
  0, // 0248, Despotar
  0, // 0249, Lugia
  0, // 0250, Ho-Oh
  0, // 0251, Celebi
  0, // 0252, Geckarbor
  0, // 0253, Reptain
  0, // 0254, Gewaldro
  0, // 0255, Flemmli
  0, // 0256, Jungglut
  0, // 0257, Lohgock
  0, // 0258, Hydropi
  0, // 0259, Moorabbel
  0, // 0260, Sumpex
  0, // 0261, Fiffyen
  0, // 0262, Magnayen
  0, // 0263, Zigzachs
  0, // 0264, Geradaks
  0, // 0265, Waumpel
  0, // 0266, Schaloko
  0, // 0267, Papinella
  0, // 0268, Panekon
  0, // 0269, Pudox
  0, // 0270, Loturzel
  0, // 0271, Lombrero
  0, // 0272, Kappalores
  0, // 0273, Samurzel
  0, // 0274, Blanas
  0, // 0275, Tengulist
  0, // 0276, Schwalbini
  0, // 0277, Schwalboss
  0, // 0278, Wingull
  0, // 0279, Pelipper
  0, // 0280, Trasla
  0, // 0281, Kirlia
  0, // 0282, Guardevoir
  0, // 0283, Gehweiher
  0, // 0284, Maskeregen
  0, // 0285, Knilz
  0, // 0286, Kapilz
  0, // 0287, Bummelz
  0, // 0288, Muntier
  0, // 0289, Letarking
  0, // 0290, Nincada
  0, // 0291, Ninjask
  0, // 0292, Ninjatom
  0, // 0293, Flurmel
  0, // 0294, Krakeelo
  0, // 0295, Krawumms
  0, // 0296, Makuhita
  0, // 0297, Hariyama
  0, // 0298, Azurill
  0, // 0299, Nasgnet
  0, // 0300, Eneco
  0, // 0301, Enekoro
  0, // 0302, Zobiris
  0, // 0303, Flunkifer
  0, // 0304, Stollunior
  0, // 0305, Stollrak
  0, // 0306, Stolloss
  0, // 0307, Meditie
  0, // 0308, Meditalis
  0, // 0309, Frizelbliz
  0, // 0310, Voltenso
  0, // 0311, Plusle
  0, // 0312, Minun
  0, // 0313, Volbeat
  0, // 0314, Illumise
  0, // 0315, Roselia
  0, // 0316, Schluppuck
  0, // 0317, Schlukwech
  0, // 0318, Kanivanha
  0, // 0319, Tohaido
  0, // 0320, Wailmer
  0, // 0321, Wailord
  0, // 0322, Camaub
  0, // 0323, Camerupt
  0, // 0324, Qurtel
  0, // 0325, Spoink
  0, // 0326, Groink
  0, // 0327, Pandir
  0, // 0328, Knacklion
  0, // 0329, Vibrava
  0, // 0330, Libelldra
  0, // 0331, Tuska
  0, // 0332, Noktuska
  0, // 0333, Wablu
  0, // 0334, Altaria
  0, // 0335, Sengo
  0, // 0336, Vipitis
  0, // 0337, Lunastein
  0, // 0338, Sonnfel
  0, // 0339, Schmerbe
  0, // 0340, Welsar
  0, // 0341, Krebscorps
  0, // 0342, Krebutack
  0, // 0343, Puppance
  0, // 0344, Lepumentas
  0, // 0345, Liliep
  0, // 0346, Wielie
  0, // 0347, Anorith
  0, // 0348, Armaldo
  0, // 0349, Barschwa
  0, // 0350, Milotic
  0, // 0351, Formeo
  0, // 0352, Kecleon
  0, // 0353, Shuppet
  0, // 0354, Banette
  0, // 0355, Zwirrlicht
  0, // 0356, Zwirrklop
  0, // 0357, Tropius
  0, // 0358, Palimpalim
  0, // 0359, Absol
  0, // 0360, Isso
  0, // 0361, Schneppke
  0, // 0362, Firnontor
  0, // 0363, Seemops
  0, // 0364, Seejong
  0, // 0365, Walraisa
  0, // 0366, Perlu
  0, // 0367, Aalabyss
  0, // 0368, Saganabyss
  0, // 0369, Relicanth
  0, // 0370, Liebiskus
  0, // 0371, Kindwurm
  0, // 0372, Draschel
  0, // 0373, Brutalanda
  0, // 0374, Tanhel
  0, // 0375, Metang
  0, // 0376, Metagross
  0, // 0377, Regirock
  0, // 0378, Regice
  0, // 0379, Registeel
  0, // 0380, Latias
  0, // 0381, Latios
  0, // 0382, Kyogre
  0, // 0383, Groudon
  0, // 0384, Rayquaza
  0, // 0385, Jirachi
  0, // 0386, Deoxys
};


const uint8_t monsterAttackId[] = {
  0, // invalid placeholder
  ATTACK_ID_SEED, //0001, Bisasam
  ATTACK_ID_SEED, //0002, Bisaknosp
  ATTACK_ID_SEED, //0003, Bisaflor
  ATTACK_ID_EMBER, //0004, Glumanda
  ATTACK_ID_EMBER, //0005, Glutexo
  ATTACK_ID_EMBER, //0006, Glurak
  ATTACK_ID_WATER, //0007, Schiggy
  ATTACK_ID_WATER, //0008, Schillok
  ATTACK_ID_WATER, //0009, Turtok
  ATTACK_ID_STRINGSHOT, //0010, Raupy
  ATTACK_ID_STRINGSHOT, //0011, Safcon
  ATTACK_ID_STRINGSHOT, //0012, Smettbo
  ATTACK_ID_POISONSTING, //0013, Hornliu
  ATTACK_ID_POISONSTING, //0014, Kokuna
  ATTACK_ID_POISONSTING, //0015, Bibor
  ATTACK_ID_AIR_SLASH, //0016, Taubsi
  ATTACK_ID_AIR_SLASH, //0017, Tauboga
  ATTACK_ID_AIR_SLASH, //0018, Tauboss
  ATTACK_ID_SCRATCH, //0019, Rattfratz
  ATTACK_ID_SCRATCH, //0020, Rattikarl
  ATTACK_ID_AIR_SLASH, //0021, Habitak
  ATTACK_ID_AIR_SLASH, //0022, Ibitak
  ATTACK_ID_POISONSTING, //0023, Rettan
  ATTACK_ID_POISONSTING, //0024, Arbok
  ATTACK_ID_LIGHTNING, //0025, Pikachu
  ATTACK_ID_LIGHTNING, //0026, Raichu
  ATTACK_ID_SAND_TOMB, //0027, Sandan
  ATTACK_ID_SAND_TOMB, //0028, Sandamer
  ATTACK_ID_POISONSTING, //0029, Nidoran♀
  ATTACK_ID_POISONSTING, //0030, Nidorina
  ATTACK_ID_POISONSTING, //0031, Nidoqueen
  ATTACK_ID_POISONSTING, //0032, Nidoran♂
  ATTACK_ID_POISONSTING, //0033, Nidorino
  ATTACK_ID_POISONSTING, //0034, Nidoking
  ATTACK_ID_SING, //0035, Piepi
  ATTACK_ID_SING, //0036, Pixi
  ATTACK_ID_EMBER, //0037, Vulpix
  ATTACK_ID_EMBER, //0038, Vulnona
  ATTACK_ID_SING, //0039, Pummeluff
  ATTACK_ID_SING, //0040, Knuddeluff
  ATTACK_ID_POISONSTING, //0041, Zubat
  ATTACK_ID_POISONSTING, //0042, Golbat
  ATTACK_ID_SEED, //0043, Myrapla
  ATTACK_ID_SEED, //0044, Duflor
  ATTACK_ID_SEED, //0045, Giflor
  ATTACK_ID_SEED, //0046, Paras
  ATTACK_ID_SEED, //0047, Parasek
  ATTACK_ID_POISONSTING, //0048, Bluzuk
  ATTACK_ID_POISONSTING, //0049, Omot
  ATTACK_ID_SAND_TOMB, //0050, Digda
  ATTACK_ID_SAND_TOMB, //0051, Digdri
  ATTACK_ID_SCRATCH, //0052, Mauzi
  ATTACK_ID_SCRATCH, //0053, Snobilikat
  ATTACK_ID_WATER, //0054, Enton
  ATTACK_ID_WATER, //0055, Entoron
  ATTACK_ID_KARATE_CHOP, //0056, Menki
  ATTACK_ID_KARATE_CHOP, //0057, Rasaff
  ATTACK_ID_EMBER, //0058, Fukano
  ATTACK_ID_EMBER, //0059, Arkani
  ATTACK_ID_WATER, //0060, Quapsel
  ATTACK_ID_WATER, //0061, Quaputzi
  ATTACK_ID_WATER, //0062, Quappo
  ATTACK_ID_CONFUSION, //0063, Abra
  ATTACK_ID_CONFUSION, //0064, Kadabra
  ATTACK_ID_CONFUSION, //0065, Simsala
  ATTACK_ID_KARATE_CHOP, //0066, Machollo
  ATTACK_ID_KARATE_CHOP, //0067, Maschock
  ATTACK_ID_KARATE_CHOP, //0068, Machomei
  ATTACK_ID_SEED, //0069, Knofensa
  ATTACK_ID_SEED, //0070, Ultrigaria
  ATTACK_ID_SEED, //0071, Sarzenia
  ATTACK_ID_WATER, //0072, Tentacha
  ATTACK_ID_WATER, //0073, Tentoxa
  ATTACK_ID_SAND_TOMB, //0074, Kleinstein
  ATTACK_ID_SAND_TOMB, //0075, Georok
  ATTACK_ID_SAND_TOMB, //0076, Geowaz
  ATTACK_ID_EMBER, //0077, Ponita
  ATTACK_ID_EMBER, //0078, Gallopa
  ATTACK_ID_CONFUSION, //0079, Flegmon
  ATTACK_ID_CONFUSION, //0080, Lahmus
  ATTACK_ID_LIGHTNING, //0081, Magnetilo
  ATTACK_ID_LIGHTNING, //0082, Magneton
  ATTACK_ID_AIR_SLASH, //0083, Porenta
  ATTACK_ID_AIR_SLASH, //0084, Dodu
  ATTACK_ID_AIR_SLASH, //0085, Dodri
  ATTACK_ID_WATER, //0086, Jurob
  ATTACK_ID_WATER, //0087, Jugong
  ATTACK_ID_POISONSTING, //0088, Sleima
  ATTACK_ID_POISONSTING, //0089, Sleimok
  ATTACK_ID_WATER, //0090, Muschas
  ATTACK_ID_WATER, //0091, Austos
  ATTACK_ID_POISONSTING, //0092, Nebulak
  ATTACK_ID_POISONSTING, //0093, Alpollo
  ATTACK_ID_POISONSTING, //0094, Gengar
  ATTACK_ID_SAND_TOMB, //0095, Onix
  ATTACK_ID_CONFUSION, //0096, Traumato
  ATTACK_ID_CONFUSION, //0097, Hypno
  ATTACK_ID_WATER, //0098, Krabby
  ATTACK_ID_WATER, //0099, Kingler
  ATTACK_ID_LIGHTNING, //0100, Voltobal
  ATTACK_ID_LIGHTNING, //0101, Lektrobal
  ATTACK_ID_CONFUSION, //0102, Owei
  ATTACK_ID_CONFUSION, //0103, Kokowei
  ATTACK_ID_SAND_TOMB, //0104, Tragosso
  ATTACK_ID_SAND_TOMB, //0105, Knogga
  ATTACK_ID_KARATE_CHOP, //0106, Kicklee
  ATTACK_ID_KARATE_CHOP, //0107, Nockchan
  ATTACK_ID_SCRATCH, //0108, Schlurp
  ATTACK_ID_POISONSTING, //0109, Smogon
  ATTACK_ID_POISONSTING, //0110, Smogmog
  ATTACK_ID_SAND_TOMB, //0111, Rihorn
  ATTACK_ID_SAND_TOMB, //0112, Rizeros
  ATTACK_ID_SCRATCH, //0113, Chaneira
  ATTACK_ID_SEED, //0114, Tangela
  ATTACK_ID_SCRATCH, //0115, Kangama
  ATTACK_ID_WATER, //0116, Seeper
  ATTACK_ID_WATER, //0117, Seemon
  ATTACK_ID_WATER, //0118, Goldini
  ATTACK_ID_WATER, //0119, Golking
  ATTACK_ID_WATER, //0120, Sterndu
  ATTACK_ID_CONFUSION, //0121, Starmie
  ATTACK_ID_CONFUSION, //0122, Pantimos
  ATTACK_ID_STRINGSHOT, //0123, Sichlor
  ATTACK_ID_CONFUSION, //0124, Rossana
  ATTACK_ID_LIGHTNING, //0125, Elektek
  ATTACK_ID_EMBER, //0126, Magmar
  ATTACK_ID_STRINGSHOT, //0127, Pinsir
  ATTACK_ID_SCRATCH, //0128, Tauros
  ATTACK_ID_WATER, //0129, Karpador
  ATTACK_ID_WATER, //0130, Garados
  ATTACK_ID_WATER, //0131, Lapras
  ATTACK_ID_SCRATCH, //0132, Ditto
  ATTACK_ID_SCRATCH, //0133, Evoli
  ATTACK_ID_WATER, //0134, Aquana
  ATTACK_ID_LIGHTNING, //0135, Blitza
  ATTACK_ID_EMBER, //0136, Flamara
  ATTACK_ID_SCRATCH, //0137, Porygon
  ATTACK_ID_WATER, //0138, Amonitas
  ATTACK_ID_WATER, //0139, Amoroso
  ATTACK_ID_WATER, //0140, Kabuto
  ATTACK_ID_WATER, //0141, Kabutops
  ATTACK_ID_ROCK_THROW, //0142, Aerodactyl
  ATTACK_ID_SCRATCH, //0143, Relaxo
  ATTACK_ID_ICE_BEAM, //0144, Arktos
  ATTACK_ID_LIGHTNING, //0145, Zapdos
  ATTACK_ID_EMBER, //0146, Lavados
  ATTACK_ID_DRAGON_RAGE, //0147, Dratini
  ATTACK_ID_DRAGON_RAGE, //0148, Dragonir
  ATTACK_ID_DRAGON_RAGE, //0149, Dragoran
  ATTACK_ID_CONFUSION, //0150, Mewtu
  ATTACK_ID_CONFUSION, //0151, Mew
  ATTACK_ID_SEED, //0152, Endivie
  ATTACK_ID_SEED, //0153, Lorblatt
  ATTACK_ID_SEED, //0154, Meganie
  ATTACK_ID_EMBER, //0155, Feurigel
  ATTACK_ID_EMBER, //0156, Igelavar
  ATTACK_ID_EMBER, //0157, Tornupto
  ATTACK_ID_WATER, //0158, Karnimani
  ATTACK_ID_WATER, //0159, Tyracroc
  ATTACK_ID_WATER, //0160, Impergator
  ATTACK_ID_SCRATCH, //0161, Wiesor
  ATTACK_ID_SCRATCH, //0162, Wiesenior
  ATTACK_ID_AIR_SLASH, //0163, Hoothoot
  ATTACK_ID_AIR_SLASH, //0164, Noctuh
  ATTACK_ID_STRINGSHOT, //0165, Ledyba
  ATTACK_ID_STRINGSHOT, //0166, Ledian
  ATTACK_ID_POISONSTING, //0167, Webarak
  ATTACK_ID_POISONSTING, //0168, Ariados
  ATTACK_ID_POISONSTING, //0169, Iksbat
  ATTACK_ID_WATER, //0170, Lampi
  ATTACK_ID_WATER, //0171, Lanturn
  ATTACK_ID_LIGHTNING, //0172, Pichu
  ATTACK_ID_LEER, //0173, Pii
  ATTACK_ID_LEER, //0174, Fluffeluff
  ATTACK_ID_LEER, //0175, Togepi
  ATTACK_ID_LEER, //0176, Togetic
  ATTACK_ID_CONFUSION, //0177, Natu
  ATTACK_ID_CONFUSION, //0178, Xatu
  ATTACK_ID_LIGHTNING, //0179, Voltilamm
  ATTACK_ID_LIGHTNING, //0180, Waaty
  ATTACK_ID_LIGHTNING, //0181, Ampharos
  ATTACK_ID_SEED, //0182, Blubella
  ATTACK_ID_WATER, //0183, Marill
  ATTACK_ID_WATER, //0184, Azumarill
  ATTACK_ID_ROCK_THROW, //0185, Mogelbaum
  ATTACK_ID_WATER, //0186, Quaxo
  ATTACK_ID_SEED, //0187, Hoppspross
  ATTACK_ID_SEED, //0188, Hubelupf
  ATTACK_ID_SEED, //0189, Papungha
  ATTACK_ID_SCRATCH, //0190, Griffel
  ATTACK_ID_SEED, //0191, Sonnkern
  ATTACK_ID_SEED, //0192, Sonnflora
  ATTACK_ID_STRINGSHOT, //0193, Yanma
  ATTACK_ID_WATER, //0194, Felino
  ATTACK_ID_WATER, //0195, Morlord
  ATTACK_ID_CONFUSION, //0196, Psiana
  ATTACK_ID_BITE, //0197, Nachtara
  ATTACK_ID_AIR_SLASH, //0198, Kramurx
  ATTACK_ID_CONFUSION, //0199, Laschoking
  ATTACK_ID_SHADOW_BALL, //0200, Traunfugil
  ATTACK_ID_CONFUSION, //0201, Icognito
  ATTACK_ID_CONFUSION, //0202, Woingenau
  ATTACK_ID_CONFUSION, //0203, Girafarig
  ATTACK_ID_STRINGSHOT, //0204, Tannza
  ATTACK_ID_STRINGSHOT, //0205, Forstellka
  ATTACK_ID_SCRATCH, //0206, Dummisel
  ATTACK_ID_SAND_TOMB, //0207, Skorgla
  ATTACK_ID_SAND_TOMB, //0208, Stahlos
  ATTACK_ID_LEER, //0209, Snubbull
  ATTACK_ID_LEER, //0210, Granbull
  ATTACK_ID_WATER, //0211, Baldorfish
  ATTACK_ID_STRINGSHOT, //0212, Scherox
  ATTACK_ID_ROCK_THROW, //0213, Pottrott
  ATTACK_ID_KARATE_CHOP, //0214, Skaraborn
  ATTACK_ID_ICE_BEAM, //0215, Sniebel
  ATTACK_ID_SCRATCH, //0216, Teddiursa
  ATTACK_ID_SCRATCH, //0217, Ursaring
  ATTACK_ID_EMBER, //0218, Schneckmag
  ATTACK_ID_EMBER, //0219, Magcargo
  ATTACK_ID_ICE_BEAM, //0220, Quiekel
  ATTACK_ID_ICE_BEAM, //0221, Keifel
  ATTACK_ID_WATER, //0222, Corasonn
  ATTACK_ID_WATER, //0223, Remoraid
  ATTACK_ID_WATER, //0224, Octillery
  ATTACK_ID_ICE_BEAM, //0225, Botogel
  ATTACK_ID_WATER, //0226, Mantax
  ATTACK_ID_METAL_CLAW, //0227, Panzaeron
  ATTACK_ID_EMBER, //0228, Hunduster
  ATTACK_ID_EMBER, //0229, Hundemon
  ATTACK_ID_DRAGON_RAGE, //0230, Seedraking
  ATTACK_ID_SAND_TOMB, //0231, Phanpy
  ATTACK_ID_SAND_TOMB, //0232, Donphan
  ATTACK_ID_SCRATCH, //0233, Porygon2
  ATTACK_ID_SCRATCH, //0234, Damhirplex
  ATTACK_ID_SCRATCH, //0235, Farbeagle
  ATTACK_ID_KARATE_CHOP, //0236, Rabauz
  ATTACK_ID_KARATE_CHOP, //0237, Kapoera
  ATTACK_ID_CONFUSION, //0238, Kussilla
  ATTACK_ID_LIGHTNING, //0239, Elekid
  ATTACK_ID_EMBER, //0240, Magby
  ATTACK_ID_SCRATCH, //0241, Miltank
  ATTACK_ID_SCRATCH, //0242, Heiteira
  ATTACK_ID_LIGHTNING, //0243, Raikou
  ATTACK_ID_EMBER, //0244, Entei
  ATTACK_ID_WATER, //0245, Suicune
  ATTACK_ID_SAND_TOMB, //0246, Larvitar
  ATTACK_ID_SAND_TOMB, //0247, Pupitar
  ATTACK_ID_ROCK_THROW, //0248, Despotar
  ATTACK_ID_CONFUSION, //0249, Lugia
  ATTACK_ID_EMBER, //0250, Ho-Oh
  ATTACK_ID_CONFUSION, //0251, Celebi
  ATTACK_ID_SEED, //0252, Geckarbor
  ATTACK_ID_SEED, //0253, Reptain
  ATTACK_ID_SEED, //0254, Gewaldro
  ATTACK_ID_EMBER, //0255, Flemmli
  ATTACK_ID_EMBER, //0256, Jungglut
  ATTACK_ID_EMBER, //0257, Lohgock
  ATTACK_ID_WATER, //0258, Hydropi
  ATTACK_ID_WATER, //0259, Moorabbel
  ATTACK_ID_WATER, //0260, Sumpex
  ATTACK_ID_BITE, //0261, Fiffyen
  ATTACK_ID_BITE, //0262, Magnayen
  ATTACK_ID_SCRATCH, //0263, Zigzachs
  ATTACK_ID_SCRATCH, //0264, Geradaks
  ATTACK_ID_STRINGSHOT, //0265, Waumpel
  ATTACK_ID_STRINGSHOT, //0266, Schaloko
  ATTACK_ID_STRINGSHOT, //0267, Papinella
  ATTACK_ID_STRINGSHOT, //0268, Panekon
  ATTACK_ID_POISONSTING, //0269, Pudox
  ATTACK_ID_WATER, //0270, Loturzel
  ATTACK_ID_WATER, //0271, Lombrero
  ATTACK_ID_WATER, //0272, Kappalores
  ATTACK_ID_SEED, //0273, Samurzel
  ATTACK_ID_SEED, //0274, Blanas
  ATTACK_ID_SEED, //0275, Tengulist
  ATTACK_ID_AIR_SLASH, //0276, Schwalbini
  ATTACK_ID_AIR_SLASH, //0277, Schwalboss
  ATTACK_ID_WATER, //0278, Wingull
  ATTACK_ID_WATER, //0279, Pelipper
  ATTACK_ID_CONFUSION, //0280, Trasla
  ATTACK_ID_CONFUSION, //0281, Kirlia
  ATTACK_ID_CONFUSION, //0282, Guardevoir
  ATTACK_ID_WATER, //0283, Gehweiher
  ATTACK_ID_STRINGSHOT, //0284, Maskeregen
  ATTACK_ID_SEED, //0285, Knilz
  ATTACK_ID_SEED, //0286, Kapilz
  ATTACK_ID_SCRATCH, //0287, Bummelz
  ATTACK_ID_SCRATCH, //0288, Muntier
  ATTACK_ID_SCRATCH, //0289, Letarking
  ATTACK_ID_SAND_TOMB, //0290, Nincada
  ATTACK_ID_STRINGSHOT, //0291, Ninjask
  ATTACK_ID_STRINGSHOT, //0292, Ninjatom
  ATTACK_ID_SCRATCH, //0293, Flurmel
  ATTACK_ID_SCRATCH, //0294, Krakeelo
  ATTACK_ID_SCRATCH, //0295, Krawumms
  ATTACK_ID_KARATE_CHOP, //0296, Makuhita
  ATTACK_ID_KARATE_CHOP, //0297, Hariyama
  ATTACK_ID_LEER, //0298, Azurill
  ATTACK_ID_ROCK_THROW, //0299, Nasgnet
  ATTACK_ID_SCRATCH, //0300, Eneco
  ATTACK_ID_SCRATCH, //0301, Enekoro
  ATTACK_ID_SHADOW_BALL, //0302, Zobiris
  ATTACK_ID_LEER, //0303, Flunkifer
  ATTACK_ID_ROCK_THROW, //0304, Stollunior
  ATTACK_ID_ROCK_THROW, //0305, Stollrak
  ATTACK_ID_ROCK_THROW, //0306, Stolloss
  ATTACK_ID_CONFUSION, //0307, Meditie
  ATTACK_ID_CONFUSION, //0308, Meditalis
  ATTACK_ID_LIGHTNING, //0309, Frizelbliz
  ATTACK_ID_LIGHTNING, //0310, Voltenso
  ATTACK_ID_LIGHTNING, //0311, Plusle
  ATTACK_ID_LIGHTNING, //0312, Minun
  ATTACK_ID_STRINGSHOT, //0313, Volbeat
  ATTACK_ID_STRINGSHOT, //0314, Illumise
  ATTACK_ID_SEED, //0315, Roselia
  ATTACK_ID_POISONSTING, //0316, Schluppuck
  ATTACK_ID_POISONSTING, //0317, Schlukwech
  ATTACK_ID_WATER, //0318, Kanivanha
  ATTACK_ID_WATER, //0319, Tohaido
  ATTACK_ID_WATER, //0320, Wailmer
  ATTACK_ID_WATER, //0321, Wailord
  ATTACK_ID_EMBER, //0322, Camaub
  ATTACK_ID_EMBER, //0323, Camerupt
  ATTACK_ID_EMBER, //0324, Qurtel
  ATTACK_ID_CONFUSION, //0325, Spoink
  ATTACK_ID_CONFUSION, //0326, Groink
  ATTACK_ID_SCRATCH, //0327, Pandir
  ATTACK_ID_SAND_TOMB, //0328, Knacklion
  ATTACK_ID_DRAGON_RAGE, //0329, Vibrava
  ATTACK_ID_DRAGON_RAGE, //0330, Libelldra
  ATTACK_ID_SEED, //0331, Tuska
  ATTACK_ID_SEED, //0332, Noktuska
  ATTACK_ID_AIR_SLASH, //0333, Wablu
  ATTACK_ID_DRAGON_RAGE, //0334, Altaria
  ATTACK_ID_SCRATCH, //0335, Sengo
  ATTACK_ID_POISONSTING, //0336, Vipitis
  ATTACK_ID_CONFUSION, //0337, Lunastein
  ATTACK_ID_CONFUSION, //0338, Sonnfel
  ATTACK_ID_WATER, //0339, Schmerbe
  ATTACK_ID_WATER, //0340, Welsar
  ATTACK_ID_WATER, //0341, Krebscorps
  ATTACK_ID_WATER, //0342, Krebutack
  ATTACK_ID_CONFUSION, //0343, Puppance
  ATTACK_ID_CONFUSION, //0344, Lepumentas
  ATTACK_ID_SEED, //0345, Liliep
  ATTACK_ID_SEED, //0346, Wielie
  ATTACK_ID_ROCK_THROW, //0347, Anorith
  ATTACK_ID_ROCK_THROW, //0348, Armaldo
  ATTACK_ID_WATER, //0349, Barschwa
  ATTACK_ID_WATER, //0350, Milotic
  ATTACK_ID_SCRATCH, //0351, Formeo
  ATTACK_ID_SCRATCH, //0352, Kecleon
  ATTACK_ID_SHADOW_BALL, //0353, Shuppet
  ATTACK_ID_SHADOW_BALL, //0354, Banette
  ATTACK_ID_SHADOW_BALL, //0355, Zwirrlicht
  ATTACK_ID_SHADOW_BALL, //0356, Zwirrklop
  ATTACK_ID_SEED, //0357, Tropius
  ATTACK_ID_CONFUSION, //0358, Palimpalim
  ATTACK_ID_BITE, //0359, Absol
  ATTACK_ID_CONFUSION, //0360, Isso
  ATTACK_ID_ICE_BEAM, //0361, Schneppke
  ATTACK_ID_ICE_BEAM, //0362, Firnontor
  ATTACK_ID_WATER, //0363, Seemops
  ATTACK_ID_WATER, //0364, Seejong
  ATTACK_ID_WATER, //0365, Walraisa
  ATTACK_ID_WATER, //0366, Perlu
  ATTACK_ID_WATER, //0367, Aalabyss
  ATTACK_ID_WATER, //0368, Saganabyss
  ATTACK_ID_WATER, //0369, Relicanth
  ATTACK_ID_WATER, //0370, Liebiskus
  ATTACK_ID_DRAGON_RAGE, //0371, Kindwurm
  ATTACK_ID_DRAGON_RAGE, //0372, Draschel
  ATTACK_ID_DRAGON_RAGE, //0373, Brutalanda
  ATTACK_ID_CONFUSION, //0374, Tanhel
  ATTACK_ID_CONFUSION, //0375, Metang
  ATTACK_ID_CONFUSION, //0376, Metagross
  ATTACK_ID_ROCK_THROW, //0377, Regirock
  ATTACK_ID_ICE_BEAM, //0378, Regice
  ATTACK_ID_METAL_CLAW, //0379, Registeel
  ATTACK_ID_DRAGON_RAGE, //0380, Latias
  ATTACK_ID_DRAGON_RAGE, //0381, Latios
  ATTACK_ID_WATER, //0382, Kyogre
  ATTACK_ID_SAND_TOMB, //0383, Groudon
  ATTACK_ID_DRAGON_RAGE, //0384, Rayquaza
  ATTACK_ID_CONFUSION, //0385, Jirachi
  ATTACK_ID_CONFUSION, //0386, Deoxys
};