#include "ArrayVisualizer.h"
#include <cstdlib>

ArrayVisualizer::ArrayVisualizer(int w, int h) : screenWidth(w), screenHeight(h) {
    inputBox = { 50, 120, 200, 40 };
    addButton = { 270, 120, 120, 40 };
    sizeBox = { 50, 50, 200, 40 };
    setSizeButton = { 270, 50, 120, 40 };
    clearButton = { 410, 50, 150, 40 };
}

void ArrayVisualizer::HandleInput() {
    Vector2 mousePos = GetMousePosition();

    // Handle activation of input boxes
    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeValueInput = true;
        activeSizeInput = false;
    }
    else if (CheckCollisionPointRec(mousePos, sizeBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeSizeInput = true;
        activeValueInput = false;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeValueInput = false;
        activeSizeInput = false;
    }

    // Keyboard input
    int key = GetCharPressed();
    while (key > 0) {
        if (isdigit(key)) {
            if (activeValueInput) inputValue += (char)key;
            else if (activeSizeInput) sizeInput += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (activeValueInput && !inputValue.empty()) inputValue.pop_back();
        else if (activeSizeInput && !sizeInput.empty()) sizeInput.pop_back();
    }

    // Set array size
    if (CheckCollisionPointRec(mousePos, setSizeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!sizeInput.empty()) {
            maxSize = std::stoi(sizeInput);
            elements.clear();
            sizeInput.clear();
        }
    }

    // Add element
    if (CheckCollisionPointRec(mousePos, addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!inputValue.empty() && elements.size() < maxSize) {
            int val = std::stoi(inputValue);
            float spacing = 80.0f;
            elements.push_back(AnimatedElement(val, 50 + elements.size() * spacing, -50, screenHeight / 2.0f));
            inputValue.clear();
        }
    }

    // Clear array
    if (CheckCollisionPointRec(mousePos, clearButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        elements.clear();
        maxSize = 0;
        inputValue.clear();
        sizeInput.clear();
        activeSizeInput = true;
        activeValueInput = false;
    }
}

void ArrayVisualizer::UpdateAnimations() {
    const float speed = 5.0f;
    for (auto& e : elements) {
        if (e.y < e.targetY) {
            e.y += speed;
            if (e.y > e.targetY) e.y = e.targetY;
        }
    }
}

void ArrayVisualizer::DrawUI() {
    // Draw array size input
    DrawRectangleRec(sizeBox, activeSizeInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(sizeBox, 2, DARKGRAY);
    DrawText(sizeInput.c_str(), sizeBox.x + 5, sizeBox.y + 5, 20, BLACK);

    DrawRectangleRec(setSizeButton, LIGHTGRAY);
    DrawRectangleLinesEx(setSizeButton, 2, DARKGRAY);
    DrawText("Set Size", setSizeButton.x + 10, setSizeButton.y + 5, 20, BLACK);

    DrawRectangleRec(clearButton, LIGHTGRAY);
    DrawRectangleLinesEx(clearButton, 2, DARKGRAY);
    DrawText("Clear Array", clearButton.x + 10, clearButton.y + 5, 20, BLACK);

    // Draw element input
    DrawRectangleRec(inputBox, activeValueInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
    DrawText(inputValue.c_str(), inputBox.x + 5, inputBox.y + 5, 20, BLACK);

    DrawRectangleRec(addButton, LIGHTGRAY);
    DrawRectangleLinesEx(addButton, 2, DARKGRAY);
    DrawText("Add", addButton.x + 25, addButton.y + 5, 20, BLACK);

    // Show number of elements added / max size
    if (maxSize > 0) {
        DrawText(
            TextFormat("Elements: %d / %d", (int)elements.size(), maxSize),
            inputBox.x, inputBox.y + 50, 20, DARKGRAY
        );
    }
}

void ArrayVisualizer::Draw() {
    for (auto& e : elements) {
        DrawRectangle(e.x, e.y, 60, 40, SKYBLUE);
        DrawRectangleLines(e.x, e.y, 60, 40, DARKBLUE);
        DrawText(std::to_string(e.value).c_str(), e.x + 15, e.y + 10, 20, BLACK);
    }
}
