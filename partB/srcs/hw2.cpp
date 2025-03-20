#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

struct Coordinates {
    pair<int, int> lowerLeft;
    pair<int, int> lowerRight;
    pair<int, int> upperLeft;
    pair<int, int> upperRight;
};

struct Block {
    int width;
    int height;
    Coordinates coordinates;
    bool rotated;
    Block(int w, int h) : width(w), height(h), coordinates(Coordinates()), rotated(false) {}
    void rotate(){
        int temp = width;
        width = height;
        height = temp;
        rotated = !rotated;
    }
};

struct Node {
    char value;
    Block* block;
    vector<pair<unsigned int, pair<int, int>>> graphVertices; // <rotations, <width, height>> || <rotations, <height, width>>
    Node *left;
    Node* right;
    Node(char v) : value(v), block(nullptr), left(nullptr), right(nullptr) {}
};

Node* buildTree(stack<Node*>& npe_stack, vector<Block>& blocks) {
    Node* root = npe_stack.top();
    npe_stack.pop();
    if (isdigit(root->value)) root->block = &blocks.at(int(root->value) - '0');
    if (!isdigit(root->value)) {
        root->right = buildTree(npe_stack, blocks);
        root->left = buildTree(npe_stack, blocks);
    }
    return root;
}

void compute_shape_curve(Node* root) {
    if (root == nullptr) return;

    compute_shape_curve(root->left);
    compute_shape_curve(root->right);

    // leaf node case
    if (root->left == nullptr && root->right == nullptr) {
        root->graphVertices.push_back(make_pair(false, make_pair(root->block->width, root->block->height)));
        root->graphVertices.push_back(make_pair(true, make_pair(root->block->height, root->block->width)));
    }
    else // non-leaf node case
    {
        vector<pair<unsigned int, pair<int, int>>> leftVertices = root->left->graphVertices;
        vector<pair<unsigned int, pair<int, int>>> rightVertices = root->right->graphVertices;
        // compute the graph vertices according to the slicing function
        if (root->value == '+') {
            // add vertical (max x, sum y)

            // Sort left vertices by min width
            sort(leftVertices.begin(), leftVertices.end(), [](const pair<unsigned int, pair<int, int>>& a, const pair<unsigned int, pair<int, int>>& b) {
                return a.second.first < b.second.first;
            });

            // Sort right vertices by min width
            sort(rightVertices.begin(), rightVertices.end(), [](const pair<unsigned int, pair<int, int>>& a, const pair<unsigned int, pair<int, int>>& b) {
                return a.second.first < b.second.first;
            });

            int widthBoundMin = max(leftVertices.front().second.first, rightVertices.front().second.first);

            // left vertices as plane reference
            for (auto it = leftVertices.begin(); it != leftVertices.end(); ++it) 
            {
                auto& vertexL = *it;
                // Construct line segment: (min width, max width)
                pair<int, int> lineSegment;
                if (std::next(it) != leftVertices.end()) // peak next vertex
                {
                    auto& nextVertexL = *std::next(it);
                    lineSegment = make_pair(vertexL.second.first, nextVertexL.second.first);
                } else {
                    lineSegment = make_pair(vertexL.second.first, INT_MAX);
                }
                
                for (auto& vertexR : rightVertices) 
                {
                    if (vertexR.second.first >= widthBoundMin 
                        && (vertexR.second.first >= lineSegment.first || vertexR.second.first <= lineSegment.second)) 
                    {
                        root->graphVertices.push_back(make_pair(vertexL.first << 1 | vertexR.first, make_pair(vertexR.second.first, vertexR.second.second+vertexL.second.second)));
                    }
                }
            }

            // right vertices as plane reference
            for (auto it = rightVertices.begin(); it != rightVertices.end(); ++it) 
            {
                auto& vertexR = *it;
                // Construct line segment: (min width, max width)
                pair<int, int> lineSegment;
                if (std::next(it) != rightVertices.end()) // peak next vertex
                {
                    auto& nextVertexR = *std::next(it);
                    lineSegment = make_pair(vertexR.second.first, nextVertexR.second.first);
                } else {
                    lineSegment = make_pair(vertexR.second.first, INT_MAX);
                }

                for (auto& vertexL : leftVertices)
                {
                    if (vertexL.second.first >= widthBoundMin
                        && (vertexL.second.first >= lineSegment.first || vertexL.second.first <= lineSegment.second))
                    {
                        root->graphVertices.push_back(make_pair(vertexL.first << 1 | vertexR.first, make_pair(vertexL.second.first, vertexL.second.second+vertexR.second.second)));
                    }
                }
            }

            
        } else if (root->value == '*') 
        {
            // add horizontal (sum x, max y)

            // Sort left vertices by min height
            sort(leftVertices.begin(), leftVertices.end(), [](const pair<unsigned int, pair<int, int>>& a, const pair<unsigned int, pair<int, int>>& b) {
                return a.second.second < b.second.second;
            });

            // Sort right vertices by min height
            sort(rightVertices.begin(), rightVertices.end(), [](const pair<unsigned int, pair<int, int>>& a, const pair<unsigned int, pair<int, int>>& b) {
                return a.second.second < b.second.second;
            });

            int heightBoundMin = max(leftVertices.front().second.second, rightVertices.front().second.second);
            
            // left vertices as plane reference
            for (auto it = leftVertices.begin(); it != leftVertices.end(); ++it) 
            {
                auto& vertexL = *it;
                // Construct line segment: (min height, max height)
                pair<int, int> lineSegment;
                if (std::next(it) != leftVertices.end()) // peak next vertex
                {
                    auto& nextVertexL = *std::next(it);
                    lineSegment = make_pair(vertexL.second.second, nextVertexL.second.second);
                } else {
                    lineSegment = make_pair(vertexL.second.second, INT_MAX);
                }

                for (auto& vertexR : rightVertices)
                {
                    if (vertexR.second.second >= heightBoundMin
                        && (vertexR.second.second >= lineSegment.first || vertexR.second.second <= lineSegment.second))
                    {
                        root->graphVertices.push_back(make_pair(vertexL.first << 1 | vertexR.first, make_pair(vertexR.second.first+vertexL.second.first, vertexR.second.second)));
                    }
                }
            }

            // right vertices as plane reference
            for (auto it = rightVertices.begin(); it != rightVertices.end(); ++it) 
            {
                auto& vertexR = *it;
                // Construct line segment: (min height, max height)
                pair<int, int> lineSegment;
                if (std::next(it) != rightVertices.end()) // peak next vertex
                {
                    auto& nextVertexR = *std::next(it);
                    lineSegment = make_pair(vertexR.second.second, nextVertexR.second.second);
                } else {
                    lineSegment = make_pair(vertexR.second.second, INT_MAX);
                }

                for (auto& vertexL : leftVertices)
                {
                    if (vertexL.second.second >= heightBoundMin
                        && (vertexL.second.second >= lineSegment.first || vertexL.second.second <= lineSegment.second))
                    {
                        root->graphVertices.push_back(make_pair(vertexL.first << 1 | vertexR.first, make_pair(vertexL.second.first+vertexR.second.first, vertexL.second.second)));
                    }
                }
            }
        }
    }
}

