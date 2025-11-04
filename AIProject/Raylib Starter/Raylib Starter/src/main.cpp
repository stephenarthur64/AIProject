#include "raylib.h"
#include "LinkedListVisualizer.h"
#include "BinaryTreeVisualizer.h"
#include "ArrayVisualizer.h"

enum AppMode { MENU, LINKED_LIST, BINARY_TREE, ARRAY };

int main() {
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
            // Draw menu
            Rectangle llBtn = { screenWidth / 2.0f - 150, 300, 300, 60 };
            Rectangle btBtn = { screenWidth / 2.0f - 150, 400, 300, 60 };
            Rectangle arrBtn = { screenWidth / 2.0f - 150, 500, 300, 60 };

            DrawRectangleRec(llBtn, LIGHTGRAY);
            DrawRectangleRec(btBtn, LIGHTGRAY);
            DrawRectangleRec(arrBtn, LIGHTGRAY);

            DrawText("Linked List", llBtn.x + 50, llBtn.y + 15, 30, BLACK);
            DrawText("Binary Tree", btBtn.x + 50, btBtn.y + 15, 30, BLACK);
            DrawText("Array", arrBtn.x + 50, arrBtn.y + 15, 30, BLACK);

            Vector2 mousePos = GetMousePosition();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (CheckCollisionPointRec(mousePos, llBtn)) mode = LINKED_LIST;
                else if (CheckCollisionPointRec(mousePos, btBtn)) mode = BINARY_TREE;
                else if (CheckCollisionPointRec(mousePos, arrBtn)) mode = ARRAY;
            }
        }
        else if (mode == LINKED_LIST) {
            if (IsKeyPressed(KEY_ESCAPE)) mode = MENU;
            listVis.Update();
            listVis.Draw();
        }
        else if (mode == BINARY_TREE) {
            if (IsKeyPressed(KEY_ESCAPE)) mode = MENU;
            treeVis.Update();
            treeVis.Draw();
        }
        else if (mode == ARRAY)
        {
            array.HandleInput();
            array.UpdateAnimations();
            array.DrawUI();
            array.Draw();
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
