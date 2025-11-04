#pragma once
#include "raylib.h"
#include <vector>
#include <queue>
#include <string>
#include <functional>

struct TreeNode {
    int value;
    TreeNode* left;
    TreeNode* right;
    Vector2 position;
    Vector2 targetPosition;
    bool animating;
    bool positioned;
    bool searchHighlight = false; // For search animation
};

class BinaryTree {
public:
    BinaryTree(int screenWidth, int screenHeight);

    void Insert(int value);
    void UpdateAnimations();
    void Draw();

    // Binary tree specific UI
    void DrawUI();               // Draw input boxes + search button
    void HandleInput();          // Handle input for search & insert

private:
    TreeNode* root;
    int screenWidth, screenHeight;
    std::vector<TreeNode*> animatingNodes;

    // Search animation
    std::queue<TreeNode*> searchQueue;
    bool searching = false;
    int searchTarget = 0;
    float searchDelay = 0.5f; // seconds between highlighting nodes
    float searchTimer = 0.0f;

    // UI state
    std::string inputValue;
    std::string searchValue;
    bool activeInput = false;
    bool activeSearch = false;
    Rectangle valueBox;
    Rectangle searchBox;
    Rectangle insertBtn;
    Rectangle searchBtn;

    void DrawNode(TreeNode* node);
    void UpdateNode(TreeNode* node, float dt);
    void ComputeNodePositions(TreeNode* node, int depth, int xMin, int xMax);

    void Search(int value);
};
