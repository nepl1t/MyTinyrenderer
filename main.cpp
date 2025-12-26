#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <utility>
#include "tgaimage.h"
#include "model.h"
// #define _TESTCASE_FLOAT_ 1

// #define OBJ_PATH "../obj/african_head/african_head.obj"
#define OBJ_PATH "../obj/diablo3_pose/diablo3_pose.obj"

#define rotation_angle_y (M_PI/6)
#define persp_c 2.4f

using std::pair, std::swap, std::abs;


// Return Rotation Matrix [Column][Row]
vec3<vec3<float>> Rotation_y(float theta){
    vec3<vec3<float>> res = {
        {cosf32(theta), 0, -sinf32(theta)}, // column 0
        {0,1,0},                            // column 1
        {sinf32(theta), 0, cosf32(theta)}   // column 2
    };
    return res;
}

// 一个简单的三维点结构体
struct Point {
    int x;
    int y; 
    int z;
    Point() : x(0), y(0), z(0) {}
    Point(int x_val, int y_val) : x(x_val), y(y_val), z(255) {}
    Point(int x_val, int y_val, int z_val) : x(x_val), y(y_val), z(z_val) {}
    Point(const std::pair<int, int>& p) : x(p.first), y(p.second), z(255) {}
    Point(const vec3<float>& v) : x(static_cast<int>(v.x)), y(static_cast<int>(v.y)), z(static_cast<int>(v.z)) {}
    std::pair<int, int> to_pair() const {
        return std::make_pair(x, y);
    }

    const bool operator < (const Point &Y){
        if(x == Y.x) return y < Y.y;
        else return x < Y.x;
    }
};


constexpr TGAColor white   = {{255, 255, 255, 255}}; // attention, BGRA order
constexpr TGAColor green   = {{  0, 255,   0, 255}};
constexpr TGAColor red     = {{  0,   0, 255, 255}};
constexpr TGAColor blue    = {{255, 128,  64, 255}};
constexpr TGAColor yellow  = {{  0, 200, 255, 255}};

vec3<float> rotation(vec3<float> v, float angle_y){
    vec3<vec3<float>> RMatrix = Rotation_y(angle_y);
    vec3<float> res{0,0,0};
    for(int i = 0; i < 3; i++){
        for(int j = 0; j < 3; j++)
            res[i] += RMatrix[j][i] * v[j];
    }
    return res;
}

vec3<float> persp(vec3<float> v, float c){
    return vec3<float>{v.x / (1 - v.z / c), v.y / (1 - v.z / c), v.z};
}


// Bresenham's line algorithm 实现线段绘制
void draw_line(Point A, Point B, TGAColor color,TGAImage &framebuffer){
    bool steep = false;

    if(abs(B.y - A.y) > abs(B.x - A.x)){
        swap(A.x, A.y);
        swap(B.x, B.y);
        steep = true;
    }

    if (B < A) swap(A, B);
    int dx = B.x - A.x;
    if(dx == 0) return; // A and B are the same point
    // float k = (B.y - A.y) / static_cast<float>(dx);
    // float y = static_cast<int>(A.y);
    int error = 0;
    int y = A.y;
    for (int x = A.x; x <= B.x; ++x) {
        if (!steep) framebuffer.set(x, y, color);
        else        framebuffer.set(y, x, color);
        error += 2 * abs(B.y - A.y);
        
        y += (B.y > A.y ? 1 : -1) * (error > dx);
        error -= (2 * dx) * (error > dx);
    }
}


void find_bounding_box(Point A, Point B, Point C, Point& min_point, Point& max_point){
    min_point = Point(std::min(std::min(A.x,B.x),C.x), std::min(std::min(A.y,B.y),C.y));
    max_point = Point(std::max(std::max(A.x,B.x),C.x), std::max(std::max(A.y,B.y),C.y));
}

inline float signedArea(Point A, Point B, Point C){
    return .5f * ((B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x));
}

