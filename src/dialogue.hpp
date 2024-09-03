#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <raylib.h>
#include <nlohmann/json.hpp>

class DialogueManager {
public:
    DialogueManager();
    virtual ~DialogueManager();

    void DrawBg();
    void DrawCharacter();
    void DrawDialogue();
    void ManageDialogue();

    int curDiaIdx = 0;
    std::string curSeqStr = "Intro";

private:
    std::array<Image, 8> bgImgs;
    std::array<Texture2D, 8> bgTexs;

    std::array<Image, 3> charImgs;
    std::array<Texture2D, 3> charTexs;

    int charIdx;

    Rectangle expressionRec;
    Vector2 charPos;

    Font defaultFont;
    Font italicsFont;
    Font boldFont;

    float framesCounter = 0;

    int boxX;

    nlohmann::json dia;

    std::string curDia;
    std::string curPlace;

    int letterCount = 0;
    char input[9] = "\0";
    std::string name;

    std::string TextWrapped(std::string text, Font& font, float& width, const int& textOffsetX);
    int IndexOfStringInVector(const std::vector<std::string>& stringArray, std::string stringToFind);

    // Constants
    const char fontSize = 36;
    const char spacing = 0;

    const int borderSize = 10;
    const int optionBorderSize = 200;
    const int maxDiaBoxWidth = 800;
    const int diaBoxHeight = 175;
    const int nameBoxWidth = 175;
    const int nameBoxHeight = 40;
    const int optionBtnWidth = 300;
    const int optionBtnHeight = 100;
    const int optionBtnsWidth = 700;
    const Color boxCol = { 0, 0, 0, 200 };
    const std::vector<std::string> places = { 
        "Lolake", 
        "Hall", 
        "Hall2",
        "LongHall",
        "Dorm", 
        "Classroom", 
        "Chemistry",
        "LoveClub"
    };
    const std::vector<std::string> chars = { 
        "Carmilla", 
        "Ivory", 
        "Kaya" 
    };
    const std::vector<std::string> exprs = {
        "EyesClosed",
        "Sad",
        "Crying",
        "Confused",
        "Disappointed",
        "Neutral",
        "Happy",
        "Surprised",
        "Angry"
    };
};

#endif