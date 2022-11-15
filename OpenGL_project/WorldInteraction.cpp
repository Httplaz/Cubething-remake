#include "WorldInteraction.h"

glm::ivec3 WorldInteraction::raycast(glm::vec3 pos, glm::vec3 dir, ChunkHolder& holder)
{
    vec3 border1 = holder.getSize();

    while (pos.x >= 0 && pos.y >= 0-200 && pos.z >= 0 && pos.x < border1.x && pos.y < border1.y+200 && pos.z < border1.z)
    {
        vec3 sdir = sign(dir);
        vec3 dist = (vec3(1., 1., 1.) - fract(pos)) * (sdir + vec3(1.)) / 2.f + fract(pos) * (sdir - vec3(1.)) / -2.f + 1e-4f;
        vec3 prior = dist / abs(dir);
        float m = std::min(prior.x, std::min(prior.y, prior.z));
        pos += dir * m;
        vec3 cube = floor(pos);
        uint32_t c = holder.getBlock(cube);
        if (c > 0)
            return cube;
    }
    return glm::ivec3(-1);

}
