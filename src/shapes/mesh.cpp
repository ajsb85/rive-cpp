#include "rive/shapes/mesh.hpp"
#include "rive/shapes/image.hpp"
#include "rive/shapes/vertex.hpp"
#include "rive/shapes/mesh_vertex.hpp"
#include "rive/bones/skin.hpp"
#include "rive/artboard.hpp"
#include "rive/factory.hpp"
#include "rive/span.hpp"
#include <limits>

using namespace rive;

/// Called whenever a vertex moves (x/y change).
void Mesh::markDrawableDirty() {
    if (skin() != nullptr) {
        skin()->addDirt(ComponentDirt::Skin);
    }
    addDirt(ComponentDirt::Vertices);
}

void Mesh::addVertex(MeshVertex* vertex) { m_Vertices.push_back(vertex); }

StatusCode Mesh::onAddedDirty(CoreContext* context) {
    StatusCode result = Super::onAddedDirty(context);
    if (result != StatusCode::Ok) {
        return result;
    }

    if (!parent()->is<Image>()) {
        return StatusCode::MissingObject;
    }

    // All good, tell the image it has a mesh.
    parent()->as<Image>()->setMesh(this);

    return StatusCode::Ok;
}

StatusCode Mesh::onAddedClean(CoreContext* context) {
    // Make sure Core found indices in the file for this Mesh.
    if (m_IndexBuffer == nullptr) {
        return StatusCode::InvalidObject;
    }

    // Check the indices are all in range. We should consider having a better
    // error reporting system to the implementor.
    for (auto index : *m_IndexBuffer) {
        if (index >= m_Vertices.size()) {
            return StatusCode::InvalidObject;
        }
    }
    return Super::onAddedClean(context);
}

void Mesh::decodeTriangleIndexBytes(Span<const uint8_t> value) {
    // decode the triangle index bytes
    rcp<IndexBuffer> buffer = rcp<IndexBuffer>(new IndexBuffer());

    BinaryReader reader(value);
    while (!reader.reachedEnd()) {
        buffer->push_back(reader.readVarUintAs<uint16_t>());
    }
    m_IndexBuffer = buffer;
}

void Mesh::copyTriangleIndexBytes(const MeshBase& object) {
    m_IndexBuffer = object.as<Mesh>()->m_IndexBuffer;
}

/// Called whenever a bone moves that is connected to the skin.
void Mesh::markSkinDirty() { addDirt(ComponentDirt::Vertices); }

void Mesh::buildDependencies() {
    Super::buildDependencies();
    if (skin() != nullptr) {
        skin()->addDependent(this);
    }
    parent()->addDependent(this);

    // TODO: This logic really needs to move to a "intializeGraphics/Renderer"
    // method that is passed a reference to the Renderer.

    // TODO: if this is an instance, share the index and uv buffer from the
    // source. Consider storing an m_SourceMesh.

    std::vector<float> uv = std::vector<float>(m_Vertices.size() * 2);
    std::size_t index = 0;

    for (auto vertex : m_Vertices) {
        uv[index++] = vertex->u();
        uv[index++] = vertex->v();
    }

    auto factory = artboard()->factory();
    m_UVRenderBuffer = factory->makeBufferF32(toSpan(uv));
    m_IndexRenderBuffer = factory->makeBufferU16(toSpan(*m_IndexBuffer));
}

void Mesh::update(ComponentDirt value) {
    if (hasDirt(value, ComponentDirt::Vertices)) {
        if (skin() != nullptr) {
            skin()->deform({(Vertex**)m_Vertices.data(), m_Vertices.size()});
        }
        m_VertexRenderBuffer = nullptr;
    }
    Super::update(value);
}

void Mesh::draw(Renderer* renderer, const RenderImage* image, BlendMode blendMode, float opacity) {
    if (m_VertexRenderBuffer == nullptr) {

        std::vector<float> vertices(m_Vertices.size() * 2);
        std::size_t index = 0;
        for (auto vertex : m_Vertices) {
            auto translation = vertex->renderTranslation();
            vertices[index++] = translation.x;
            vertices[index++] = translation.y;
        }

        auto factory = artboard()->factory();
        m_VertexRenderBuffer = factory->makeBufferF32(toSpan(vertices));
    }

    if (skin() == nullptr) {
        renderer->transform(parent()->as<WorldTransformComponent>()->worldTransform());
    }
    renderer->drawImageMesh(
        image, m_VertexRenderBuffer, m_UVRenderBuffer, m_IndexRenderBuffer, blendMode, opacity);
}
