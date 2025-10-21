#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <utility>
#include "tgaimage.h"
#include "model.h"

#define OBJ_PATH "../obj/diablo3_pose/diablo3_pose.obj"

using std::pair;

constexpr TGAColor white   = {{255, 255, 255, 255}}; // attention, BGRA order
constexpr TGAColor green   = {{  0, 255,   0, 255}};
constexpr TGAColor red     = {{  0,   0, 255, 255}};
constexpr TGAColor blue    = {{255, 128,  64, 255}};
constexpr TGAColor yellow  = {{  0, 200, 255, 255}};

void line(pair<int,int> A, pair<int,int> B, TGAColor color,TGAImage &framebuffer){
    // int x0 = A.first;
    // int y0 = A.second;
    // int x1 = B.first;
    // int y1 = B.second;

    // for(float t = 0.0; t <= 1.0; t += 0.02){
    //     int x = std::round(x0 + (x1 - x0) * t);
    //     int y = std::round(y0 + (y1 - y0) * t);
    //     framebuffer.set(x, y, color);
    // }

    bool steep = false;

    if(std::abs(B.second - A.second) > std::abs(B.first - A.first)){
        std::swap(A.first, A.second);
        std::swap(B.first, B.second);
        steep = true;
    }

    if (B < A) std::swap(A, B);
    int dx = B.first - A.first;
    if(dx == 0) return; // A and B are the same point
    float k = (B.second - A.second) / static_cast<float>(dx);
    float y = static_cast<int>(A.second);
    for (int x = A.first; x <= B.first; ++x) {
        if (!steep) framebuffer.set(x, y, color);
        else        framebuffer.set(y, x, color);
        y += k;
    }
}

void draw_model(Model &model, TGAImage &framebuffer){
    for(int i = 0; i < model.get_num_faces(); i++){
        vec3<vec3<int>> face = model.face(i);
        vec3<float> v0 = model.vert(face[0][0]);
        vec3<float> v1 = model.vert(face[1][0]);
        vec3<float> v2 = model.vert(face[2][0]);

        int x0 = static_cast<int>((v0.x + 1.0f) * framebuffer.width()  / 2.0f);
        int y0 = static_cast<int>((v0.y + 1.0f) * framebuffer.height() / 2.0f);
        int x1 = static_cast<int>((v1.x + 1.0f) * framebuffer.width()  / 2.0f);
        int y1 = static_cast<int>((v1.y + 1.0f) * framebuffer.height() / 2.0f);
        int x2 = static_cast<int>((v2.x + 1.0f) * framebuffer.width()  / 2.0f);
        int y2 = static_cast<int>((v2.y + 1.0f) * framebuffer.height() / 2.0f);

        line({x0, y0}, {x1, y1}, red, framebuffer);
        line({x1, y1}, {x2, y2}, red, framebuffer);
        line({x2, y2}, {x0, y0}, red, framebuffer);

        framebuffer.set(x0, y0, white);
        framebuffer.set(x1, y1, white);
        framebuffer.set(x2, y2, white);
    }
    return;
}

// void line_test(){

// printf("Hello, TinyRenderer!\n");
// printf("C++20 is enabled: %d\n", __cplusplus >= 202002L);
// printf("sizeof TGAColor: %zu bytes\n", sizeof(TGAColor));
// printf("sizeof TGAImage: %zu bytes\n", sizeof(TGAImage));
// printf("Line drawing test...\n");
//     constexpr int width  = 64;
//     constexpr int height = 64;
//     TGAImage framebuffer(width, height, TGAImage::RGB);

//     std::srand(std::time({}));
//     for (int i=0; i<(1<<24); i++) {
//         int ax = rand()%width, ay = rand()%height;
//         int bx = rand()%width, by = rand()%height;
//         line({ax, ay}, {bx, by}, { static_cast<unsigned char>(rand()%256), static_cast<unsigned char>(rand()%256), static_cast<unsigned char>(rand()%256), static_cast<unsigned char>(rand()%256) }, framebuffer);
//     }

//     framebuffer.write_tga_file("framebuffer.tga");
//     return;
// }

int main(int argc, char** argv) {

    // line_test();
    constexpr int width  = 2160;
    constexpr int height = 2160;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    printf("Loading model...\n");
    Model model(OBJ_PATH);
    printf("Drawing model...\n");
    draw_model(model, framebuffer);

    // int ax =  30, ay =  3;
    // int bx = 35, by = 47;
    // int cx = 42, cy = 6;

    // framebuffer.set(ax, ay, white);
    // framebuffer.set(bx, by, white);
    // framebuffer.set(cx, cy, white);

    // line({ax, ay}, {bx, by}, blue, framebuffer);
    // line({cx, cy}, {bx, by}, green, framebuffer);
    // line({cx, cy}, {ax, ay}, yellow, framebuffer);
    // line({ax, ay}, {cx, cy}, red, framebuffer);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

