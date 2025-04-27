// filepath: /home/wind/Projects/SourrondView360/src/common/common.h
// 或者 model_loader.h
#include "mesh.h" // 需要 Vertex 结构
#include <string>
#include <vector>

// 函数：从 OBJ 文件加载模型数据
bool loadModel(const std::string &path, std::vector<Vertex> &outVertices,
               std::vector<unsigned int> &outIndices);