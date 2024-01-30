class Vertex2D {
public:
    float x, y;
    float r, g, b; // RGB color

    Vertex2D(float x, float y, float r, float g, float b) : x(x), y(y), r(r), g(g), b(b) {}
};

class Vertex3D {
public:
    float x, y, z;
    float r, g, b; // RGB color

    Vertex3D(float x, float y, float z, float r, float g, float b) : x(x), y(y), z(z), r(r), g(g), b(b) {}
};