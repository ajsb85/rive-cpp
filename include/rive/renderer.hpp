/*
 * Copyright 2022 Rive
 */

#ifndef _RIVE_RENDERER_HPP_
#define _RIVE_RENDERER_HPP_

#include "rive/shapes/paint/color.hpp"
#include "rive/command_path.hpp"
#include "rive/layout.hpp"
#include "rive/refcnt.hpp"
#include "rive/span.hpp"
#include "rive/math/aabb.hpp"
#include "rive/math/mat2d.hpp"
#include "rive/shapes/paint/blend_mode.hpp"
#include "rive/shapes/paint/stroke_cap.hpp"
#include "rive/shapes/paint/stroke_join.hpp"

#include <cmath>
#include <stdio.h>
#include <cstdint>

namespace rive {
    class Vec2D;

    // Helper that computes a matrix to "align" content (source) to fit inside frame (destination).
    Mat2D computeAlignment(Fit, Alignment, const AABB& frame, const AABB& content);

    // A render buffer holds an immutable array of values
    class RenderBuffer : public RefCnt {
        const size_t m_Count;

    public:
        RenderBuffer(size_t count);
        ~RenderBuffer() override;

        size_t count() const { return m_Count; }
    };

    enum class RenderPaintStyle { stroke, fill };

    enum class RenderTileMode {
        clamp,
        repeat,
        mirror,
        decal, // fill outside the domain with transparent
    };

    /*
     *  Base class for Render objects that specify the src colors.
     *
     *  Shaders are immutable, and sharable between multiple paints, etc.
     *
     *  It is common that a shader may be created with a 'localMatrix'. If this is
     *  not null, then it is applied to the shader's domain before the Renderer's CTM.
     */
    class RenderShader : public RefCnt {
    public:
        RenderShader();
        ~RenderShader() override;
    };

    class RenderPaint {
    public:
        RenderPaint();
        virtual ~RenderPaint();
    
        virtual void style(RenderPaintStyle style) = 0;
        virtual void color(ColorInt value) = 0;
        virtual void thickness(float value) = 0;
        virtual void join(StrokeJoin value) = 0;
        virtual void cap(StrokeCap value) = 0;
        virtual void blendMode(BlendMode value) = 0;
        virtual void shader(rcp<RenderShader>) = 0;
    };

    class RenderImage {
    protected:
        int m_Width = 0;
        int m_Height = 0;

    public:
        RenderImage();
        virtual ~RenderImage();

        int width() const { return m_Width; }
        int height() const { return m_Height; }

        virtual rcp<RenderShader> makeShader(RenderTileMode tx,
                                             RenderTileMode ty,
                                             const Mat2D* localMatrix = nullptr) const = 0;
    };

    class RenderPath : public CommandPath {
    public:
        RenderPath();
        ~RenderPath() override;

        RenderPath* renderPath() override { return this; }
        void addPath(CommandPath* path, const Mat2D& transform) override {
            addRenderPath(path->renderPath(), transform);
        }

        virtual void addRenderPath(RenderPath* path, const Mat2D& transform) = 0;
    };

    class Renderer {
    public:
        virtual ~Renderer() {}
        virtual void save() = 0;
        virtual void restore() = 0;
        virtual void transform(const Mat2D& transform) = 0;
        virtual void drawPath(RenderPath* path, RenderPaint* paint) = 0;
        virtual void clipPath(RenderPath* path) = 0;
        virtual void drawImage(const RenderImage*, BlendMode, float opacity) = 0;
        virtual void drawImageMesh(const RenderImage*,
                                   rcp<RenderBuffer> vertices_f32,
                                   rcp<RenderBuffer> uvCoords_f32,
                                   rcp<RenderBuffer> indices_u16,
                                   BlendMode,
                                   float opacity) = 0;

        // helpers

        void translate(float x, float y);
        void scale(float sx, float sy);
        void rotate(float radians);

        void align(Fit fit, Alignment alignment, const AABB& frame, const AABB& content) {
            transform(computeAlignment(fit, alignment, frame, content));
        }
    };
} // namespace rive
#endif
