// main.c - Project 1945 (fixed bullet collision with proper deactivation)
#include "raylib.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define MAP_TILE_SIZE 32
#define BULLET_LIFETIME 3
#define MAX_PLAYER_BULLETS 10
#define MAX_ENEMIES 10
#define MAX_ENEMY_BULLETS 10
#define MAX_LIVES 3  // Set the maximum number of lives

typedef struct Map {
    unsigned int tilesX;
    unsigned int tilesY;
} Map;

typedef struct Player {
    Texture2D planeTex;
    Vector2 planePos;
    Rectangle planeFrameRec;
    Rectangle pCollider;
    int playerSpeed;
    int framesSpeed;
    int currentFrame;
    int framesCounter;
} Player;

typedef struct pBullet {
    Rectangle pBulRect;
    Rectangle collider;
    Texture2D pBulTex;
    Vector2 pBulPos;
    bool isActive;
} pBullet;

typedef struct Enemy {
    Texture2D enemyTex;
    Vector2 enemyPos;
    Rectangle enemyFrameRec;
    Rectangle collider;
    int enemySpeed;
    int framesSpeed;
    int enemyCurrentFrame;
    int enemyframesCounter;
    bool shootAvaible;
} Enemy;

typedef struct EnemyBullet {
    Texture2D enemyBulletTex;
    Vector2 enemyBulletPos;
    Rectangle enemyBulletFrameRec;
    Rectangle collider;
    bool isActive;
} EnemyBullet;