static bool insideTriangle(Point tri_0, Point tri_1, Point tri_2, Point t)
{   
    const int64_t cross1 = (tri_1.x - tri_0.x) * (t.y - tri_0.y) - (tri_1.y - tri_0.y) * (t.x - tri_0.x);
    const int64_t cross2 = (tri_2.x - tri_1.x) * (t.y - tri_1.y) - (tri_2.y - tri_1.y) * (t.x - tri_1.x);
    const int64_t cross3 = (tri_0.x - tri_2.x) * (t.y - tri_2.y) - (tri_0.y - tri_2.y) * (t.x - tri_2.x);

    const bool allPos = (cross1 >= 0) && (cross2 >= 0) && (cross3 >= 0);
    const bool allNeg = (cross1 <= 0) && (cross2 <= 0) && (cross3 <= 0);

    return (allPos || allNeg);
}

// 如果不使用插值：简单的三角形绘制函数，颜色为传递的固定颜色
// 如果使用插值：忽略传递的颜色参数，基于顶点深度值进行插值的三角形绘制函数，实现简单的灰度渐变效果
void draw_triangle(Point A, Point B, Point C, TGAColor color, TGAImage &framebuffer, TGAImage &zbuffer){
    Point bbox[2];
    find_bounding_box(A,B,C, bbox[0], bbox[1]);
    for(int x = bbox[0].x; x <= bbox[1].x; x++){
        for(int y = bbox[0].y; y <= bbox[1].y; y++){
            // Barycentric coordinates method
            float area_ABC = signedArea(A, B, C);
            float alpha = signedArea(Point(x,y), B, C) / area_ABC;
            float beta = signedArea(A, Point(x,y), C) / area_ABC;
            float gamma = signedArea(A, B, Point(x,y)) / area_ABC;
            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            // uint8_t interpolate_color = alpha * A.z + beta * B.z + gamma * C.z;
            uint8_t interpolate_depth = static_cast<uint8_t>(alpha * A.z + beta * B.z + gamma * C.z);
            if (interpolate_depth <= zbuffer.get(x, y)[0]) continue;
            zbuffer.set(x, y, {interpolate_depth});
            // Grayscale interpolation
            framebuffer.set(x, y, color);

            // If not using interpolation during rasterization, use this faster method
            // if(insideTriangle(A,B,C, {x, y})){
            //     framebuffer.set(x, y, color);
            // }
        }
    }
}

// 根据三个顶点的颜色进行插值的三角形绘制函数，实现颜色渐变效果
void draw_triangle_color_interpolate(Point A, Point B, Point C, TGAColor colorA, TGAColor colorB, TGAColor colorC, TGAImage &framebuffer){
    Point bbox[2];
    find_bounding_box(A,B,C, bbox[0], bbox[1]);
    for(int x = bbox[0].x; x <= bbox[1].x; x++){
        for(int y = bbox[0].y; y <= bbox[1].y; y++){
            // Barycentric coordinates method
            float area_ABC = signedArea(A, B, C);
            float alpha = signedArea(Point(x,y), B, C) / area_ABC;
            float beta = signedArea(A, Point(x,y), C) / area_ABC;
            float gamma = signedArea(A, B, Point(x,y)) / area_ABC;
            if(alpha < 0 || beta < 0 || gamma < 0) continue;
            uint8_t interpolate_color[3]=
            {
                static_cast<uint8_t>(alpha * colorA[0] + beta * colorB[0] + gamma * colorC[0]),
                static_cast<uint8_t>(alpha * colorA[1] + beta * colorB[1] + gamma * colorC[1]),
                static_cast<uint8_t>(alpha * colorA[2] + beta * colorB[2] + gamma * colorC[2])
            };
            framebuffer.set(x, y, {interpolate_color[0], interpolate_color[1], interpolate_color[2], 255}); 
        }
    }
}



