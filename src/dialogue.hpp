#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <cstdint>
#include <raylib.h>
#include <nlohmann/json.hpp>

class DialogueManager {
private:
    Image classImg;
    Texture2D classTex;

    Image carmillaImg;
    Texture2D carmillaTex;

    Image ivoryImg;
    Texture2D ivoryTex;

    Image kayaImg;
    Texture2D kayaTex;

    Rectangle expressionRec;
    Vector2 charPos;

    Font font;

    uint8_t recMult;

    uint16_t framesCounter = 0;

    uint16_t diaBoxWidth;
    uint16_t boxX;
    uint16_t diaBoxY;
    uint16_t nameBoxY;
    uint16_t optionBtnX;
    uint16_t optionBtnY;

    std::string curBg;
    std::string curChar;
    std::string curExpr;
    std::string curDia;

    nlohmann::json dia;

    // Constants
    const uint8_t fontSize = 32;
    const uint8_t optionFontSize = 64;
    const uint8_t spacing = 1;
    const uint8_t borderSize = 10;
    const uint8_t optionBorderSize = 100;
    const uint8_t diaBoxHeight = 200;
    const uint8_t nameBoxWidth = 200;
    const uint8_t nameBoxHeight = 30;
    const uint8_t optionBtnWidth = 200;
    const uint8_t optionBtnHeight = 100;
    const uint16_t optionBtnsWidth = 700;
    const Color boxCol = { 0, 0, 0, 200 };
    const std::map<std::string, char> charMap = {
        { "Carmilla", 0 },
        { "Ivory", 1 },
        { "Kaya", 2 }
    };
    const std::map<std::string, char> expressionMap = {
        { "Sighing", 0 },
        { "Sad", 1 },
        { "Crying", 2 },
        { "Annoyed", 3 },
        { "Sad2", 4 },
        { "Neutral", 5 },
        { "Happy", 6 },
        { "Surprised", 7 },
        { "Angry", 8 },
        { "Furious", 9 },
    };

public:
    DialogueManager();
    virtual ~DialogueManager();

    void DrawBg();
    void DrawCharacter();
    void DrawDialogue();
    void ManageDialogue();

    uint16_t curDiaIdx = 0;
    uint16_t curSeqIdx = 0;
};

#endif