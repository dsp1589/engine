// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/picture_layer.h"

#include "flutter/common/threads.h"
#include "lib/ftl/logging.h"

namespace flow {

PictureLayer::PictureLayer() {}

PictureLayer::~PictureLayer() {
  // The picture may contain references to textures that are associated
  // with the IO thread's context.
  SkPicture* picture = picture_.release();
  if (picture) {
    blink::Threads::IO()->PostTask([picture]() { picture->unref(); });
  }
}

void PictureLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  if (auto cache = context->raster_cache) {
    raster_cache_result_ = cache->GetPrerolledImage(
        context->gr_context, picture_.get(), matrix, is_complex_, will_change_);
  }

  SkRect bounds = picture_->cullRect().makeOffset(offset_.x(), offset_.y());
  set_paint_bounds(bounds);
  context->child_paint_bounds = bounds;
}

void PictureLayer::Paint(PaintContext& context) {
  FTL_DCHECK(picture_);

  TRACE_EVENT0("flutter", "PictureLayer::Paint");

  SkAutoCanvasRestore save(&context.canvas, true);
  context.canvas.translate(offset_.x(), offset_.y());

  if (raster_cache_result_.is_valid()) {
    context.canvas.drawImageRect(
        raster_cache_result_.image(),             // image
        raster_cache_result_.source_rect(),       // source
        raster_cache_result_.destination_rect(),  // destination
        nullptr,                                  // paint
        SkCanvas::kStrict_SrcRectConstraint       // source constraint
        );
  } else {
    context.canvas.drawPicture(picture_.get());
  }
}

}  // namespace flow
