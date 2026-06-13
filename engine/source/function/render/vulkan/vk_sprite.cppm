export module crumb.render.vulkan:sprite;

import :context;
import :pipeline;
import :texture;
import crumb.core;
import vk_mem_alloc;
import vulkan;
import std;

export namespace Crumb::Render {

struct SpriteVertex
{
    Vector2 position;
    Vector2 uv;
};

// CPU 端精灵数据，每帧 memcpy 到 SSBO
struct SpriteData
{
    Matrix4x4 mvp;
    std::uint32_t textureId;
};

class VKSpriteRenderer
{
  public:
    VKSpriteRenderer(VKContext &context, VKPipeline &pipeline, std::uint32_t maxSprites);

    bool Initialize();
    bool BindSprite(std::uint32_t index, const VKTexture &texture);
    void SetScreenSize(float width, float height);
    void UpdateSprite(std::uint32_t index, const Matrix4x4 &model);
    void Draw(vk::CommandBuffer cmd);

  private:
    VKContext &m_context;
    VKPipeline &m_pipeline;
    std::uint32_t m_maxSprites;

    // 共享四边形网格
    vma::raii::Buffer m_vertexBuffer = nullptr;
    vma::raii::Buffer m_indexBuffer = nullptr;

    // SSBO — 所有精灵数据，持久映射
    vma::raii::Buffer m_ssbo = nullptr;
    void* m_ssboMapped = nullptr;

    // 共享描述符池 + 一个集
    vk::raii::DescriptorPool m_descriptorPool = nullptr;
    vk::raii::DescriptorSet m_sharedSet = nullptr;

    // CPU 端精灵数据 + 纹理注册
    std::vector<SpriteData> m_spriteData;
    std::vector<const VKTexture*> m_textures;
    std::vector<vk::DescriptorImageInfo> m_texDescriptors;

    // 正交投影
    Matrix4x4 m_orthoProj;

    // 活跃精灵数
    std::uint32_t m_activeCount = 0;

    bool CreateQuadMesh();
    bool CreateDescriptorPool();
    bool CreateSSBO();
    void RegisterTexture(std::uint32_t slot, const VKTexture& tex);
};

} // namespace Crumb::Render
