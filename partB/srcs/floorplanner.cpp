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
    int x, y;
    int lShape, rShape;                                                                 // index of the left and right child
    Rotation rotation;                                                                  // original or rotated for leaf block graph
    GraphVertex(int x, int y, int l, int r) : x(x), y(y), lShape(l), rShape(r) {}       // constructor for internal node graph
    GraphVertex(int x, int y, Rotation r) : x(x), y(y), rotation(r) {}                  // constructor for leaf node graph
};

struct Node
{
    NodeType type;                      // type of node (LEAF or INTERNAL)
    char value;                         // value of node (module index or operator)
    Block *block;                       // pointer to the block/module (only for leaf nodes)
    Node *left;                         // pointer to the left child node
    Node *right;                        // pointer to the right child node
    vector<GraphVertex> shape_curve;    // vector of GraphVertex objects representing the shape curve
    int chosen_index;                   // index of chosen shape

    Node(char v, NodeType t) : type(t), value(v), left(nullptr), right(nullptr) {}
};

/**
 * Recursively constructs a slicing tree from a stack of NPE tokens
 *
 * @param npe_stack Stack containing NPE tokens in postfix notation
 * @param blocks Vector of blocks/modules to be placed in the floorplan
 * @return Root node of the constructed slicing tree
 */
Node *buildTree(stack<Node *> &npe_stack, vector<Block> &blocks)
{
    Node *root = npe_stack.top();
    npe_stack.pop();
    if (root->type == NodeType::LEAF)
    {
        root->block = &blocks.at(root->value);
    }
    else if (root->type == NodeType::INTERNAL)
    {
        // cout << "[debug: buildTree] operator " << root->value << endl;
        // cout << "[debug: buildTree] build right" << endl;
        root->right = buildTree(npe_stack, blocks);
        // cout << "[debug: buildTree] build left" << endl;
        root->left = buildTree(npe_stack, blocks);
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
        root->shape_curve = {
            GraphVertex(root->block->shapes.at(Rotation::ORIGINAL).first, root->block->shapes.at(Rotation::ORIGINAL).second, Rotation::ORIGINAL),
            GraphVertex(root->block->shapes.at(Rotation::ROTATED).first, root->block->shapes.at(Rotation::ROTATED).second, Rotation::ROTATED)
        };
    }
    else
    {
        vector<GraphVertex> candidates;
        vector<GraphVertex> left_curve = root->left->shape_curve;
        vector<GraphVertex> right_curve = root->right->shape_curve;

        // DP: generate all combinations and store in each internal node
        for (int i = 0; i < left_curve.size(); i++)
        {
            for (int j = 0; j < right_curve.size(); j++)
            {
                int x, y;
                if (root->value == '*')
                {
                    x = left_curve.at(i).x + right_curve.at(j).x;
                    y = max(left_curve.at(i).y, right_curve.at(j).y);
                }
                else if (root->value == '+')
                { // '+'
                    x = max(left_curve.at(i).x, right_curve.at(j).x);
                    y = left_curve.at(i).y + right_curve.at(j).y;
                }
                candidates.emplace_back(x, y, i, j);
            }
        }

        // Sort dp result by width ascending, then height ascending
        sort(candidates.begin(), candidates.end(),
             [](const GraphVertex &a, const GraphVertex &b)
             {
                 return (a.x == b.x) ? (a.y < b.y) : (a.x < b.x);
             });

        // Filter and keep non-increasing points
        vector<GraphVertex> result;
        if (!candidates.empty())
        {
            result.push_back(candidates[0]);
            int min_y = candidates[0].y;
            for (size_t k = 1; k < candidates.size(); ++k)
            {
                if (candidates[k].y < min_y)
                {
                    result.push_back(candidates[k]);
                    min_y = candidates[k].y;
                }
            }
        }
        root->shape_curve = result; // store finalized dp result
    }
}

/**
 * @brief Sets the chosen orientation for each block in the floorplan
 *
 * Recursively traverses the slicing tree and sets the orientation of each block
 * based on the chosen_index of each node. For leaf nodes, it directly sets the
 * block's orientation. For internal nodes, it recursively processes left and right
 * subtrees.
 *
 * @param root Current node in the slicing tree
 */
void setOrientation(Node *root)
{
    if (root->type == LEAF)
    {
        root->block->orientation = root->shape_curve[root->chosen_index].rotation;
    }
    else
    {
        int left_idx = root->shape_curve[root->chosen_index].lShape;
        int right_idx = root->shape_curve[root->chosen_index].rShape;
        root->left->chosen_index = left_idx;
        root->right->chosen_index = right_idx;
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
    int idx = root->chosen_index;

    if (root->type == LEAF)
    {
        int w_m = root->block->shapes.at(root->block->orientation).first;
        int h_m = root->block->shapes.at(root->block->orientation).second;
        // cout << "[debug: computeCood] node " << int(root->value) << " starting at (" << x << ", " << y << ")" << endl;
        // cout << "[debug: computeCood] has width: " << w_m << " height: " << h_m << " orientation: " << root->block->orientation << endl;
        root->block->coordinates = Coordinates(
            {x, y},            // Lower left
            {x + w_m, y},      // Lower right
            {x, y + h_m},      // Upper left
            {x + w_m, y + h_m} // Upper right
        );
    }
    else
    {
        int left_idx = root->shape_curve[idx].lShape;
        int w_left = root->left->shape_curve[left_idx].x;
        int h_left = root->left->shape_curve[left_idx].y;

        if (root->value == '*')
        {
            computeCood(root->left, x, y);
            computeCood(root->right, x + w_left, y);
        }
        else
        { // '+'
            computeCood(root->left, x, y);
            computeCood(root->right, x, y + h_left);
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
    stack<Node *> npe_stack;
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
            npe_stack.push(new Node(token[0], NodeType::INTERNAL));
        }
        else
        {
            bCount++;
            npe_stack.push(new Node(stoi(token), NodeType::LEAF));
        }
    }
    Node *root = buildTree(npe_stack, blocks);

    // Compute shape curves
    compute_shape_curve(root);

    // Find minimal area
    int min_area = INT_MAX;
    int best_idx = -1;
    for (int i = 0; i < root->shape_curve.size(); ++i)
    {
        int area = root->shape_curve[i].x * root->shape_curve[i].y;
        if (area < min_area)
        {
            min_area = area;
            best_idx = i;
        }
    }
    root->chosen_index = best_idx;

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
    output << min_area << endl;

    // Clean up (optional, skipped for simplicity)
    // delete root;

    return 0;
}