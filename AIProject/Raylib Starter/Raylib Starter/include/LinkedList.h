#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include "globals.h"

// -----------------------------------------------------------------------------
// Node structure
// -----------------------------------------------------------------------------
struct Node {
    int value;
    Node* next;
    float x, y;

    Node(int v, float xpos, float ypos);
};

// -----------------------------------------------------------------------------
// Animated node for smooth entry
// -----------------------------------------------------------------------------
struct AnimatedNode {
    Node* node;
    float targetY;

    AnimatedNode(Node* n, float targetY);
};

// -----------------------------------------------------------------------------
// Animated pointer (arrow animation between nodes)
// -----------------------------------------------------------------------------
struct AnimatedPointer {
    Vector2 start;
    Vector2 end;
    Vector2 current;
    bool active;
    float progress;
    float speed;

    AnimatedPointer(Vector2 s, Vector2 e);

    void Update();
    void Draw();
    bool IsFinished() const;
};

// -----------------------------------------------------------------------------
// LinkedList data structure + animation
// -----------------------------------------------------------------------------
class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    void AddNode(int value);
    void InsertNodeAt(int index, int value);
    void DeleteLastNode();

    void UpdateAnimations();
    void Draw();

private:
    Node* head;
    std::vector<Node*> nodes;
    std::vector<AnimatedNode> animatedNodes;
    std::vector<AnimatedPointer> animatedPointers;

    void UpdateLinks();
};
