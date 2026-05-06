#pragma once
#include <cstdint>

// 基础坐标结构
struct Point { 
    int x, y; 
};

// 算法输入视图 (2D栅格专用)
struct GridView {
    int W, H;
    const uint8_t* blocked; 
};