/*
 * Copyright 2022 Rive
 */

#ifndef _RIVE_SKIA_FACTORY_HPP_
#define _RIVE_SKIA_FACTORY_HPP_

#include "rive/factory.hpp"
#include <vector>

namespace rive {

class SkiaFactory : public Factory {
    rcp<RenderBuffer> makeBufferU16(Span<const uint16_t>) override;
    rcp<RenderBuffer> makeBufferU32(Span<const uint32_t>) override;
    rcp<RenderBuffer> makeBufferF32(Span<const float>) override;

    rcp<RenderShader> makeLinearGradient(float sx, float sy,
                                         float ex, float ey,
                                         const ColorInt colors[],    // [count]
                                         const float stops[],        // [count]
                                         size_t count,
                                         RenderTileMode,
                                         const Mat2D* localMatrix = nullptr) override;

    rcp<RenderShader> makeRadialGradient(float cx, float cy, float radius,
                                         const ColorInt colors[],    // [count]
                                         const float stops[],        // [count]
                                         size_t count,
                                         RenderTileMode,
                                         const Mat2D* localMatrix = nullptr) override;

    std::unique_ptr<RenderPath> makeRenderPath(Span<const Vec2D> points,
                                               Span<const uint8_t> verbs,
                                               FillRule) override;

    std::unique_ptr<RenderPath> makeEmptyRenderPath() override;

    std::unique_ptr<RenderPaint> makeRenderPaint() override;

    std::unique_ptr<RenderImage> decodeImage(Span<const uint8_t>) override;

    //
    // New virtual for access the platform's codecs
    //

public:
    enum class ColorType {
        rgba,
        bgra,
    };
    enum class AlphaType {
        premul,
        opaque,
    };
    struct ImageInfo {
        size_t      rowBytes;   // number of bytes between rows
        uint32_t    width;      // logical width in pixels
        uint32_t    height;     // logical height in pixels
        ColorType   colorType;
        AlphaType   alphaType;
    };

    // Clients can override this to provide access to the platform's decoders, rather
    // than solely relying on the codecs built into Skia. This allows for the Skia impl
    // to not have to duplicate the code for codecs that the platform may already have.
    virtual std::vector<uint8_t> platformDecode(Span<const uint8_t>, ImageInfo* info) {
        return std::vector<uint8_t>();  // empty vector means decode failed
    }
};

} // namespace rive
#endif
