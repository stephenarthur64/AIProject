#include "ArrayVisualizer.h"
#include <cstdlib>

ArrayVisualizer::ArrayVisualizer(int w, int h) : screenWidth(w), screenHeight(h) {
    inputBox = { 50, 170, 200, 40 };
    addButton = { 270, 170, 120, 40 };
    sizeBox = { 50, 100, 200, 40 };
    setSizeButton = { 270, 100, 120, 40 };
    clearButton = { 410, 100, 150, 40 };

    indexBox = { 50, 240, 200, 40 };
    setAtIndexButton = { 270, 240, 150, 40 };
}


void ArrayVisualizer::HandleInput() {
    Vector2 mousePos = GetMousePosition();

    // Handle activation of input boxes
    if (CheckCollisionPointRec(mousePos, inputBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeValueInput = true;
        activeSizeInput = false;
        activeIndexInput = false;
    }
    else if (CheckCollisionPointRec(mousePos, sizeBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeSizeInput = true;
        activeValueInput = false;
        activeIndexInput = false;
    }
    else if (CheckCollisionPointRec(mousePos, indexBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeIndexInput = true;
        activeSizeInput = false;
        activeValueInput = false;
    }
    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        activeValueInput = false;
        activeSizeInput = false;
        activeIndexInput = false;
    }

    // Keyboard input
    int key = GetCharPressed();
    while (key > 0) {
        if (isdigit(key)) {
            if (activeValueInput) inputValue += (char)key;
            else if (activeSizeInput) sizeInput += (char)key;
            else if (activeIndexInput) indexInput += (char)key;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (activeValueInput && !inputValue.empty()) inputValue.pop_back();
        else if (activeSizeInput && !sizeInput.empty()) sizeInput.pop_back();
        else if (activeIndexInput && !indexInput.empty()) indexInput.pop_back();
    }

    // Set array size
    if (CheckCollisionPointRec(mousePos, setSizeButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!sizeInput.empty()) {
            maxSize = std::stoi(sizeInput);
            elements.clear();
            sizeInput.clear();
        }
    }

    // Add element sequentially (append)
    if (CheckCollisionPointRec(mousePos, addButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!inputValue.empty() && elements.size() < maxSize) {
            int val = std::stoi(inputValue);
            float spacing = 80.0f;
            float startX = 50.0f;
            float startY = screenHeight / 2.0f;
            elements.push_back(AnimatedElement(val, startX + elements.size() * spacing, -50, startY));
            inputValue.clear();
        }
    }

    // Set element at specific index
    // Set element at specific index
    if (CheckCollisionPointRec(mousePos, setAtIndexButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!inputValue.empty() && !indexInput.empty() && maxSize > 0) {
            int val = std::stoi(inputValue);
            int index = std::stoi(indexInput);

            if (index >= 0 && index < maxSize) {
                float spacing = 80.0f;
                float startX = 50.0f;
                float startY = screenHeight / 2.0f;

                if (index < (int)elements.size()) {
                    // Replace existing value
                    elements[index].value = val;
                    elements[index].highlightTimer = 0.5f; // 0.5 seconds of highlight
                    elements[index].y = startY - 15; // small animation bump
                }
                else {
                    // Add empty placeholders until we reach index
                    while ((int)elements.size() < index)
                        elements.push_back(AnimatedElement(0, startX + elements.size() * spacing, startY, startY));

                    // Add new element
                    AnimatedElement newElem(val, startX + index * spacing, -50, startY);
                    newElem.highlightTimer = 0.5f; // flash when added
                    elements.push_back(newElem);
                }

                inputValue.clear();
                indexInput.clear();
            }
        }
    }


    // Clear array
    if (CheckCollisionPointRec(mousePos, clearButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        elements.clear();
        maxSize = 0;
        inputValue.clear();
        sizeInput.clear();
        indexInput.clear();
        activeSizeInput = true;
        activeValueInput = false;
        activeIndexInput = false;
    }
}


void ArrayVisualizer::UpdateAnimations() {
    const float speed = 5.0f;
    for (auto& e : elements) {
        // Smooth drop animation
        if (e.y < e.targetY) {
            e.y += speed;
            if (e.y > e.targetY) e.y = e.targetY;
        }

        // Decrease highlight timer
        if (e.highlightTimer > 0.0f)
            e.highlightTimer -= GetFrameTime();
    }
}


void ArrayVisualizer::DrawUI() {
    // Array size
    DrawRectangleRec(sizeBox, activeSizeInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(sizeBox, 2, DARKGRAY);
    DrawText(sizeInput.c_str(), sizeBox.x + 5, sizeBox.y + 5, 20, BLACK);
    DrawRectangleRec(setSizeButton, LIGHTGRAY);
    DrawRectangleLinesEx(setSizeButton, 2, DARKGRAY);
    DrawText("Set Size", setSizeButton.x + 10, setSizeButton.y + 5, 20, BLACK);

    // Clear button
    DrawRectangleRec(clearButton, LIGHTGRAY);
    DrawRectangleLinesEx(clearButton, 2, DARKGRAY);
    DrawText("Clear Array", clearButton.x + 10, clearButton.y + 5, 20, BLACK);

    // Value input
    DrawRectangleRec(inputBox, activeValueInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(inputBox, 2, DARKGRAY);
    DrawText(inputValue.c_str(), inputBox.x + 5, inputBox.y + 5, 20, BLACK);
    DrawRectangleRec(addButton, LIGHTGRAY);
    DrawRectangleLinesEx(addButton, 2, DARKGRAY);
    DrawText("Add", addButton.x + 35, addButton.y + 5, 20, BLACK);

    // Index input
    DrawRectangleRec(indexBox, activeIndexInput ? RAYWHITE : LIGHTGRAY);
    DrawRectangleLinesEx(indexBox, 2, DARKGRAY);
    DrawText(indexInput.c_str(), indexBox.x + 5, indexBox.y + 5, 20, BLACK);
    DrawRectangleRec(setAtIndexButton, LIGHTGRAY);
    DrawRectangleLinesEx(setAtIndexButton, 2, DARKGRAY);
    DrawText("Set At Index", setAtIndexButton.x + 10, setAtIndexButton.y + 5, 20, BLACK);

    // Info
    if (maxSize > 0)
        DrawText(TextFormat("Elements: %d / %d", (int)elements.size(), maxSize),
            inputBox.x, inputBox.y + 50, 20, DARKGRAY);
}


void ArrayVisualizer::Draw() {
    if (maxSize <= 0) return; // Don’t draw anything until array size is set

    float startX = 50.0f;
    float startY = screenHeight / 2.0f;
    float boxWidth = 60.0f;
    float boxHeight = 40.0f;
    float spacing = 80.0f;

    // --- Draw array slots ---
    for (int i = 0; i < maxSize; i++) {
        float x = startX + i * spacing;
        DrawRectangleLines(x, startY, boxWidth, boxHeight, GRAY);
        DrawText(TextFormat("%d", i), x + 20, startY + boxHeight + 10, 18, DARKGRAY);
    }

    // --- Draw actual elements ---
    for (auto& e : elements) {
        // Fade highlight color over time
        Color color = SKYBLUE;
        if (e.highlightTimer > 0.0f) {
            float t = e.highlightTimer / 0.5f; // normalize to [0,1]
            color = ColorAlpha(YELLOW, t);     // brighter at first, fading out
        }

        DrawRectangle(e.x, e.y, boxWidth, boxHeight, color);
        DrawRectangleLines(e.x, e.y, boxWidth, boxHeight, DARKBLUE);
        DrawText(std::to_string(e.value).c_str(), e.x + 15, e.y + 10, 20, BLACK);
    }

    // Optional title
    DrawText("Array Visualization", startX, startY - 60, 24, DARKGRAY);
}


