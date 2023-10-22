#include "Animation.h"
#include <cstdio>

Animation::Animation()
    : frames(0)
    , currentFrameIndex(0)
    , currentFrameTime(0.f)
{
}

/* void Animation::AddFrame(Texture2D texture, int x, int y, int width, int
 * height, */
/*                          float frameTime) { */
/*   FrameData data; */
/*   data.texture = texture; */
/*   data.x = x; */
/*   data.y = y; */
/*   data.width = width; */
/*   data.height = height; */
/*   data.displayTimeSeconds = frameTime; */
/**/
/*   frames.push_back(data); */
/* } */

void Animation::AddFrame(int x, int y, int width, int height, float frameTime, std::string texturePath)
{
    FrameData data;
    data.x = x;
    data.y = y;
    data.width = width;
    data.height = height;
    data.displayTimeSeconds = frameTime;
    data.texturePath = texturePath;

    frames.push_back(data);
}

const FrameData* Animation::GetCurrentFrame() const
{
    if (frames.size() > 0) {
        return &frames[currentFrameIndex];
    }

    return nullptr;
}

bool Animation::UpdateFrame(float deltaTime)
{
    if (frames.size() > 0) {
        currentFrameTime += deltaTime;

        if (currentFrameTime >= frames[currentFrameIndex].displayTimeSeconds) {
            currentFrameTime = 0.f;
            IncrementFrame();
            return true;
        }
    }

    return false;
}

void Animation::IncrementFrame()
{
    // check if we reached the last frame
    if (currentFrameIndex == (frames.size() - 1)) {
        currentFrameIndex = 0;
    } else {
        currentFrameIndex++;
    }
}

void Animation::Reset() { currentFrameIndex = 0; }
