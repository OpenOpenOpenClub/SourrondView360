#include "model_loader.h"
#include <iostream>
#include <tinyobjloader/tiny_obj_loader.h>
#include <unordered_map> // 用于优化顶点去重

// Helper function for vertex uniqueness (optional but recommended for
// performance)
namespace std {
template <> struct hash<Vertex> {
  size_t operator()(Vertex const &vertex) const {
    size_t seed = 0;
    // Simple hash combination - consider a more robust one if needed
    auto hashCombine = [&](size_t &seed, size_t hash) {
      seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };

    hashCombine(seed, hash<float>()(vertex.Position.x));
    hashCombine(seed, hash<float>()(vertex.Position.y));
    hashCombine(seed, hash<float>()(vertex.Position.z));
    hashCombine(seed, hash<float>()(vertex.Normal.x));
    hashCombine(seed, hash<float>()(vertex.Normal.y));
    hashCombine(seed, hash<float>()(vertex.Normal.z));
    hashCombine(seed, hash<float>()(vertex.TexCoords.x));
    hashCombine(seed, hash<float>()(vertex.TexCoords.y));
    return seed;
  }
};
} // namespace std

// Equality operator for Vertex needed for unordered_map
bool operator==(const Vertex &lhs, const Vertex &rhs) {
  return lhs.Position == rhs.Position && lhs.Normal == rhs.Normal &&
         lhs.TexCoords == rhs.TexCoords;
}

// ... (includes and hash function) ...

bool loadModel(const std::string &path, std::vector<Vertex> &outVertices,
               std::vector<unsigned int> &outIndices) {
  tinyobj::attrib_t attrib;
  std::vector<tinyobj::shape_t> shapes;
  std::vector<tinyobj::material_t> materials;
  // std::string warn; // <--- 移除 warn
  std::string err; // err 会包含警告和错误信息

  // tinyobj::LoadObj 会自动处理路径中的目录部分，用于查找 .mtl 文件
  std::string dir = path.substr(0, path.find_last_of("/\\") + 1);

  // --- 修改 LoadObj 调用参数 ---
  if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(),
                        dir.c_str())) {
    // err 可能包含警告和错误，即使返回 false
    if (!err.empty()) {
      std::cerr << "TinyObjLoader Error/Warning: " << err << std::endl;
    }
    return false;
  }
  // 即使加载成功，也可能有警告信息
  if (!err.empty()) {
    std::cout << "TinyObjLoader Warning: " << err << std::endl;
  }
  // --- 结束修改 ---

  std::cout << "TinyObjLoader: Successfully loaded model " << path << std::endl;
  std::cout << "  > Vertices: " << attrib.vertices.size() / 3 << std::endl;
  std::cout << "  > Normals: " << attrib.normals.size() / 3 << std::endl;
  std::cout << "  > TexCoords: " << attrib.texcoords.size() / 2 << std::endl;
  std::cout << "  > Shapes: " << shapes.size() << std::endl;
  std::cout << "  > Materials: " << materials.size() << std::endl;

  outVertices.clear();
  outIndices.clear();
  std::unordered_map<Vertex, uint32_t> uniqueVertices{}; // 用于顶点去重

  for (const auto &shape : shapes) {
    for (const auto &index : shape.mesh.indices) {
      Vertex vertex{};

      // --- 位置 ---
      vertex.Position = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};

      // --- 法线 (检查是否存在) ---
      if (index.normal_index >= 0 && !attrib.normals.empty()) {
        vertex.Normal = {attrib.normals[3 * index.normal_index + 0],
                         attrib.normals[3 * index.normal_index + 1],
                         attrib.normals[3 * index.normal_index + 2]};
      } else {
        vertex.Normal = {0.0f, 1.0f, 0.0f}; // Example: Default up
      }

      // --- 纹理坐标 (检查是否存在) ---
      if (index.texcoord_index >= 0 && !attrib.texcoords.empty()) {
        vertex.TexCoords = {attrib.texcoords[2 * index.texcoord_index + 0],
                            1.0f -
                                attrib.texcoords[2 * index.texcoord_index + 1]};
      } else {
        vertex.TexCoords = {0.0f, 0.0f}; // Default if no tex coords
      }

      // --- 顶点去重 ---
      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(outVertices.size());
        outVertices.push_back(vertex);
      }
      outIndices.push_back(uniqueVertices[vertex]);
    }
  }
  std::cout << "TinyObjLoader: Processed " << outVertices.size()
            << " unique vertices and " << outIndices.size() << " indices."
            << std::endl;

  return true;
}

// ... (rest of file) ...