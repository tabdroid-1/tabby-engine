#include "Graphics/Graphics.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <cmath>

namespace Tabby {

void Graphics::DrawSprite(Matrix mat, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector3 origin, Vector2 size, Color tint)
{
    rlPushMatrix();

    // peel off just the rotation
    // Quaternion quat = QuaternionFromMatrix(mat);
    // mat = QuaternionToMatrix(quat);

    // apply just the rotation
    // rlMultMatrixf(MatrixToFloat(mat));

    // translate backwards in the inverse rotated matrix to put the item where it goes in world space
    // position = Vector3Transform(position, MatrixInvert(mat));
    rlTranslatef(position.x, position.y, position.z);
    rlTranslatef(-origin.x, -origin.y, -origin.z);
    rlRotatef(rotation.x, 1.0f, 0.0f, 0.0f);
    rlRotatef(rotation.y, 0.0f, 1.0f, 0.0f);
    rlRotatef(rotation.z, 0.0f, 0.0f, 1.0f);

    // draw the billboard
    float width = size.x / 2;
    float height = size.y / 2;

    Color color = WHITE;

    rlCheckRenderBatchLimit(16);

    rlSetTexture(texture.id);

    // draw quad
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer

    rlTexCoord2f((float)source.x / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(-width, -height, 0); // Bottom Left Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(+width, -height, 0); // Bottom Right Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)source.y / texture.height);
    rlVertex3f(+width, +height, 0); // Top Right Of The Texture and Quad

    rlTexCoord2f((float)source.x / texture.width, (float)source.y / texture.height);
    rlVertex3f(-width, +height, 0); // Top Left Of The Texture and Quad

    rlEnd();
    rlSetTexture(0);

    rlPopMatrix();
}

void Graphics::Draw3DBillboardRec(Camera camera, Texture2D texture, Rectangle source, Vector3 position, Vector2 size, Color tint)
{
    rlPushMatrix();

    // get the camera view matrix
    Matrix mat = MatrixInvert(MatrixLookAt(camera.position, camera.target, camera.up));

    std::cout << mat.m0 << "  " << mat.m1 << "  " << mat.m2 << "  " << mat.m3 << "\n"
              << mat.m4 << "  " << mat.m5 << "  " << mat.m6 << "  " << mat.m7 << " \n "
              << mat.m8 << "  " << mat.m9 << "  " << mat.m10 << "  " << mat.m11 << " \n "
              << mat.m12 << "  " << mat.m13 << "  " << mat.m14 << "  " << mat.m15 << " \n "
              << "\n";

    // peel off just the rotation
    Quaternion quat = QuaternionFromMatrix(mat);
    mat = QuaternionToMatrix(quat);

    // apply just the rotation
    rlMultMatrixf(MatrixToFloat(mat));

    // translate backwards in the inverse rotated matrix to put the item where it goes in world space
    position = Vector3Transform(position, MatrixInvert(mat));
    rlTranslatef(position.x, position.y, position.z);

    // draw the billboard
    float width = size.x / 2;
    float height = size.y / 2;

    Color color = WHITE;

    rlCheckRenderBatchLimit(6);

    rlSetTexture(texture.id);

    // draw quad
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    // Front Face
    rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer

    rlTexCoord2f((float)source.x / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(-width, -height, 0); // Bottom Left Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)(source.y + source.height) / texture.height);
    rlVertex3f(+width, -height, 0); // Bottom Right Of The Texture and Quad

    rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)source.y / texture.height);
    rlVertex3f(+width, +height, 0); // Top Right Of The Texture and Quad

    rlTexCoord2f((float)source.x / texture.width, (float)source.y / texture.height);
    rlVertex3f(-width, +height, 0); // Top Left Of The Texture and Quad

    rlEnd();
    rlSetTexture(0);
    rlPopMatrix();
}

// void Graphics::TransformCamera(Camera camera, int mode, Vector3 position, Vector3 rotation)
// {
//
//     camera.position = (Vector3) { 2.5f, 2.5f, 3.0f }; // Camera position
//
//     camera.target = (Vector3) { 0.0f, 0.0f, 0.0f }; // Camera looking at point
//
//     camera.up = (Vector3) { 0.0f, 1.0f, 0.0f }; // Camera up vector (rotation towards target)
//
//     camera.fovy = 65.0f; // Camera field-of-view Y
//
//     camera.projection = CAMERA_PERSPECTIVE; // Camera mode type
//
//     Vector3 direction;
//
//     Vector3 cameraFront;
//
//     Vector3 cameraRight;
//
//     Vector3 cameraUp;
//
//     // this is where magik happened
//
//     float pitch = -0.6; // mouseDelta y
//
//     float yaw = -2.45; // mouseDelta x
//
//     direction.x = cos(yaw) * cos(pitch);
//
//     direction.y = sin(pitch);
//
//     direction.z = sin(yaw) * cos(pitch);
//
//     cameraFront = Vector3Normalize(direction);
//
//     cameraRight = Vector3Normalize(Vector3CrossProduct(camera.up, cameraFront));
//
//     cameraUp = Vector3CrossProduct(direction, cameraRight);
//
//     camera.target = Vector3Add(camera.position, cameraFront);
//
//     // end of magik
//
//     float deltaTime = GetFrameTime();
//
//     if (IsKeyDown(KEY_W)) {
//
//         camera.position = Vector3Add(camera.position, Vector3Scale(cameraFront, deltaTime * 2.0f));
//     }
//
//     if (IsKeyDown(KEY_S)) {
//
//         camera.position = Vector3Subtract(camera.position, Vector3Scale(cameraFront, deltaTime * 2.0f));
//     }
//
//     if (IsKeyDown(KEY_A)) {
//
//         camera.position = Vector3Subtract(camera.position, Vector3Scale(Vector3CrossProduct(cameraFront, cameraUp), deltaTime * 2.0f));
//     }
//
//     if (IsKeyDown(KEY_D)) {
//
//         camera.position = Vector3Add(camera.position, Vector3Scale(Vector3CrossProduct(cameraFront, cameraUp), deltaTime * 2.0f));
//     }
//
//     if (IsKeyDown(KEY_SPACE)) {
//
//         camera.position = Vector3Add(camera.position, Vector3Scale(cameraUp, deltaTime * 2.0f));
//     }
//
//     if (IsKeyDown(KEY_LEFT_SHIFT)) {
//
//         camera.position = Vector3Subtract(camera.position, Vector3Scale(cameraUp, deltaTime * 2.0f));
//     }
//
//     if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
//
//         DisableCursor();
//
//         Vector2 mouseDelta = GetMouseDelta();
//
//         yaw += mouseDelta.x * deltaTime;
//
//         pitch += -mouseDelta.y * deltaTime;
//
//         if (pitch > 1.5) {
//
//             pitch = 1.5;
//
//         } else if (pitch < -1.5) {
//
//             pitch = -1.5;
//         }
//
//     } else {
//
//         EnableCursor();
//     }
//
//     direction.x = cos(yaw) * cos(pitch);
//
//     direction.y = sin(pitch);
//
//     direction.z = sin(yaw) * cos(pitch);
//
//     cameraFront = Vector3Normalize(direction);
//
//     cameraRight = Vector3Normalize(Vector3CrossProduct(camera.up, cameraFront));
//
//     cameraUp = Vector3CrossProduct(direction, cameraRight);
//
//     camera.target = Vector3Add(camera.position, cameraFront);
// }
}

