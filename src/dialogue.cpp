#include "dialogue.hpp"
#include <fstream>

DialogueManager::DialogueManager() {
    // Places
    for (int i = 0; i < places.size(); ++i) { 
        std::string placeFile = "resources/places/" + places[i] + ".png";
        bgImgs[i] = LoadImage(placeFile.c_str());
        
        bgTexs[i] = LoadTextureFromImage(bgImgs[i]);
        UnloadImage(bgImgs[i]);
    }

    // Characters
    for (int i = 0; i < chars.size(); ++i) { 
        std::string charFile = "resources/characters/" + chars[i] + ".png";
        charImgs[i] = LoadImage(charFile.c_str());

        ImageResize(&charImgs[i], charImgs[i].width / 2, charImgs[i].height / 2);

        charTexs[i] = LoadTextureFromImage(charImgs[i]);
        UnloadImage(charImgs[i]);
    }

    // Expression frame
    expressionRec = { 0.0f, 0.0f, charTexs[0].width / (float)exprs.size(), (float)charTexs[0].height };

    // Fonts
    defaultFont = LoadFontEx("resources/fonts/LinLibertine_R.ttf", 64, 0, 250);
    italicsFont = LoadFontEx("resources/fonts/LinLibertine_RI.ttf", 64, 0, 250);
    boldFont = LoadFontEx("resources/fonts/LinLibertine_RB.ttf", 64, 0, 250);
    SetTextLineSpacing(32);

    // Parse dialogue JSON
    std::ifstream diaFile("src/dialogue.json");
    dia = nlohmann::json::parse(diaFile);
}

DialogueManager::~DialogueManager() {
    for (int i = 0; i < places.size(); ++i) UnloadTexture(bgTexs[i]);
    for (int i = 0; i < chars.size(); ++i) UnloadTexture(charTexs[i]);
    UnloadFont(defaultFont);
    UnloadFont(italicsFont);
    UnloadFont(boldFont);
} 

void DialogueManager::DrawBg() {
    // Update current BG
    if (dia[curSeqStr][curDiaIdx].contains("Background")) {
        curPlace = dia[curSeqStr][curDiaIdx]["Background"];
    }

    // Draw
    int bgIdx = IndexOfStringInVector(places, curPlace);
    Color tint = dia[curSeqStr][curDiaIdx].contains("Character") ? LIGHTGRAY : WHITE;
    DrawTexture(bgTexs[bgIdx], GetScreenWidth() / 2.0f - bgTexs[bgIdx].width / 2.0f, GetScreenHeight() / 2.0f - bgTexs[bgIdx].height / 2.0f, tint);
}

void DialogueManager::DrawCharacter() {
    if (!dia[curSeqStr][curDiaIdx].contains("Character")) return;

    // Update current character and expression
    std::string curExpr = dia[curSeqStr][curDiaIdx]["Expression"];

    charPos = {GetScreenWidth() / 2.0f - expressionRec.width / 2.0f, GetScreenHeight() / 2.0f - expressionRec.height / 2.0f};

    // Expression check
    int expIdx = IndexOfStringInVector(exprs, curExpr);

    expressionRec.x = expIdx * charTexs[0].width / (float)exprs.size();

    // Draw correct character
    charIdx = IndexOfStringInVector(chars, dia[curSeqStr][curDiaIdx]["Character"]);
    DrawTextureRec(charTexs[charIdx], expressionRec, charPos, WHITE);
}

