#pragma once

#include <string>
#include <vector>

/* struct FrameData { */
/*   Texture2D texture; */
/*   int x; */
/*   int y; */
/*   int width; */
/*   int height; */
/*   float displayTimeSeconds; */
/* }; */

struct FrameData {
    int x;
    int y;
    int width;
    int height;
    float displayTimeSeconds;
    std::string texturePath;
};

class Animation {
public:
    Animation();

    /* void AddFrame(Texture2D textureID, int x, int y, int width, int height, */
    /*               float frameTime); */

    void AddFrame(int x, int y, int width, int height, float frameTime, std::string texturePath);

    const FrameData* GetCurrentFrame() const;

    bool UpdateFrame(float deltaTime);

    void Reset();

private:
    void IncrementFrame();

    std::vector<FrameData> frames;
    int currentFrameIndex;
    float currentFrameTime;
};