// void Graphics::DrawSprite(Matrix mat, Texture2D texture, Rectangle source, Vector3 position, Vector3 rotation, Vector2 size, Color tint)
// {
//     rlPushMatrix();
//
//     // peel off just the rotation
//     Quaternion quat = QuaternionFromMatrix(mat);
//     mat = QuaternionToMatrix(quat);
//
//     // apply just the rotation
//     rlMultMatrixf(MatrixToFloat(mat));
//
//     // translate backwards in the inverse rotated matrix to put the item where it goes in world space
//     position = Vector3Transform(position, MatrixInvert(mat));
//     rlTranslatef(position.x, position.y, position.z);
//
//     // draw the billboard
//     float width = size.x / 2;
//     float height = size.y / 2;
//
//     Color color = WHITE;
//
//     rlCheckRenderBatchLimit(6);
//
//     rlSetTexture(texture.id);
//
//     // draw quad
//     rlBegin(RL_QUADS);
//     rlColor4ub(tint.r, tint.g, tint.b, tint.a);
//     // Front Face
//     rlNormal3f(0.0f, 0.0f, 1.0f); // Normal Pointing Towards Viewer
//
//     rlTexCoord2f((float)source.x / texture.width, (float)(source.y + source.height) / texture.height);
//     rlVertex3f(-width, -height, 0); // Bottom Left Of The Texture and Quad
//
//     rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)(source.y + source.height) / texture.height);
//     rlVertex3f(+width, -height, 0); // Bottom Right Of The Texture and Quad
//
//     rlTexCoord2f((float)(source.x + source.width) / texture.width, (float)source.y / texture.height);
//     rlVertex3f(+width, +height, 0); // Top Right Of The Texture and Quad
//
//     rlTexCoord2f((float)source.x / texture.width, (float)source.y / texture.height);
//     rlVertex3f(-width, +height, 0); // Top Left Of The Texture and Quad
//
//     rlEnd();
//     rlSetTexture(0);
//
//     rlPopMatrix();
// }