void compute_coordinates(Node *root, int x_offset = 0, int y_offset = 0)
{
    if (root == nullptr)
        return;

    // If leaf node, set coordinates directly
    if (isdigit(root->value))
    {
        Block *block = root->block;
        block->coordinates.lowerLeft = make_pair(x_offset, y_offset);
        block->coordinates.lowerRight = make_pair(x_offset + block->width, y_offset);
        block->coordinates.upperLeft = make_pair(x_offset, y_offset + block->height);
        block->coordinates.upperRight = make_pair(x_offset + block->width, y_offset + block->height);
        return;
    }

    // Get optimal shape from computed vertices
    auto minVertex = std::min_element(
        root->graphVertices.begin(),
        root->graphVertices.end(),
        [](const auto &a, const auto &b)
        {
            return (a.second.first * a.second.second) < (b.second.first * b.second.second);
        });

    unsigned int minRotations = minVertex->first;

    // Extract rotation information for both children
    bool leftRotated = (minRotations & 2) != 0;
    bool rightRotated = (minRotations & 1) != 0;

    // Apply rotations if needed
    if (root->left->block && leftRotated)
        root->left->block->rotate();
    if (root->right->block && rightRotated)
        root->right->block->rotate();

    if (root->value == '+')
    { // Horizontal cut - blocks stacked bottom to top
        // Left child at the bottom
        compute_coordinates(root->left, x_offset, y_offset);

        // Determine left child's height for positioning right child
        int leftHeight;
        if (isdigit(root->left->value))
        {
            leftHeight = root->left->block->height;
        }
        else
        {
            // Find max y-coordinate in left subtree
            leftHeight = 0;
            for (auto &vertex : root->left->graphVertices)
            {
                if (vertex.first == (minRotations >> 1))
                {
                    leftHeight = vertex.second.second;
                    break;
                }
            }
        }

        // Right child on top of left child
        compute_coordinates(root->right, x_offset, y_offset + leftHeight);
    }
    else if (root->value == '*')
    { // Vertical cut - blocks placed left to right
        // Left child at the left side
        compute_coordinates(root->left, x_offset, y_offset);

        // Determine left child's width for positioning right child
        int leftWidth;
        if (isdigit(root->left->value))
        {
            leftWidth = root->left->block->width;
        }
        else
        {
            // Find max x-coordinate in left subtree
            leftWidth = 0;
            for (auto &vertex : root->left->graphVertices)
            {
                if (vertex.first == (minRotations >> 1))
                {
                    leftWidth = vertex.second.first;
                    break;
                }
            }
        }

        // Right child to the right of left child
        compute_coordinates(root->right, x_offset + leftWidth, y_offset);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        printf("usage: hw2 <in_file> <out_file>, %s is received", argv[0]);
        exit(EXIT_FAILURE);
    }
    char *inFile = argv[1];
    char *outFile = argv[2];

    ifstream input(inFile);
    if (!input.is_open())
    {
        printf("Failed to open input file: %s\n", inFile);
        exit(EXIT_FAILURE);
    }
    ofstream output(outFile);
    if (!output.is_open())
    {
        printf("Failed to create/open output file: %s\n", outFile);
        exit(EXIT_FAILURE);
    }

    vector<Block> blocks;
    
    // Read number of blocks
    int n;
    input >> n;
    
    // Read each block's dimensions
    for(int i = 0; i < n; i++) {
        int width, height;
        if(input >> width >> height) {
            blocks.emplace_back(width, height);
        } else {
            cerr << "Error reading block " << i + 1 << endl;
            break;
        }
    }

    // Read Normalized Polish Expression
    stack<Node*> npe_stack;
    char c;
    int cCount = 0;
    int oCount = 0;
    while (input >> c) {
        if (isdigit(c)) {
            cCount++;
            npe_stack.push(new Node(c));
        } else {
            oCount++;
            if (!(oCount < cCount)) {
                cerr << "Invalid NPE" << endl;
                exit(EXIT_FAILURE);
            }
            npe_stack.push(new Node(c));
        }
    }
    // Build the slicing tree
    Node* slicingTree = buildTree(npe_stack, blocks);
    
    // recursively initialize the shape curve of slicing tree
    compute_shape_curve(slicingTree);

    //TODO: Retrieve the coordinates of the blocks
    compute_coordinates(slicingTree);

    // Output the block coordinates
    for (int i = 0; i < n; i++) {
        Block& block = blocks.at(i);
        output << "(" << block.coordinates.lowerLeft.first << " " << block.coordinates.lowerLeft.second << ")" << " "
               << "(" << block.coordinates.lowerRight.first << " " << block.coordinates.lowerRight.second << ")" << " "
               << "(" << block.coordinates.upperLeft.first << " " << block.coordinates.upperLeft.second << ")" << " "
               << "(" << block.coordinates.upperRight.first << " " << block.coordinates.upperRight.second << ")" << endl;
    }

    //TODO: Calculate the area of the slicing floorplan
    // Find minimum area configuration using min_element lambda
    auto minVertex = std::min_element(
        slicingTree->graphVertices.begin(),
        slicingTree->graphVertices.end(),
        [](const auto& a, const auto& b) {
            return (a.second.first * a.second.second) < (b.second.first * b.second.second);
        }
    );

    // Extract results
    unsigned int minRotations = minVertex->first;
    pair<int, int> minDimensions = minVertex->second;
    int minArea = minDimensions.first * minDimensions.second;

    // Output results
    output << minArea << endl;

    input.close();
    output.close();
    return EXIT_SUCCESS;
}