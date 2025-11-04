#pragma once
#include "raylib.h"
#include "LinkedList.h"

class LinkedListVisualizer {
public:
    LinkedListVisualizer();
    void Update();
    void Draw();

private:
    LinkedList list;
    std::string inputValue, inputIndex;
    bool activeValueBox, activeIndexBox;

    bool Button(Rectangle rect, const char* label, float uiScale);
    void DrawInputBox(Rectangle rect, std::string& input, bool active, float uiScale);
};
