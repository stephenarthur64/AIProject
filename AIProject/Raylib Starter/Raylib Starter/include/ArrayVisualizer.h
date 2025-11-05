#pragma once
#include "raylib.h"
#include <vector>
#include <string>

struct AnimatedElement {
    int value;
    float x, y;
    float targetY;
    float highlightTimer; // for visual feedback (seconds)

    AnimatedElement(int v, float xpos, float ypos, float tY)
        : value(v), x(xpos), y(ypos), targetY(tY), highlightTimer(0.0f) {
    }
};


class ArrayVisualizer {
private:
    int screenWidth, screenHeight;
    std::vector<AnimatedElement> elements;

    std::string inputValue;   // for entering element values
    std::string sizeInput;    // for entering array size
    bool activeValueInput = false;
    bool activeSizeInput = true;

    Rectangle inputBox;
    Rectangle addButton;
    Rectangle sizeBox;
    Rectangle setSizeButton;
    Rectangle clearButton;

    int maxSize = 0;

    std::string indexInput;   // for entering the target index
    bool activeIndexInput = false;

    Rectangle indexBox;
    Rectangle setAtIndexButton;

public:
    ArrayVisualizer(int w, int h);

    void HandleInput();
    void UpdateAnimations();
    void DrawUI();
    void Draw();
};
