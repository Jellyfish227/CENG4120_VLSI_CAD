#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <climits>

using namespace std;

enum NodeType
{
    LEAF = 0,
    INTERNAL = 1
};

enum Rotation
{
    NONE = -1,
    ORIGINAL = 0,
    ROTATED = 1
};

struct Coordinates
{
    pair<int, int> lowerLeft;
    pair<int, int> lowerRight;
    pair<int, int> upperLeft;
    pair<int, int> upperRight;
    Coordinates(
        pair<int, int> lowerLeft,
        pair<int, int> lowerRight,
        pair<int, int> upperLeft,
        pair<int, int> upperRight) : lowerLeft(lowerLeft), lowerRight(lowerRight), upperLeft(upperLeft), upperRight(upperRight) {}
    Coordinates() {}
};

struct Block
{
    vector<pair<int, int>> shapes; // <width, height>
    Coordinates coordinates;
    Rotation orientation;
    Block(int w, int h)
    {
        shapes.push_back(make_pair(w, h));
        shapes.push_back(make_pair(h, w));
    }
    Block() {}
};

struct GraphVertex
{
    int w, h;
    int lShape, rShape;                                                                 // index of the left and right child
    Rotation rotation;                                                                  // original or rotated for leaf block graph
    GraphVertex(int w, int h, int l, int r) : w(w), h(h), lShape(l), rShape(r) {}       // constructor for internal node graph
    GraphVertex(int w, int h, Rotation r) : w(w), h(h), rotation(r) {}                  // constructor for leaf node graph
};

struct Node
{
    NodeType type;                      // type of node (LEAF or INTERNAL)
    char value;                         // value of node (module index or operator)
    Block *block;                       // pointer to the block/module (only for leaf nodes)
    Node *left;                         // pointer to the left child node
    Node *right;                        // pointer to the right child node
    vector<GraphVertex> shapeCurve;    // vector of GraphVertex objects representing the shape curve
    int minGraphIndex;                   // index of chosen shape

    Node(char v, NodeType t) : type(t), value(v), left(nullptr), right(nullptr) {}
};

/**
 * Recursively constructs a slicing tree from a stack of NPE tokens
 *
 * @param npeStack Stack containing NPE tokens in postfix notation
 * @param blocks Vector of blocks/modules to be placed in the floorplan
 * @return Root node of the constructed slicing tree
 */
Node *buildTree(stack<Node *> &npeStack, vector<Block> &blocks)
{
    Node *root = npeStack.top();
    npeStack.pop();
    if (root->type == NodeType::LEAF)
    {
        root->block = &blocks.at(root->value);
    }
    else if (root->type == NodeType::INTERNAL)
    {
        // cout << "[debug: buildTree] operator " << root->value << endl;
        // cout << "[debug: buildTree] build right" << endl;
        root->right = buildTree(npeStack, blocks);
        // cout << "[debug: buildTree] build left" << endl;
        root->left = buildTree(npeStack, blocks);
    }
    return root;
}

/**
 * @brief Computes the shape curve for each node in the slicing tree
 *
 * Use dynamic programming to compute all possible width-height configurations.
 * For leaf nodes: creates shape curve with two points (original and rotated orientations).
 * 
 * For '+' (vertical cut): (max width, sum height)
 * 
 * For '*' (horizontal cut): (sum width, max height)
 *
 * @param root Root node of the slicing tree
 */
void compute_shape_curve(Node *root)
{
    if (root == nullptr)
        return;

    // Process left and right subtrees first
    compute_shape_curve(root->left);
    compute_shape_curve(root->right);

    if (root->type == LEAF)
    {
        root->shapeCurve = {
            GraphVertex(root->block->shapes.at(Rotation::ORIGINAL).first, root->block->shapes.at(Rotation::ORIGINAL).second, Rotation::ORIGINAL),
            GraphVertex(root->block->shapes.at(Rotation::ROTATED).first, root->block->shapes.at(Rotation::ROTATED).second, Rotation::ROTATED)
        };
    }
    else
    {
        vector<GraphVertex> dp;
        vector<GraphVertex> leftCurve = root->left->shapeCurve;
        vector<GraphVertex> rightCurve = root->right->shapeCurve;

        // DP: generate all combinations and store in each internal node
        for (int i = 0; i < leftCurve.size(); i++)
        {
            for (int j = 0; j < rightCurve.size(); j++)
            {
                int w, h;
                if (root->value == '*')
                {
                    w = leftCurve.at(i).w + rightCurve.at(j).w;
                    h = max(leftCurve.at(i).h, rightCurve.at(j).h);
                }
                else if (root->value == '+')
                { // '+'
                    w = max(leftCurve.at(i).w, rightCurve.at(j).w);
                    h = leftCurve.at(i).h + rightCurve.at(j).h;
                }
                dp.emplace_back(w, h, i, j);
            }
        }

        // Sort dp result by width ascending, then height ascending
        sort(dp.begin(), dp.end(),
             [](const GraphVertex &a, const GraphVertex &b)
             {
                 return (a.w == b.w) ? (a.h < b.h) : (a.w < b.w);
             });

        // Filter and keep non-increasing points
        vector<GraphVertex> result;
        if (!dp.empty())
        {
            result.push_back(dp.at(0));
            int minH = dp.at(0).h;
            for (size_t k = 1; k < dp.size(); ++k)
            {
                if (dp.at(k).h < minH)
                {
                    result.push_back(dp.at(k));
                    minH = dp.at(k).h;
                }
            }
        }
        root->shapeCurve = result; // store finalized dp result
    }
}

