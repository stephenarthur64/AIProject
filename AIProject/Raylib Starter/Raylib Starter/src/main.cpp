#include "raylib.h"
#include "LinkedListVisualizer.h"
#include "BinaryTreeVisualizer.h"
#include "ArrayVisualizer.h"

enum AppMode { MENU, LINKED_LIST, BINARY_TREE, ARRAY };

int main() {
    const int screenWidth = 1600;
    const int screenHeight = 900;

    InitWindow(screenWidth, screenHeight, "Data Structure Visualizer (Scaled UI)");
    SetTargetFPS(60);

    AppMode mode = MENU;
    LinkedListVisualizer listVis;
    BinaryTreeVisualizer treeVis(screenWidth, screenHeight);
    ArrayVisualizer array(screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        float uiScale = screenWidth / 1600.0f;
        int fontSize = (int)(40 * uiScale);

        if (mode == MENU) {
            // --- MAIN MENU ---
            Rectangle llBtn = { screenWidth / 2.0f - 150, 300, 300, 60 };
            Rectangle btBtn = { screenWidth / 2.0f - 150, 400, 300, 60 };
            Rectangle arrBtn = { screenWidth / 2.0f - 150, 500, 300, 60 };

            Vector2 mousePos = GetMousePosition();

            // Draw and detect hover for each menu button
            auto drawMenuButton = [&](Rectangle rect, const char* text) {
                Color btnColor = CheckCollisionPointRec(mousePos, rect) ? DARKGRAY : LIGHTGRAY;
                DrawRectangleRec(rect, btnColor);
                DrawRectangleLinesEx(rect, 2, BLACK);
                DrawText(text, rect.x + 50, rect.y + 15, 30, BLACK);
                };

            drawMenuButton(llBtn, "Linked List");
            drawMenuButton(btBtn, "Binary Tree");
            drawMenuButton(arrBtn, "Array");

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, llBtn)) mode = LINKED_LIST;
                else if (CheckCollisionPointRec(mousePos, btBtn)) mode = BINARY_TREE;
                else if (CheckCollisionPointRec(mousePos, arrBtn)) mode = ARRAY;
            }
        }
        else {
            // --- BACK BUTTON ---
            Rectangle backBtn = { 20, 20, 120, 40 };
            Vector2 mousePos = GetMousePosition();
            Color backColor = LIGHTGRAY;

            bool hovered = CheckCollisionPointRec(mousePos, backBtn);
            if (hovered) backColor = DARKGRAY;
            if (hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) backColor = GRAY;

            DrawRectangleRec(backBtn, backColor);
            DrawRectangleLinesEx(backBtn, 2, BLACK);
            DrawText("Back", backBtn.x + 30, backBtn.y + 10, 20, BLACK);

            // Handle back button click
            if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                mode = MENU;
            }

            // Also allow ESC to go back
            if (IsKeyPressed(KEY_ESCAPE)) mode = MENU;

            // --- VISUALIZER HANDLING ---
            if (mode == LINKED_LIST) {
                listVis.Update();
                listVis.Draw();
            }
            else if (mode == BINARY_TREE) {
                treeVis.Update();
                treeVis.Draw();
            }
            else if (mode == ARRAY) {
                array.HandleInput();
                array.UpdateAnimations();
                array.DrawUI();
                array.Draw();
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
