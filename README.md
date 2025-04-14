# Legendary Retro Shooter 🔫👾

![retro shooter](https://github.com/LuDeVing/Retro-Shooter/blob/master/retro%20shooter.gif)

Welcome to **Legendary Retro Shooter**! This project is a tribute to the classic first-person shooters of the 90s, built from the ground up using C++ and the fantastic **olcPixelGameEngine**. Prepare to dive into pixelated corridors, blast uniquely themed enemies across multiple levels, and experience the satisfying challenge of old-school FPS gameplay.

## ✨ Features Deep Dive

* **Classic Raycasting Engine:** Experience authentic retro visuals powered by a raycasting engine. This technique simulates a 3D perspective by casting rays from the camera for each column of pixels, creating the distinct look and feel of games like Wolfenstein 3D and Doom. Walls are rendered based on ray intersections, giving a pseudo-3D environment.
* **Multiple Themed Levels:** Battle your way through a series of distinct levels (currently `Level1` to `Level5` are implemented). Each level features unique textures for walls, floors, and ceilings, along with specific enemy types and layouts, providing varied challenges and environments.
* **Diverse Enemy Roster:** Encounter different types of enemies across the levels. They possess basic AI enabling them to patrol, chase the player upon detection, and fire projectiles. Some levels introduce tougher variants or even boss encounters!
* **Arsenal of Weapons:** Wield three distinct firearms:
    * **Pistol:** Your reliable starting sidearm.
    * **Shotgun:** A powerful close-range weapon, great for clearing rooms (unlockable).
    * **Automatic Weapon:** A rapid-fire energy weapon for sustained combat (unlockable).
* **Health & Power-ups:** Manage your health carefully! Collect health packs dropped by enemies or found in levels to stay alive. Discover and pick up new guns to expand your combat options.
* **Interactive Environments:** Engage with the game world by opening doors (`E` key) to progress. Special end-of-level doors trigger level completion and save your progress.
* **Intuitive GUI System:** Navigate the game easily with a graphical user interface:
    * **Start Menu:** Begin your adventure or exit the game.
    * **Level Selection:** Choose which level to play (unlocked levels are available).
    * **Pause Menu:** Pause the action, continue, or return to the main menu.
* **Persistent Progress:** The game automatically saves your highest level reached and which guns you've unlocked (`Save.txt`), so you can pick up where you left off.
* **Immersive Skyboxes:** Look up! Each level features a unique skybox texture that enhances the atmosphere and sense of place.
* **Detailed Sprite Rendering:** Enemies, bullets, and item pickups are rendered as sprites within the 3D world, correctly scaled and sorted based on distance for proper layering.
* **Atmospheric Sound Effects:** Hear the action! Includes sounds for weapon fire (Pistol, Shotgun, Automatic), picking up items, opening doors, and enemy actions (Requires Windows `PlaySound`).

## 📸 Screenshots & Videos Showcase

* **Gameplay Screenshots:**:

    ![image](https://github.com/user-attachments/assets/aaca24e2-4fda-40c2-9dcf-1179adeec254)
    *Caption: Encounter of a dangerous gnome in the corridors of Level 1!*

    ![image](https://github.com/user-attachments/assets/298f89cc-8a7f-4a5f-8951-e72b1a0a4441)
    *Caption: Battling alien foes on a strange planet in Level 4.*

    ![image](https://github.com/user-attachments/assets/b7febf6c-b364-472a-832e-1de2c74b22e0)
    *Caption: Facing off against the final boss!*
   
* **GUI Screenshots:**.
    
    ![Capture (1)](https://github.com/user-attachments/assets/5f0a3b75-3885-47b9-a86d-825e96a65792)
  
    *Caption: The main menu screen.*

    ![Capture (2)](https://github.com/user-attachments/assets/cb6218e3-af0b-4d78-9ea2-9c37ad519d49)
  
    *Caption: Choosing the next challenge.*
    
* **Gameplay Videos/GIFs:** Short clips are perfect for demonstrating the flow of the game.
    
    ![spaceship](https://github.com/user-attachments/assets/d45ea13f-0cb9-42b9-9d7d-fca7523918dd)
  
    *Caption: Clearing a room in a spaceship.*

    [![Watch the video](https://img.youtube.com/vi/ygnje6VUFOA/maxresdefault.jpg)](https://youtu.be/ygnje6VUFOA)
  
    *Caption: Fighting the zombie horde.*
  

## ⚙️ Installation & Setup Guide

![Coding Setup GIF](https://media.giphy.com/media/LmNwrBhejkK9EFP504/giphy.gif)

You can get game without the code easly with this link

https://www.dropbox.com/scl/fi/l9wuelyvmwxl6aqrk549p/retro-shooter.rar?rlkey=1q3upd8k61ljvv81knkbym1qr&st=kfmeop9k&dl=0

Otherwise, getting Legendary Retro Shooter up and running involves compiling the C++ source code.

1.  **Prerequisites (Dependencies):**
    * **olcPixelGameEngine (PGE):** The core graphics and game framework. You absolutely need the `olcPixelGameEngine.h` header file. Download it directly from the [OneLoneCoder/olcPixelGameEngine GitHub repository](https://github.com/OneLoneCoder/olcPixelGameEngine). Place it where your compiler can find it (either in the project directory or a standard include path).
    * **stb_image.h:** A vital single-header library for loading images (PNG, JPG, BMP, etc.) used for textures and sprites. Get `stb_image.h` from the [nothings/stb GitHub repository](https://github.com/nothings/stb). Like PGE, ensure it's accessible to your compiler. The necessary `#define STB_IMAGE_IMPLEMENTATION` is already present in `main.txt` (which should be renamed `main.cpp`), meaning you only need to include the header.
    * **C++ Compiler:** You'll need a compiler that supports at least C++11 (though C++17 is used in the example command). Common choices include:
        * **g++** (MinGW on Windows, standard on Linux)
        * **MSVC** (Microsoft Visual C++ within Visual Studio)
    * **Windows Operating System:** **Crucially**, this project uses the `PlaySound` function from the Windows API (`windows.h`) for audio playback. This means:
        * It will **only compile and run correctly on Windows** out-of-the-box.
        * To compile on Windows, you must link against the `winmm.lib` library, which provides multimedia functions like `PlaySound`.
        * 
2.  **Building the Game:**
    * Gather all your source files (`.cpp`) and header files (`.h`). This includes `main.cpp`, `misc.cpp`, `Player.cpp`, `Map.cpp`, `Levels.cpp` (assuming these exist based on headers), and any others.
    * Compile them together using your chosen C++ compiler, making sure to link the necessary libraries.
    * **Notes on Building:**
        * The specific Windows libraries (`-luser32`, `-lgdi32`, etc.) are standard requirements for olc::PGE applications on Windows.
        * `-lwinmm` is essential for `PlaySound`.
        * `-o shooter.exe` specifies the output executable name.
        * `-std=c++17` requests C++17 standard features. Adjust if needed.
        * `-static` attempts to link libraries statically (may depend on your toolchain setup).
        * *If using Visual Studio:* Create a project, add all source/header files, include the PGE and stb headers, and ensure `winmm.lib` is added to the Linker->Input->Additional Dependencies in the project properties.

3.  **Running the Game:**
    * Once compiled successfully, you'll have an executable (e.g., `shooter.exe`).
    * **Crucially:** Place the executable in the *same directory* as the `Resources` folder. The game expects to find all its assets (textures, sounds, level data, save file) within this folder structure.
    * Double-click the executable or run it from the command line (`./shooter.exe`).

![Debugging Fun GIF](https://media.giphy.com/media/ZVik7pBtu9dNS/giphy.gif)

## 🎮 How to Play - Controls & Objective

Master the controls to survive the retro onslaught!

* **Movement:**
    * `W`: Move Forward
    * `S`: Move Backward / Retreat
    * `A`: Turn Left
    * `D`: Turn Right
    *(Classic tank-style controls common in early FPS games)*
* **Combat & Interaction:**
    * `Space`: Fire your currently selected weapon. Hold down for automatic fire with the appropriate gun.
    * `E`: Interact with objects directly in front of you, primarily used to **Open Doors**.
    * `1`: Equip Pistol (Always available).
    * `2`: Equip Shotgun (Requires unlocking via pickup).
    * `3`: Equip Automatic Weapon (Requires unlocking via pickup).
* **Game Management:**
    * `ESC`: Access the Pause Menu during gameplay. From here you can continue or return to the Main Menu.

**Your Mission:**

Your goal is to successfully navigate each level from start to finish. Explore the corridors, eliminate hostile enemies, and manage your health. Search for doors and use the `E` key to open them, revealing new areas. Find the designated 'End Door' in each level to complete it and unlock the next. Keep an eye out for health packs and weapon pickups to improve your chances of survival! Your progress (highest level unlocked and guns acquired) is saved automatically when you complete a level.

![Coding Cat GIF](https://media.giphy.com/media/13HBDT4QSTpveU/giphy.gif)

## 🔗 Dependencies & Libraries

This project relies on the following external components:

* **[olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine)** by David Barr (Javidx9): The core engine providing the window, pixel-based drawing, input handling, and game loop structure.
* **[stb_image.h](https://github.com/nothings/stb)** by Sean Barrett: A single-file C library used for easily loading various image file formats into memory for use as textures.
* **Windows API (winmm.lib):** Used specifically for the `PlaySound` function to handle audio playback. This is a Windows-specific dependency.

## 📁 Project Structure Overview (Inferred)

```LegendaryRetroShooter/
├── main.cpp             # Core game logic, main loop, PGE setup, rendering calls
├── misc.cpp             # Texture class implementation (loading via stb_image)
├── misc.h               # Header for Texture class
├── Player.h             # Player class definition (position, health, rotation, etc.)
├── Player.cpp           # Player class implementation
├── map.h                # Definitions for Room, Block, MapData structures
├── map.cpp              # Implementation for Room/Map logic
├── Levels.h             # Level layout arrays, enemy/item placement data
├── Levels.cpp           # (Potentially empty or contains helper functions for levels)
├── olcPixelGameEngine.h # The Pixel Game Engine header
├── stb_image.h          # STB Image Loading library header
├── Resources/           # Folder containing all external game assets
│   ├── Fonts/           # (If any fonts were used, e.g., for UI)
│   ├── Guns/            # Gun sprites/animations (pistolAnim1.png, etc.)
│   ├── Level1/          # Assets specific to Level 1 (wall.png, floor.png, etc.)
│   ├── Level2/          # Assets specific to Level 2
│   ├── ...              # Folders for Level 3, 4, 5 assets
│   ├── Screen/          # GUI assets (startScreen.png, playButton.png, etc.)
│   ├── Sounds/          # Sound effect files (.wav)
│   └── Save.txt         # File to store persistent game progress
└── shooter.exe          # The compiled game executable (example name)
```

---

Thanks for checking out the project and have fun with it!
