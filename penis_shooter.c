#include "raylib.h"
#include "stdio.h"

#if defined (PLATFORM_DESKTOP)
  #define GLSL_VERSION 330
#else 
  #define GLSL_VERSION 100
#endif

#define MAX_POSTPRO_SHADERS 12



int main(void) {
  SetConfigFlags(FLAG_MSAA_4X_HINT);
  const int SCREEN_WIDTH = 800;
  const int SCREEN_HEIGHT = 800;
  
  // init camera
  Camera camera = { 0 };
  camera.position = (Vector3) {0.2f, 0.4f, 0.2f};
  camera.target = (Vector3) {0.185f, 0.4f, 0.0f};
  camera.up = (Vector3) {0.0f, 1.0f, 0.0f};
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;
  Vector3 position = {0.0f, 0.0f, 0.0f};
  

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "penis_shooter"); 
   
  // loading resources and shit
  
  // cubemap
  Image imMap = LoadImage("img/map2.png");
  Texture2D cubicmap = LoadTextureFromImage(imMap);
  Mesh mesh = GenMeshCubicmap(imMap, (Vector3) {1.0f, 5.0f, 1.0f});
  Model model = LoadModelFromMesh(mesh);
  
  // GUN
  Image gun = LoadImage("img/fps_gun2.png");
  Texture2D gun_tex = LoadTextureFromImage(gun);
  BlendMode blendMode = 0; 
  

  Image muzz_im = LoadImage("img/muzzle_flash.png");
  Texture2D muzz_tex = LoadTextureFromImage(muzz_im);

  // load shader
  Shader cross_hatch = LoadShader(0, TextFormat("shaders/cross_hatch.glsl", GLSL_VERSION));

  RenderTexture2D  target = LoadRenderTexture(SCREEN_WIDTH,SCREEN_HEIGHT);

  Texture2D texture = LoadTexture("img/cubicmap.png");
  // cube material
  model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

  Color *mapPixels = LoadImageColors(imMap);
  UnloadImage(imMap);
  
  Vector3 mapPosition = {-16.0f, 0.0f, -8.0f};

  DisableCursor();
  
  SetTargetFPS(60);
  
  bool shade = true;


  while(!WindowShouldClose()) {
    Vector3 oldCamPos = camera.position;
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
    
    Vector2 playerPos = { camera.position.x, camera.position.z };
    float playerRadius = 0.1f;
    
    int playerCellX = (int)(playerPos.x - mapPosition.x + 0.5f);
    int playerCellY = (int)(playerPos.y - mapPosition.z + 0.5f);

    if(playerCellX < 0) {
      playerCellX = 0;
    }else if (playerCellX >= cubicmap.width) {
      playerCellX = cubicmap.width - 1;
    }

    if(playerCellY < 0) {
      playerCellY = 0;
    } else if (playerCellY >= cubicmap.height) {
      playerCellY = cubicmap.height - 1;
    }

    
    for (int y = 0; y < cubicmap.height; y++)
    {
        for (int x = 0; x < cubicmap.width; x++)
        {
            if ((mapPixels[y*cubicmap.width + x].r == 255) &&       // Collision: white pixel, only check R channel
                (CheckCollisionCircleRec(playerPos, playerRadius,
                (Rectangle){ mapPosition.x - 0.5f + x*1.0f, mapPosition.z - 0.5f + y*1.0f, 1.0f, 1.0f })))
            {
                // Collision detected, reset camera position
                camera.position = oldCamPos;
            }
        }
    }
    
    if (IsKeyPressed(KEY_T) && shade==true){
        shade = false;
    } else if (IsKeyPressed(KEY_T) && shade==false){
      shade = true;
    }
    if (shade == true) {

         BeginTextureMode(target);
         ClearBackground(RAYWHITE);
             BeginMode3D(camera);
               DrawModel(model, mapPosition, 1.0f, WHITE);
             EndMode3D();
             // fire_gun(muzz_tex, (Vector2) {(float) SCREEN_WIDTH/2 , (float) SCREEN_HEIGHT/2});
              //gun image
            
            BeginBlendMode(blendMode);
                  // DrawTextureEx(gun_tex, (Vector2) {SCREEN_WIDTH/1.4 - gun.width/2, SCREEN_HEIGHT/2 - gun.height/2},0.0f, 1.5f , WHITE);
                  if (IsKeyPressed(KEY_SPACE)) {
                      DrawTexture(muzz_tex, SCREEN_HEIGHT/2 - muzz_im.width/2 , SCREEN_WIDTH/2 - muzz_im.width/2, WHITE);  
                  }
                  DrawTexture(gun_tex, SCREEN_WIDTH/2 - gun.width/2, SCREEN_HEIGHT/2 - gun.height/2, WHITE);
            EndBlendMode();
         EndTextureMode();

         BeginDrawing();
         ClearBackground(RAYWHITE);
              DrawTextureEx(cubicmap, (Vector2){ GetScreenWidth() - cubicmap.width*4.0f - 20, 20.0f }, 0.0f, 4.0f, WHITE);
              DrawRectangleLines(GetScreenWidth() - cubicmap.width*4 - 20, 20, cubicmap.width*4, cubicmap.height*4, GREEN);
              // Draw player position radar
              DrawRectangle(GetScreenWidth() - cubicmap.width*4 - 20 + playerCellX*4, 20 + playerCellY*4, 4, 4, RED);
              DrawFPS(10, 10);

          // apply shader ayylmao 
          BeginShaderMode(cross_hatch);
              DrawTextureRec(target.texture, (Rectangle) {0,0,(float) target.texture.width, (float)-target.texture.height}, (Vector2) {0,0}, WHITE);
          EndShaderMode();

          // draw shit on top
          DrawText(TextFormat("+"), 400, 400, 20, BLACK);
      EndDrawing();
    } else {
        BeginDrawing();
        ClearBackground(RAYWHITE);
             BeginMode3D(camera);
               DrawModel(model, mapPosition, 1.0f, WHITE);
             EndMode3D();
              //gun image
              BeginBlendMode(blendMode);
                  // DrawTextureEx(gun_tex, (Vector2) {SCREEN_WIDTH/1.4 - gun.width/2, SCREEN_HEIGHT/2 - gun.height/2},0.0f, 1.5f , WHITE);
                  if (IsKeyPressed(KEY_SPACE)) {
                      DrawTexture(muzz_tex, SCREEN_HEIGHT/2 - muzz_im.width/2 , SCREEN_WIDTH/2 - muzz_im.width/2, WHITE);  
                  }
                  DrawTexture(gun_tex, SCREEN_WIDTH/2 - gun.width/2, SCREEN_HEIGHT/2 - gun.height/2, WHITE);
              EndBlendMode();
        EndDrawing();
    }
  
  }

   UnloadRenderTexture(target);
   UnloadImageColors(mapPixels);
   UnloadTexture(texture); 
   UnloadTexture(cubicmap);
   UnloadModel(model);
   UnloadTexture(gun_tex); 
   CloseWindow();
   return 0;

}
