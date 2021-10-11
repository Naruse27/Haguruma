#include "Blender.h"

void Blender::SetBlendMode(Blender::BLEND_STATE blend, ID3D11DeviceContext* context)
{
    context->OMSetBlendState(this->states[blend].Get(), nullptr, 0xFFFFFFFF);
    bs = blend;
}