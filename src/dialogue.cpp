#include "dialogue.hpp"
#include <fstream>
#include <raylib.h>
#include <iostream>

DialogueManager::DialogueManager() {
    // Background
    classImg = LoadImage("resources/places/Classroom.png");

    classTex = LoadTextureFromImage(classImg);
    UnloadImage(classImg);

    // Characters
    carmillaImg = LoadImage("resources/characters/Carmilla.png");
    ivoryImg = LoadImage("resources/characters/Ivory.png");
    kayaImg = LoadImage("resources/characters/Kaya.png");
    
    ImageResize(&carmillaImg, carmillaImg.width / 2, carmillaImg.height / 2);
    ImageResize(&ivoryImg, ivoryImg.width / 2, ivoryImg.height / 2);
    ImageResize(&kayaImg, kayaImg.width / 2, kayaImg.height / 2);

    carmillaTex = LoadTextureFromImage(carmillaImg);
    ivoryTex = LoadTextureFromImage(ivoryImg);
    kayaTex = LoadTextureFromImage(kayaImg);

    UnloadImage(carmillaImg);
    UnloadImage(ivoryImg);
    UnloadImage(kayaImg);

    // Expression frame
    expressionRec = { 0.0f, 0.0f, carmillaTex.width / 10.0f, static_cast<float>(carmillaTex.height) };

    // Font
    font = LoadFontEx("resources/fonts/BelgianoSerif.ttf", 64, 0, 250);
    SetTextLineSpacing(32);

    // Parse JSON
    std::ifstream diaFile("src/dialogue.json");
    dia = nlohmann::json::parse(diaFile);
}

DialogueManager::~DialogueManager() {
    UnloadTexture(carmillaTex);
    UnloadTexture(ivoryTex);
    UnloadTexture(kayaTex);
    UnloadTexture(classTex);
    UnloadFont(font);
}

void DialogueManager::DrawBg() {
    // Update current BG
    if (dia["1"][curDiaIdx].contains("Background")) {
        curBg = dia["1"][curDiaIdx]["Background"].get<std::string>();
    }

    if (curBg.compare("Classroom") == 0) {
        DrawTexture(classTex, GetScreenWidth() / 2 - classTex.width / 2, GetScreenHeight() / 2 - classTex.height / 2, WHITE);
    }
}

void DialogueManager::DrawCharacter() {
    if (!dia["1"][curDiaIdx].contains("Character")) return;

    // Update current character and expression
    curChar = dia["1"][curDiaIdx]["Character"].get<std::string>();
    curExpr = dia["1"][curDiaIdx]["Expression"].get<std::string>();

    charPos = {GetScreenWidth() / 2.0f - expressionRec.width / 2.0f, GetScreenHeight() / 2.0f - expressionRec.height / 2.0f};

    // Expression check
    auto expIt = expressionMap.find(curExpr);
    recMult = expIt->second;

    expressionRec.x = recMult * carmillaTex .width / static_cast<float>(expressionMap.size());

    // Draw correct character
    auto charIt = charMap.find(curChar);

    switch (charIt->second) {
        case 0:
            DrawTextureRec(carmillaTex, expressionRec, charPos, WHITE);
            break;
        case 1:
            DrawTextureRec(ivoryTex, expressionRec, charPos, WHITE);
            break;
        case 2:
            DrawTextureRec(kayaTex, expressionRec, charPos, WHITE);
            break;
    }
}

void DialogueManager::DrawDialogue() {
    // Update current dialogue
    curDia = dia["1"][curDiaIdx]["Dialogue"].get<std::string>();

    // Dynamic Dimensions
    bool isScreenSmall = GetScreenWidth() < 1000 + (boxX * 2);

    diaBoxWidth = isScreenSmall ? GetScreenWidth() - (boxX * 2) : 1000;
    boxX = isScreenSmall ? borderSize : (GetScreenWidth() - diaBoxWidth) / 2.0f;

    // Dynamic Positions
    diaBoxY = GetScreenHeight() - (diaBoxHeight + borderSize);
    nameBoxY = GetScreenHeight() - (diaBoxHeight + nameBoxHeight + (borderSize * 2));

    Vector2 diaTextPos = {static_cast<float>(boxX), static_cast<float>(diaBoxY)};
    Vector2 charTextPos = {static_cast<float>(boxX), static_cast<float>(nameBoxY)};
    
    // Draw
    DrawRectangle(boxX, diaBoxY, diaBoxWidth, diaBoxHeight, boxCol);
    DrawTextEx(font, TextSubtext(curDia.c_str(), 0, framesCounter/2), diaTextPos, fontSize, spacing, WHITE);

    if (dia["1"][curDiaIdx].contains("Character")) {
        DrawRectangle(boxX, nameBoxY, nameBoxWidth, nameBoxHeight, boxCol);
        DrawTextEx(font, curChar.c_str(), charTextPos, fontSize, spacing, WHITE);
    }

    // Options
    if (dia["1"][curDiaIdx].contains("Options")) {
        for (int i = 0; i < dia["1"][curDiaIdx]["Options"].size(); ++i) {
            // Divide the screen in amount of options
            uint16_t dividedScreen = optionBtnsWidth / dia["1"][curDiaIdx]["Options"].size();
            uint16_t startX = (GetScreenWidth() - optionBtnsWidth) / 2;

            // Get the center of the corresponding divided screen
            optionBtnX = startX + (dividedScreen / 2) + (i * dividedScreen) - (optionBtnWidth / 2);
            optionBtnY = GetScreenHeight() - (diaBoxHeight + nameBoxHeight + optionBtnHeight + optionBorderSize);
            
            // Option name
            std::string nameOfOption = dia["1"][curDiaIdx]["Options"][i]["Name"].get<std::string>().c_str();

            // Center option text
            Vector2 textSize = MeasureTextEx(font, nameOfOption.c_str(), optionFontSize, spacing);
            Vector2 textPos = {optionBtnX + optionBtnWidth / 2.0f - textSize.x / 2.0f, optionBtnY + optionBtnHeight / 2.0f - textSize.y / 2.0f};

            // Button
            Rectangle btn = {static_cast<float>(optionBtnX), static_cast<float>(optionBtnY), static_cast<float>(optionBtnWidth), static_cast<float>(optionBtnHeight)};

            if (CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                std::cout << "Button: " << i << " pressed!\n";
            }

            // Draw
            DrawRectangle(btn.x, btn.y, btn.width, btn.height, boxCol);
            DrawTextEx(font, nameOfOption.c_str(), textPos, optionFontSize, spacing, WHITE);
        }
    }
}

void DialogueManager::ManageDialogue() {
    framesCounter += IsKeyDown(KEY_SPACE) ? 8 : 1;

    if (IsKeyPressed(KEY_ENTER) && framesCounter > curDia.length() && curDiaIdx < dia["1"].size() - 1 && !dia["1"][curDiaIdx].contains("Options"))  {
        framesCounter = 0; 
        curDiaIdx++;
    }
}