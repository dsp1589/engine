// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/flow/layers/clip_path_layer.h"

#if defined(OS_FUCHSIA)
#include "apps/mozart/lib/skia/type_converters.h" // nogncheck
#include "apps/mozart/services/composition/nodes.fidl.h" // nogncheck
#endif  // defined(OS_FUCHSIA)

namespace flow {

ClipPathLayer::ClipPathLayer() {}

ClipPathLayer::~ClipPathLayer() {}

void ClipPathLayer::Preroll(PrerollContext* context, const SkMatrix& matrix) {
  PrerollChildren(context, matrix);
  if (!context->child_paint_bounds.intersect(clip_path_.getBounds()))
    context->child_paint_bounds.setEmpty();
  set_paint_bounds(context->child_paint_bounds);
}

#if defined(OS_FUCHSIA)

void ClipPathLayer::UpdateScene(SceneUpdateContext& context,
                                mozart::Node* container) {
  auto node = mozart::Node::New();
  node->content_clip = mozart::RectF::From(clip_path_.getBounds());
  UpdateSceneChildrenInsideNode(context, container, std::move(node));
}

#endif  // defined(OS_FUCHSIA)

void ClipPathLayer::Paint(PaintContext& context) {
  TRACE_EVENT0("flutter", "ClipPathLayer::Paint");
  FTL_DCHECK(!needs_system_composite());

  Layer::AutoSaveLayer save(context, paint_bounds(), nullptr);
  context.canvas.clipPath(clip_path_, true);
  PaintChildren(context);
}

}  // namespace flow