void triangle_test(TGAImage &framebuffer, TGAImage &zbuffer){
    int width = framebuffer.width();
    int height = framebuffer.height();
    std::srand(std::time(0));
    for (int i=0; i<(1<<20); i++) {
        int ax = rand()%width, ay = rand()%height;
        int bx = rand()%width, by = rand()%height;
        int cx = rand()%width, cy = rand()%height;
        draw_triangle({ax,ay},{bx,by},{cx, cy},{static_cast<uint8_t>(rand()%255),
                                                static_cast<uint8_t>(rand()%255), 
                                                static_cast<uint8_t>(rand()%255), 
                                                static_cast<uint8_t>(rand()%255)}, 
                                                framebuffer, zbuffer);
    }
}

static bool is_back_face_culled(Point A, Point B, Point C){
    const int64_t cross = (B.x - A.x) * (C.y - A.y) - (B.y - A.y) * (C.x - A.x);
    return cross < 0;
    // return 
}

Point project(vec3<float> v, int width, int height){
    int x = static_cast<int>((v.x + 1.0f) * width  / 2.0f);
    int y = static_cast<int>((v.y + 1.0f) * height / 2.0f);
    int z = static_cast<int>((v.z + 1.0f) * 255.0f / 2.0f);
    return Point(x, y, z);
}

void draw_model(Model &model, TGAImage &framebuffer, TGAImage &zbuffer){
    for(int i = 0; i < model.get_num_faces(); i++){
        vec3<float> v0 = model.vert(i, 0);
        vec3<float> v1 = model.vert(i, 1);
        vec3<float> v2 = model.vert(i, 2);

        // Alternatively, using face indices:
        // vec3<vec3<int>> face = model.face(i);
        // vec3<float> v0 = model.vert(face[0][0]);
        // vec3<float> v1 = model.vert(face[1][0]);
        // vec3<float> v2 = model.vert(face[2][0]);
        Point A,B,C;
        A = project(persp(rotation(v0, rotation_angle_y), persp_c), framebuffer.width(), framebuffer.height());
        B = project(persp(rotation(v1, rotation_angle_y), persp_c), framebuffer.width(), framebuffer.height());
        C = project(persp(rotation(v2, rotation_angle_y), persp_c), framebuffer.width(), framebuffer.height()); 
        if(is_back_face_culled(A,B,C))continue;

        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        draw_triangle(A, B, C, rnd, framebuffer, zbuffer);
        
        // draw_line(A, B, red, framebuffer);
        // draw_line(B, C, red, framebuffer);
        // draw_line(C, A, red, framebuffer);
        
        // framebuffer.set(A.x, A.y, white);
        // framebuffer.set(B.x, B.y, white);
        // framebuffer.set(C.x, C.y, white);
    }
    return;
}


int main(int argc, char** argv) {

    // line_test();
    constexpr int width  = 1920;
    constexpr int height = 1920;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
    // triangle_test(framebuffer);
    printf("Loading model...\n");
    Model model(OBJ_PATH);
    printf("Drawing model...\n");
    draw_model(model, framebuffer, zbuffer);

    int az = 0, bz = 128, cz = 255;
    // draw_triangle({  7, 45, az},{ 35, 100, bz},{ 45,  60, cz}, red, framebuffer);
    // draw_triangle({120, 35, az},{ 90,   5, bz},{ 45, 110, cz}, white, framebuffer);
    // draw_triangle({115, 83, az},{ 80,  90, bz},{ 85, 120, cz}, green, framebuffer);
    // draw_triangle_color_interpolate({  7, 45, az},{ 35, 100, bz},{ 45,  60, cz}, red, blue, green, framebuffer);
    // draw_triangle_color_interpolate({120, 35, az},{ 90,   5, bz},{ 45, 110, cz}, white, yellow, green, framebuffer);
    // draw_triangle_color_interpolate({115, 83, az},{ 80,  90, bz},{ 85, 120, cz}, blue, blue, white, framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}

