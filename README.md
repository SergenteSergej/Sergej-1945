# Sergej-1945

Description

Project 1945 is a 2D arcade-style bullet hell game, inspired by the classic 1945-style shoot 'em up games. The player controls a fighter plane and battles against waves of enemy planes and their bullets. The objective is to avoid enemy bullets, destroy enemy planes, and survive as long as possible while keeping track of health and lives.

---Features---

Player Movement: Move the plane in all directions using arrow keys.

Player Shooting: Shoot bullets with the spacebar. Bullets are deactivated after a certain period.

Enemies: The game features enemies that spawn from the top of the screen. Each enemy can shoot bullets towards the player.

Collision Detection: Bullets can collide with enemies and other bullets. The player loses health when hit by an enemy bullet or collides with an enemy plane.

Health & Lives: The player has a health bar that decays over time. When health reaches zero, the player loses a life.

Score: Points are gained by destroying enemies. The score is displayed at the bottom of the screen.

Game Over: The game ends when the player runs out of lives.

---Controls---

Arrow Keys: Move the plane.

Spacebar: Shoot bullets.

---Compile the game using a C compiler---

gcc src/*.c -I include -L lib -lraylib -lwinmm -lgdi32 -lopengl32 -luser32 -lshell32 -lole32 -mconsole -o bin/project1945.exe

---Execute---

.\bin\project1945.exe

![20251205-1132-50 5573223](https://github.com/user-attachments/assets/bd817ce9-650b-435c-928d-9e639a03412e)
