#include "BinaryTreeVisualizer.h"
#include <cctype>

BinaryTreeVisualizer::BinaryTreeVisualizer(int width, int height)
    : screenWidth(width), screenHeight(height), tree(width, height), activeInputBox(false) {
}

void BinaryTreeVisualizer::Update() {
    float uiScale = screenWidth / 1600.0f;

    Rectangle inputBox = { 30 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };
    Rectangle insertBtn = { 200 * uiScale, 100 * uiScale, 140 * uiScale, 35 * uiScale };

    if (CheckCollisionPointRec(GetMousePosition(), inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        activeInputBox = true;
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        activeInputBox = false;

    if (activeInputBox) {
        int key = GetCharPressed();
        while (key > 0) {
            if (isdigit(key)) inputValue += (char)key;
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && !inputValue.empty()) inputValue.pop_back();
    }

    tree.HandleInput();
    tree.UpdateAnimations();
}

void BinaryTreeVisualizer::Draw() {
    float uiScale = screenWidth / 1600.0f;

    tree.Draw();
}