void DialogueManager::DrawDialogue() {
    // ---- DIALOGUE BOX ---- //
    bool isScreenSmall = GetScreenWidth() < maxDiaBoxWidth + (boxX * 2);

    // Dynamic dimensions
    int diaBoxWidth = isScreenSmall ? GetScreenWidth() - (boxX * 2) : maxDiaBoxWidth;

    // Dynamic positions
    boxX = isScreenSmall ? borderSize : (GetScreenWidth() - diaBoxWidth) / 2.0f;
    int diaBoxY = GetScreenHeight() - (diaBoxHeight + borderSize);

    int textOffsetX = 5;
    Vector2 diaTextPos = {(float)boxX + textOffsetX, (float)diaBoxY};

    Rectangle diaRec = {(float)boxX, (float)diaBoxY, (float)diaBoxWidth, (float)diaBoxHeight};

    // Get font
    Font curFont = dia[curSeqStr][curDiaIdx]["Font"] == "Italics" ? italicsFont : defaultFont;

    // Update current dialogue
    curDia = dia[curSeqStr][curDiaIdx]["Dialogue"];

    // ---- INPUT ---- //
    if (curDia.find("[Input]") != std::string::npos) {
        if (IsKeyPressed(KEY_ENTER) && input[0] != '\0') {
            name = input;

            framesCounter = 0; 
            curDiaIdx++;
            return;
        }

        int key = GetCharPressed();
        
        while (key > 0) {
            if (((key >= 65 && key <= 90) || (key >= 97 && key <= 122)) && letterCount <= 9) {
                input[letterCount] = (char)key;
                input[letterCount + 1] = '\0';
                letterCount++;
            }

            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE)) {
            letterCount--;
            if (letterCount < 0) letterCount = 0;
            input[letterCount] = '\0';
        }

        curDia.replace(curDia.find("[Input]"), sizeof("[Input]") - 1, input);
    }

    if (curDia.find("[Name]") != std::string::npos) {
        curDia.replace(curDia.find("[Name]"), sizeof("[Name]") - 1, name);
    }

    std::string diaWrapped = TextWrapped(curDia, curFont, diaRec.width, textOffsetX);
    
    // Draw
    DrawRectangle(diaRec.x, diaRec.y, diaRec.width, diaRec.height, boxCol);
    DrawTextEx(curFont, TextSubtext(diaWrapped.c_str(), 0, framesCounter), diaTextPos, fontSize, spacing, WHITE);
    
    // ---- CHARACTER BOX ---- //
    if (dia[curSeqStr][curDiaIdx].contains("Character") && dia[curSeqStr][curDiaIdx]["Font"] != "Italics") {
        // Dynamic positions
        int nameBoxY = GetScreenHeight() - (diaBoxHeight + nameBoxHeight + (borderSize * 2));
        Vector2 charTextPos = {(float)boxX + textOffsetX, (float)nameBoxY};

        // Text color
        Color charTextColor;

        switch (charIdx) {
            case 0: // Carmilla
                charTextColor = PURPLE;
                break;
            case 1: // Ivory
                charTextColor = ORANGE;
                break;
            case 2: // Kaya
                charTextColor = BROWN;
                break;
        }
        
        // Get person talking and correct text color
        bool isPlayerTalking = dia[curSeqStr][curDiaIdx]["Talking"] == "Player";

        std::string personTalking = isPlayerTalking ? "You" : dia[curSeqStr][curDiaIdx]["Character"];

        Color textColor = isPlayerTalking ? WHITE : charTextColor;

        // Draw
        DrawRectangle(boxX, nameBoxY, nameBoxWidth, nameBoxHeight, boxCol);
        DrawTextEx(boldFont, personTalking.c_str(), charTextPos, fontSize, spacing, textColor);
    }

    // ---- OPTION BOXES ---- //
    if (dia[curSeqStr][curDiaIdx].contains("Options")) {
        for (int i = 0; i < dia[curSeqStr][curDiaIdx]["Options"].size(); ++i) {
            // Divide the screen in amount of options
            int dividedScreen = optionBtnsWidth / dia[curSeqStr][curDiaIdx]["Options"].size();
            int startX = (GetScreenWidth() - optionBtnsWidth) / 2;

            // Get the center of the corresponding divided screen
            int optionBtnX = startX + (dividedScreen / 2) + (i * dividedScreen) - (optionBtnWidth / 2);
            int optionBtnY = GetScreenHeight() - (diaBoxHeight + nameBoxHeight + optionBtnHeight + optionBorderSize);

            Rectangle btn = {(float)optionBtnX, (float)optionBtnY, (float)optionBtnWidth, (float)optionBtnHeight};
            
            // Get option text and wrap it
            std::string nameOfOption = dia[curSeqStr][curDiaIdx]["Options"][i]["Name"];
            std::string optionWrapped = TextWrapped(nameOfOption, defaultFont, btn.width, textOffsetX);

            // Center option text
            Vector2 textSize = MeasureTextEx(defaultFont, optionWrapped.c_str(), fontSize, spacing);
            Vector2 textPos = {btn.x + btn.width / 2.0f - textSize.x / 2.0f, btn.y + btn.height / 2.0f - textSize.y / 2.0f};

            // If option button pressed, change current sequence accordingly
            if (CheckCollisionPointRec(GetMousePosition(), btn) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                framesCounter = 0;
                curDiaIdx = 0;

                curSeqStr = nameOfOption;
            }

            // Draw
            DrawRectangle(btn.x, btn.y, btn.width, btn.height, boxCol);
            DrawTextEx(defaultFont, optionWrapped.c_str(), textPos, fontSize, spacing, WHITE);
        }
    }
}

void DialogueManager::ManageDialogue() {
    // Small pause if character is a symbol
    if (curDia[framesCounter - 1] == '.' || curDia[framesCounter - 1] == '!' || curDia[framesCounter - 1] == '?') {
        framesCounter += IsKeyDown(KEY_SPACE) ? 0.5f : 0.05f;
    }
    else if (curDia[framesCounter - 1] == ',') {
        framesCounter += IsKeyDown(KEY_SPACE) ? 1 : 0.15f;
    }
    else {
        framesCounter += IsKeyDown(KEY_SPACE) ? 4 : 1;
    }

    // Next dialogue
    bool buttonPressed = IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    if ((buttonPressed || dia[curSeqStr][curDiaIdx]["Skip"] == "Yes") && framesCounter > curDia.size() && curDiaIdx < dia[curSeqStr].size() - 1 && dia[curSeqStr][curDiaIdx]["Options"] != NULL && curDia.find("Enter your name:") == std::string::npos)  {
        framesCounter = 0; 
        curDiaIdx++;
    }
}

std::string DialogueManager::TextWrapped(std::string text, Font& font, float& width, const int& textOffsetX) {
    std::string textWrapped;
    float textWidth = 0;
    float textWidthAtLastSpace = 0;
    int lastSpace = -1;

    for (int i = 0; i < text.size(); ++i) {
        textWrapped += text[i];

        std::string thisChar{text[i]};
        float charWidth = MeasureTextEx(font, thisChar.c_str(), fontSize, spacing).x;
        textWidth += charWidth;

        if (text[i] == ' ')  {
            lastSpace = i;
            textWidthAtLastSpace = textWidth;
        }

        if (textWidth >= width - (textOffsetX * 2)) {
            if (lastSpace != -1) {
                textWrapped[lastSpace] = '\n';
                textWidth -= textWidthAtLastSpace;
                lastSpace = -1;
            }
            else {
                textWrapped += '\n';
                textWidth = 0;
            }
        }
    }

    return textWrapped;
}

int DialogueManager::IndexOfStringInVector(const std::vector<std::string>& stringArray, std::string stringToFind) {
    return std::distance(stringArray.begin(), std::find(stringArray.begin(), stringArray.end(), stringToFind));
}