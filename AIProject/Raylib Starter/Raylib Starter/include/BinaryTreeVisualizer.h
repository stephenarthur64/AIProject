#pragma once
#include "raylib.h"
#include "BinaryTree.h"

class BinaryTreeVisualizer {
public:
    BinaryTreeVisualizer(int width, int height);
    void Update();
    void Draw();

private:
    int screenWidth, screenHeight;
    BinaryTree tree;
    bool activeInputBox;
    std::string inputValue;
};