int main(void)
{
    const int screenWidth = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "1945");
    SetTargetFPS(60);

    // ----------------------
    // PLAYER
    // ----------------------
    Player player = {0};

    Texture2D planeTex = LoadTexture("resources/player/myplane_strip3.png");
    Rectangle planeFrameRec = { 0, 0, planeTex.width / 3.0f, planeTex.height };
    Vector2 planePos = { screenWidth/2 - planeFrameRec.width/2, screenHeight/2 - planeFrameRec.height/2 };
    Rectangle pCollider = { planePos.x, planePos.y, planeFrameRec.width, planeFrameRec.height };

    player.planeTex = planeTex;
    player.planePos = planePos;
    player.planeFrameRec = planeFrameRec;
    player.pCollider = pCollider;
    player.playerSpeed = 5;
    player.framesSpeed = 20;
    player.framesCounter = 0;
    player.currentFrame = 0;

    // ----------------------
    // PLAYER BULLETS
    // ----------------------
    Texture2D pBulTex = LoadTexture("resources/player/bullet.png");

    pBullet p_bullet[MAX_PLAYER_BULLETS];
    float pBulletSpeed = 7.0f;
    int shootCoolDownTimer = 60;

    Rectangle pBulRect = { 0, 0, pBulTex.width, pBulTex.height };

    for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
    {
        p_bullet[i].pBulRect = pBulRect;
        p_bullet[i].pBulPos = (Vector2){ -1000, -1000 };
        p_bullet[i].collider = pBulRect;
        p_bullet[i].pBulTex = pBulTex;
        p_bullet[i].isActive = false;
    }

    // ----------------------
    // ENEMIES
    // ----------------------
    Texture2D enemyTex = LoadTexture("resources/enemy/enemy1_strip3.png");

    Rectangle enemyFrameRecProto = { 0, 0, enemyTex.width/3.0f, enemyTex.height };
    float enemySpeed = 3.0f;

    Enemy enemy[MAX_ENEMIES];
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        enemy[i].enemyTex = enemyTex;
        enemy[i].enemyFrameRec = enemyFrameRecProto;
        enemy[i].enemyPos.x = GetRandomValue(0, screenWidth - (int)enemyFrameRecProto.width);
        enemy[i].enemyPos.y = GetRandomValue(-1000, -30); // Respawn above the screen
        enemy[i].collider = enemyFrameRecProto;
        enemy[i].enemySpeed = enemySpeed;
        enemy[i].enemyCurrentFrame = 0;
        enemy[i].enemyframesCounter = 0;
        enemy[i].shootAvaible = true;
    }

    // ----------------------
    // ENEMY BULLETS
    // ----------------------
    Texture2D enemyBulletTex = LoadTexture("resources/enemy/enemybullet1.png");
    Rectangle enemyBulRec = { 0, 0, enemyBulletTex.width, enemyBulletTex.height };

    EnemyBullet e_bullet[MAX_ENEMY_BULLETS];
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    {
        e_bullet[i].enemyBulletFrameRec = enemyBulRec;
        e_bullet[i].enemyBulletPos = (Vector2){ -1000, -1000 };
        e_bullet[i].collider = enemyBulRec;
        e_bullet[i].enemyBulletTex = enemyBulletTex;
        e_bullet[i].isActive = false;
    }

    float enemyBulletSpeed = 5.0f;
    int enemyShootTimer = 0;

    // ----------------------
    // MAP & HUD
    // ----------------------
    Texture2D hudBar = LoadTexture("resources/ui/bottom.png");
    int score = 0;
    float life = 100.0f;
    float maxLife = 100.0f;
    float lifeDecay = 0.05f;   // speed draining life

    int lives = MAX_LIVES;  // Set the initial number of lives
    float respawnTime = 0.0f;  // Timer to delay health refill after losing a life

    Map map = { MAP_TILE_SIZE, MAP_TILE_SIZE };
    Texture2D seaTiles = LoadTexture("resources/map/water.png");

    // ----------------------
    // AUDIO
    // ----------------------
    InitAudioDevice();

    Sound sound = LoadSound("resources/audio/shoot.wav");  // OPTIONAL
    Music music = LoadMusicStream("resources/audio/music.ogg"); // OPTIONAL

    if (music.stream.buffer) {
        music.looping = true;
        PlayMusicStream(music);
    }

    // Load the texture for the life (heart representing a life)
    Texture2D lifeTexture = LoadTexture("resources/ui/life.png");

    // ----------------------
    // GAME OVER SCREEN
    // ----------------------
    bool gameOver = false;

    char title[128];

    while (!WindowShouldClose())
    {
        if (gameOver) {
            BeginDrawing();
            ClearBackground(BLACK);

            int textWidth = MeasureText("GAME OVER", 40);

            DrawText("GAME OVER", (screenWidth - textWidth) / 2, screenHeight / 2 - 20, 40, RED);
    
            EndDrawing();
            continue; // Skip the rest of the game loop
        }

        // ----------------------
        // UPDATE
        // ----------------------
        

        player.framesCounter++;
        if (player.framesCounter >= (60 / player.framesSpeed))
        {
            player.framesCounter = 0;
            player.currentFrame++;
            if (player.currentFrame > 2) player.currentFrame = 0;
            player.planeFrameRec.x = player.currentFrame * player.planeFrameRec.width;
        }

        // Player movement
        if (IsKeyDown(KEY_RIGHT)) player.planePos.x += player.playerSpeed;
        if (IsKeyDown(KEY_LEFT))  player.planePos.x -= player.playerSpeed;
        if (IsKeyDown(KEY_UP))    player.planePos.y -= player.playerSpeed;
        if (IsKeyDown(KEY_DOWN))  player.planePos.y += player.playerSpeed;

        // PLAYER BORDER COLLISION
        if (player.planePos.x < 0)
            player.planePos.x = 0;

        if (player.planePos.x + player.planeFrameRec.width > screenWidth)
            player.planePos.x = screenWidth - player.planeFrameRec.width;

        if (player.planePos.y < 0)
            player.planePos.y = 0;

        if (player.planePos.y + player.planeFrameRec.height > screenHeight - hudBar.height)
            player.planePos.y = screenHeight - hudBar.height - player.planeFrameRec.height;

        player.pCollider.x = player.planePos.x;
        player.pCollider.y = player.planePos.y;
        player.pCollider.width = player.planeFrameRec.width;
        player.pCollider.height = player.planeFrameRec.height;

        // Player shooting
        shootCoolDownTimer++;
        for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
        {
            if (IsKeyDown(KEY_SPACE) && shootCoolDownTimer > 5 && !p_bullet[i].isActive)
            {
                p_bullet[i].pBulPos.x = player.planePos.x + player.planeFrameRec.width/2 - pBulRect.width/2;
                p_bullet[i].pBulPos.y = player.planePos.y;
                p_bullet[i].isActive = true;

                PlaySound(sound);

                shootCoolDownTimer = 0;
                break;
            }
        }

        // Update player bullets
        for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
        {
            if (p_bullet[i].isActive)
            {
                p_bullet[i].pBulPos.y -= pBulletSpeed;
                if (p_bullet[i].pBulPos.y < -50)
                {
                    p_bullet[i].isActive = false;
                    p_bullet[i].pBulPos = (Vector2){ -1000, -1000 };
                }
            }
        }

        // LIFE DECAY OVER TIME
        life -= lifeDecay;
        if (life < 0) life = 0;

        // Update enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            enemy[i].enemyPos.y += enemy[i].enemySpeed;
            enemy[i].collider.x = enemy[i].enemyPos.x;
            enemy[i].collider.y = enemy[i].enemyPos.y;

            if (enemy[i].enemyPos.y > screenHeight + 30)
            {
                // Reposition the enemy if it moves off-screen
                enemy[i].enemyPos.x = GetRandomValue(0, screenWidth - (int)enemyFrameRecProto.width);
                enemy[i].enemyPos.y = GetRandomValue(-1000, -30); // Respawn above the screen
                enemy[i].shootAvaible = true;
            }
        }

        // Enemy shooting
        enemyShootTimer++;
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (enemy[i].enemyPos.y > 0 && enemy[i].shootAvaible && enemyShootTimer > 30)
            {
                for (int b = 0; b < MAX_ENEMY_BULLETS; b++)
                {
                    if (!e_bullet[b].isActive)
                    {
                        e_bullet[b].isActive = true;
                        e_bullet[b].enemyBulletPos.x = enemy[i].enemyPos.x + enemy[i].enemyFrameRec.width/2;
                        e_bullet[b].enemyBulletPos.y = enemy[i].enemyPos.y + enemy[i].enemyFrameRec.height;

                        enemyShootTimer = 0;
                        enemy[i].shootAvaible = false;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        {
            if (e_bullet[i].isActive)
            {
                e_bullet[i].enemyBulletPos.y += enemyBulletSpeed;
            
                // ---- UPDATE COLLIDER ----
                e_bullet[i].collider.x = e_bullet[i].enemyBulletPos.x;
                e_bullet[i].collider.y = e_bullet[i].enemyBulletPos.y;
            
                if (e_bullet[i].enemyBulletPos.y > screenHeight + 50)
                {
                    e_bullet[i].isActive = false;
                    e_bullet[i].enemyBulletPos = (Vector2){ -1000, -1000 };
                }
            }
        }

        // Collision: player bullets vs enemies
        for (int b = 0; b < MAX_PLAYER_BULLETS; b++)
        {
            if (!p_bullet[b].isActive) continue;
        
            Rectangle br = { p_bullet[b].pBulPos.x, p_bullet[b].pBulPos.y,
                             pBulRect.width, pBulRect.height };
            
            for (int e = 0; e < MAX_ENEMIES; e++)
            {
                if (CheckCollisionRecs(br, enemy[e].collider))
                {
                    p_bullet[b].isActive = false;
                    p_bullet[b].pBulPos = (Vector2){ -1000, -1000 };
                
                    // Reset enemy position after being hit
                    enemy[e].enemyPos.x = GetRandomValue(0, screenWidth - (int)enemyFrameRecProto.width);
                    enemy[e].enemyPos.y = GetRandomValue(-1000, -30); // Respawn above the screen
                    enemy[e].shootAvaible = true;
                
                    score += 100;
                
                    // HEAL LIFE
                    life += 5;
                    if (life > maxLife) life = maxLife;
                }
            }
        }

        // COLLISION: ENEMY BULLETS → PLAYER
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
        {
            if (!e_bullet[i].isActive) continue;
        
            Rectangle br = {
                e_bullet[i].enemyBulletPos.x,
                e_bullet[i].enemyBulletPos.y,
                enemyBulRec.width,
                enemyBulRec.height
            };
        
            if (CheckCollisionRecs(br, player.pCollider))
            {
                e_bullet[i].isActive = false;
                e_bullet[i].enemyBulletPos = (Vector2){ -1000, -1000 };
            
                life -= 20;  // ► DAMAGE TO PLAYER
            
                if (life < 0) life = 0;
            }
        }

        // Collision: player colliding with enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
        {
            if (CheckCollisionRecs(player.pCollider, enemy[i].collider))
            {
                life -= 20;  // Player collides with enemy, lose health
                
                // Reset enemy position
                enemy[i].enemyPos.x = GetRandomValue(0, screenWidth - (int)enemyFrameRecProto.width);
                enemy[i].enemyPos.y = GetRandomValue(-1000, -30);
                enemy[i].shootAvaible = true;
                
                if (life <= 0)
                {
                    // Handle life being 0 and reducing lives
                    lives--;
                    life = maxLife;
                }
            }
        }

        // GAME OVER CHECK
        if (lives == 0) {
            gameOver = true;
        }

        // ----------------------
        // DRAW
        // ----------------------
        BeginDrawing();
        ClearBackground(BLACK);

        // Draw map
        for (int i = 0; i < map.tilesX; i++)
            for (int j = 0; j < map.tilesY; j++)
                DrawTexture(seaTiles, i * MAP_TILE_SIZE, j * MAP_TILE_SIZE, WHITE);

        // Draw bullets
        for (int i = 0; i < MAX_PLAYER_BULLETS; i++)
            if (p_bullet[i].isActive)
                DrawTexture(p_bullet[i].pBulTex, p_bullet[i].pBulPos.x, p_bullet[i].pBulPos.y, WHITE);

        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
            if (e_bullet[i].isActive)
                DrawTexture(e_bullet[i].enemyBulletTex, e_bullet[i].enemyBulletPos.x, e_bullet[i].enemyBulletPos.y, WHITE);

        // Draw enemies
        for (int i = 0; i < MAX_ENEMIES; i++)
            DrawTextureRec(enemy[i].enemyTex, enemy[i].enemyFrameRec, enemy[i].enemyPos, WHITE);

        // Draw player
        DrawTextureRec(player.planeTex, player.planeFrameRec, player.planePos, WHITE);

        // HUD base
        int hudX = 0;
        int hudY = screenHeight - hudBar.height;
        DrawTexture(hudBar, hudX, hudY, WHITE);

        // ----------------------
        // DRAW LIFE BAR
        // ----------------------
        // Calculate life percentage
        float lifePercent = life / maxLife;  // Calculate life percentage
        if (lifePercent < 0.0f) lifePercent = 0.0f;
        if (lifePercent > 1.0f) lifePercent = 1.0f;

        int fullWidth = 126;
        int drawWidth = (int)(fullWidth * lifePercent);

        if (drawWidth < 0) drawWidth = 0;
        if (drawWidth > fullWidth) drawWidth = fullWidth;

        // Draw the health bar
        Rectangle lifeRect = { hudX + 12, hudY + 45, (float)drawWidth, 10.0f };  // Position and size
        DrawRectangleRec(lifeRect, GREEN);  // Draw the life bar in green

        // ----------------------
        // DRAW LIVES
        // ----------------------
        for (int i = 0; i < lives; i++) {
            // Draw each life as a heart image on the screen
            Vector2 heartPos = { (float)(hudX + 140 + (i * 30)), (float)(hudY + 40) };
            DrawTexture(lifeTexture, heartPos.x, heartPos.y, WHITE);  // Draw each life using life.png
        }

        // ----------------------
        // SCORE
        // ----------------------
        DrawText(TextFormat("%d", score), 230, screenHeight - 67, 20, YELLOW);

        EndDrawing();
    }

    // ----------------------
    // CLEANUP
    // ----------------------
    UnloadTexture(planeTex);
    UnloadTexture(pBulTex);
    UnloadTexture(enemyTex);
    UnloadTexture(enemyBulletTex);
    UnloadTexture(hudBar);
    UnloadTexture(seaTiles);
    UnloadTexture(lifeTexture);  // Unload the life texture

    CloseWindow();

    return 0;
}
