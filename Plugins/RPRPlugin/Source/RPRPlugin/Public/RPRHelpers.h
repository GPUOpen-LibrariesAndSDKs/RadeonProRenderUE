// RPR COPYRIGHT

#pragma once

RadeonProRender::matrix	BuildMatrixNoScale(const FTransform &transform, bool yUpRotation = false);
RadeonProRender::matrix	BuildMatrixWithScale(const FTransform &transform, bool yUpRotation = false);

rpr_image	BuildImage(class UTexture *source, rpr_context context);
