//
// Copyright 2019 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//

#include "libANGLE/renderer/metal/RenderBufferMtl.h"

#include "libANGLE/renderer/metal/ContextMtl.h"
#include "libANGLE/renderer/metal/mtl_format_utils.h"
#include "libANGLE/renderer/metal/mtl_utils.h"

namespace rx
{

RenderbufferMtl::RenderbufferMtl(const gl::RenderbufferState &state) : RenderbufferImpl(state) {}

RenderbufferMtl::~RenderbufferMtl() {}

void RenderbufferMtl::onDestroy(const gl::Context *context)
{
    releaseTexture();
}

void RenderbufferMtl::releaseTexture()
{
    mTexture = nullptr;
}

angle::Result RenderbufferMtl::setStorageImpl(const gl::Context *context,
                                              size_t samples,
                                              GLenum internalformat,
                                              size_t width,
                                              size_t height)
{
    ContextMtl *contextMtl    = mtl::GetImpl(context);
    id<MTLDevice> metalDevice = contextMtl->getMetalDevice();

    // TODO(hqle): Support MSAA
    ANGLE_CHECK(contextMtl, samples == 1, "Multisample is not supported atm.", GL_INVALID_VALUE);

    if (mTexture != nullptr && mTexture->valid())
    {
        // Check against the state if we need to recreate the storage.
        if (internalformat != mState.getFormat().info->internalFormat ||
            static_cast<GLsizei>(width) != mState.getWidth() ||
            static_cast<GLsizei>(height) != mState.getHeight())
        {
            releaseTexture();
        }
    }

    mFormat.initAndConvertToCompatibleFormatIfNotSupported(metalDevice, internalformat);

    if ((mTexture == nullptr || !mTexture->valid()) && (width != 0 && height != 0))
    {
        ANGLE_TRY(mtl::Texture::Make2DTexture(contextMtl, mFormat.metalFormat, width, height, 1,
                                              true, &mTexture));

        mRenderTarget.set(mTexture, 0, 0, mFormat);
    }

    return angle::Result::Continue;
}

angle::Result RenderbufferMtl::setStorage(const gl::Context *context,
                                          GLenum internalformat,
                                          size_t width,
                                          size_t height)
{
    return setStorageImpl(context, 1, internalformat, width, height);
}

angle::Result RenderbufferMtl::setStorageMultisample(const gl::Context *context,
                                                     size_t samples,
                                                     GLenum internalformat,
                                                     size_t width,
                                                     size_t height)
{
    // TODO(hqle): Support MSAA
    UNIMPLEMENTED();
    return angle::Result::Stop;
}

angle::Result RenderbufferMtl::setStorageEGLImageTarget(const gl::Context *context,
                                                        egl::Image *image)
{
    // TODO(hqle): Support EGLimage
    UNIMPLEMENTED();
    return angle::Result::Stop;
}

angle::Result RenderbufferMtl::getAttachmentRenderTarget(const gl::Context *context,
                                                         GLenum binding,
                                                         const gl::ImageIndex &imageIndex,
                                                         FramebufferAttachmentRenderTarget **rtOut)
{
    ASSERT(mTexture && mTexture->valid());
    *rtOut = &mRenderTarget;
    return angle::Result::Continue;
}

angle::Result RenderbufferMtl::initializeContents(const gl::Context *context,
                                                  const gl::ImageIndex &imageIndex)
{
    return mtl::InitializeTextureContents(context, mTexture, mFormat, imageIndex);
}
}