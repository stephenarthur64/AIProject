#include "LinkedListVisualizer.h"
#include <cctype>

LinkedListVisualizer::LinkedListVisualizer()
    : activeValueBox(false), activeIndexBox(false) {
}

bool LinkedListVisualizer::Button(Rectangle rect, const char* label, float uiScale) {
    DrawRectangleRec(rect, LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2 * uiScale, DARKGRAY);
    int textWidth = MeasureText(label, (int)(20 * uiScale));
    DrawText(label, rect.x + (rect.width - textWidth) / 2, rect.y + 5 * uiScale, (int)(20 * uiScale), BLACK);
    return CheckCollisionPointRec(GetMousePosition(), rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void LinkedListVisualizer::DrawInputBox(Rectangle rect, std::string& input, bool active, float uiScale) {
    DrawRectangleRec(rect, active ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(rect, 2 * uiScale, GRAY);
    DrawText(input.c_str(), rect.x + 5 * uiScale, rect.y + 5 * uiScale, (int)(20 * uiScale), BLACK);
}

void LinkedListVisualizer::Update() {
    float uiScale = screenWidth / 1600.0f;

    Rectangle valueBox = { 30 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle indexBox = { 200 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle insertBeforeBtn = { 370 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle insertAfterBtn = { 530 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle addEndBtn = { 690 * uiScale, 100 * uiScale, 100 * uiScale, 35 * uiScale };
    Rectangle deleteLastBtn = { 810 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };

    if (CheckCollisionPointRec(GetMousePosition(), valueBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        activeValueBox = true; activeIndexBox = false;
    }
    else if (CheckCollisionPointRec(GetMousePosition(), indexBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        activeIndexBox = true; activeValueBox = false;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        activeValueBox = activeIndexBox = false;
    }

    if (activeValueBox || activeIndexBox) {
        int key = GetCharPressed();
        while (key > 0) {
            if (isdigit(key)) {
                if (activeValueBox) inputValue += (char)key;
                else if (activeIndexBox) inputIndex += (char)key;
            }
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (activeValueBox && !inputValue.empty()) inputValue.pop_back();
            else if (activeIndexBox && !inputIndex.empty()) inputIndex.pop_back();
        }
    }

    if (Button(insertBeforeBtn, "Insert Before", uiScale))
        if (!inputValue.empty() && !inputIndex.empty())
            list.InsertNodeAt(std::stoi(inputIndex), std::stoi(inputValue));

    if (Button(insertAfterBtn, "Insert After", uiScale))
        if (!inputValue.empty() && !inputIndex.empty())
            list.InsertNodeAt(std::stoi(inputIndex) + 1, std::stoi(inputValue));

    if (Button(addEndBtn, "Add End", uiScale))
        if (!inputValue.empty()) list.AddNode(std::stoi(inputValue));

    if (Button(deleteLastBtn, "Delete Last", uiScale))
        list.DeleteLastNode();

    list.UpdateAnimations();
}

void LinkedListVisualizer::Draw() {
    float uiScale = screenWidth / 1600.0f;
    Rectangle valueBox = { 30 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle indexBox = { 200 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };

    DrawText("Enter Value:", valueBox.x, valueBox.y - 20 * uiScale, (int)(18 * uiScale), DARKGRAY);
    DrawInputBox(valueBox, inputValue, activeValueBox, uiScale);

    DrawText("Enter Index:", indexBox.x, indexBox.y - 20 * uiScale, (int)(18 * uiScale), DARKGRAY);
    DrawInputBox(indexBox, inputIndex, activeIndexBox, uiScale);

    list.Draw();
}
