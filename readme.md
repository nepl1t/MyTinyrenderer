## What is it?

My personal tiny renderer implementation based on [@ssloy's tinyrenderer](https://github.com/ssloy/tinyrenderer), created for learning and practice.

## Build & run

Clone the repository, then run

```shell
mkdir build && cd build
cmake -DCMAKE-BUILD-TYPE=Debug .. 
make -j{nproc} VEREBOSE = 1
./tinyrenderer
```

## To-be-dones:

1. Bresenham’s line drawing
2. Triangle rasterization
3. Barycentric coordinates
4. Hidden faces removal
5. Naive camera handling
6. Better camera
7. Shading
8. More data!
9. Tangent space
10. Shadow mapping
11. Ambient occlusion
12. Bonus: toon shading