/**
 * @brief Sets the chosen orientation for each block in the floorplan
 *
 * Recursively traverses the slicing tree and sets the orientation of each block
 * based on the minGraphIndex of each node. For leaf nodes, it directly sets the
 * block's orientation. For internal nodes, it recursively processes left and right
 * subtrees.
 *
 * @param root Current node in the slicing tree
 */
void setOrientation(Node *root)
{
    if (root->type == LEAF)
    {
        root->block->orientation = root->shapeCurve.at(root->minGraphIndex).rotation;
    }
    else
    {
        int leftIdx = root->shapeCurve.at(root->minGraphIndex).lShape;
        int rightIdx = root->shapeCurve.at(root->minGraphIndex).rShape;
        root->left->minGraphIndex = leftIdx;
        root->right->minGraphIndex = rightIdx;
        setOrientation(root->left);
        setOrientation(root->right);
    }
}

/**
 * @brief Assigns physical coordinates to each block in the floorplan
 *
 * Recursively traverses the slicing tree and assigns the coordinates
 * to each block based on the chosen shape and cut type.
 * For leaf nodes: directly sets the block's coordinates.
 * For internal nodes:
 *   - For '*' (horizontal cut): places right child to the right of left child
 *   - For '+' (vertical cut): places right child above the left child
 *
 * @param root Current node in the slicing tree
 * @param x X-coordinate of the current node's lower-left corner
 * @param y Y-coordinate of the current node's lower-left corner
 */
void computeCood(Node *root, int x, int y)
{
    int idx = root->minGraphIndex;

    if (root->type == LEAF)
    {
        int wMod = root->block->shapes.at(root->block->orientation).first;
        int hMod = root->block->shapes.at(root->block->orientation).second;
        // cout << "[debug: computeCood] node " << int(root->value) << " starting at (" << x << ", " << y << ")" << endl;
        // cout << "[debug: computeCood] has width: " << wMod << " height: " << hMod << " orientation: " << root->block->orientation << endl;
        root->block->coordinates = Coordinates(
            {x, y},            // Lower left
            {x + wMod, y},      // Lower right
            {x, y + hMod},      // Upper left
            {x + wMod, y + hMod} // Upper right
        );
    }
    else
    {
        int leftIdx = root->shapeCurve.at(idx).lShape;
        int wLeftMod = root->left->shapeCurve.at(leftIdx).w;
        int hLeftMod = root->left->shapeCurve.at(leftIdx).h;

        if (root->value == '*')
        {
            computeCood(root->left, x, y);
            computeCood(root->right, x + wLeftMod, y);
        }
        else
        { // '+'
            computeCood(root->left, x, y);
            computeCood(root->right, x, y + hLeftMod);
        }
    }
}

int main(int argc, char *argv[])
{
    // Command-line argument handling
    if (argc < 2 || argc > 3)
    {
        printf("usage: ./floorplanner <in_file> <out_file>, %s is received", argv[0]);
        exit(EXIT_FAILURE);
    }
    ifstream input(argv[1]);
    ofstream output(argv[2]);
    if (!input.is_open() || !output.is_open())
    {
        cerr << "Failed to open file" << endl;
        return 1;
    }

    // Read input
    int n;
    input >> n;
    vector<Block> blocks;

    for (int i = 0; i < n; ++i)
    {
        int width, height;
        if (input >> width >> height)
        {
            blocks.emplace_back(width, height);
        }
        else
        {
            cerr << "Error reading block " << i + 1 << endl;
            break;
        }
    }

    // Read NPE
    string line;
    getline(input, line); // Consume newline
    getline(input, line);
    vector<string> tokens;
    istringstream ss(line);
    string token;
    while (ss >> token)
    {
        tokens.push_back(token);
    }

    // Build the expression tree stack
    stack<Node *> npeStack;
    int bCount = 0, oCount = 0;
    for (const auto &token : tokens)
    {
        if (token == "+" || token == "*")
        {
            oCount++;
            if (!(oCount < bCount))
            {
                cerr << "Invalid NPE" << endl;
                exit(EXIT_FAILURE);
            }
            npeStack.push(new Node(token[0], NodeType::INTERNAL));
        }
        else
        {
            bCount++;
            npeStack.push(new Node(stoi(token), NodeType::LEAF));
        }
    }
    Node *root = buildTree(npeStack, blocks);

    // Compute shape curves
    compute_shape_curve(root);

    // Find minimal area
    int minArea = INT_MAX;
    int bestIdx = -1;
    for (int i = 0; i < root->shapeCurve.size(); ++i)
    {
        int area = root->shapeCurve.at(i).w * root->shapeCurve.at(i).h;
        if (area < minArea)
        {
            minArea = area;
            bestIdx = i;
        }
    }
    root->minGraphIndex = bestIdx;

    // Set module orientations
    setOrientation(root);

    // Assign coordinates
    computeCood(root, 0, 0);

    // Write output
    for (int i = 0; i < n; i++)
    {
        Block &block = blocks.at(i);
        output << "(" << block.coordinates.lowerLeft.first << " " << block.coordinates.lowerLeft.second << ")" << " "
               << "(" << block.coordinates.lowerRight.first << " " << block.coordinates.lowerRight.second << ")" << " "
               << "(" << block.coordinates.upperLeft.first << " " << block.coordinates.upperLeft.second << ")" << " "
               << "(" << block.coordinates.upperRight.first << " " << block.coordinates.upperRight.second << ") " << endl;
    }
    output << minArea << endl;

    input.close();
    output.close();
    return 0;
}