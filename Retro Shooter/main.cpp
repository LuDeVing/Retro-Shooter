#include <iostream>
#include <utility>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION

#include "Player.h"
#include "map.h"

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include "Levels.h"

#define PI 3.14159265359
#define DEG 0.0174533

constexpr int screenWidth = 256;
constexpr int screenheight = 240;

int roomWidth;
int roomHeight;

class Example : public olc::PixelGameEngine {
public:

	// Game global variables

	Player player;

	Room room;
	MapData mapData;

	Texture skyTexture;

	std::vector <Enemy> enemies;
	std::vector <Sprite> sprites;
	std::vector <Bullet> bullets;
	
	std::vector <std::vector <Texture> > gunTextures;

	bool shotLastFrame = false;

	// GUI screen global variables

	bool gunUnlocked[3] = { true, true, true };

	int curLevel = 0;
	int maxLevelReachable = 4;
	int maxLevelNum = 4;

	int ScreenPage = 0;
		
	bool GUIScreenIsBeingRendered = true;
	bool PauseScreenIsBeingRendered = false;

	Texture startScreenImage;

	struct GUIButton {

		olc::vi2d position = olc::vi2d();

		int width = 0;
		int height = 0;

		Texture image = Texture();

		bool isActive = true;

		GUIButton() = default;

	};

	GUIButton startButton;
	GUIButton backButton;
	GUIButton exitButton;

	GUIButton levelPreviews[6];

	GUIButton continueButton;
	GUIButton mainMenuButton;
	
	// Structs

	enum gunType { PISTOL, SHOTGUN, AUTOMATIC };

	struct Gun {

		float width = 0, height = 0;

		int textureID = 0;
		int bulletID = 0;

		gunType type = AUTOMATIC;

		float shootingStateNum = 0.1; // Lower this is higher fire rate is
		float coolingStateNum = 0.4;

	};

	struct SpriteDist {

		int type;
		int idx;
		float distSquared;

		SpriteDist() {
			this->type = 0;
			this->idx = 0;
			this->distSquared = 1000000000;
		}

		SpriteDist(int type, int idx, float distX, float distY) {
			this->type = type;
			this->idx = idx;
			this->distSquared = distX * distX + distY * distY;
		}

		bool operator<(const SpriteDist& o){
			return (distSquared < o.distSquared);
		}

	};

	float shootingStateNumCur = 0;
	float coolingStateNumCur = 0;

	float fireRateExpired = 0;

	int shootintGunFrame = 0;

	std::vector <Gun> guns;

	const int cubeSize = 64;

	const float sqrtof2 = sqrtf(2.0f);

	int fov;
	int numberOfRays;

	float ceilShade = 0.7f;
	float wallShade = 0.7f;
	float floorShade = 0.7f;

	float depthBuffer[screenWidth];

	float globalDeltaTime = 1.0f;

	Example() {
		sAppName = "Legendary Retro Shooter";
	}

	
	/*************************************** MATH FUNCTIONS ***************************************/

	std::pair <float, std::pair<olc::vf2d, bool>> getRayDistance(Room& curRoom, float rot) {

		olc::vf2d rayPos = olc::vf2d(player.position);

		bool side;

		float shortestLength = 10000000000;

		float interx, intery;
		float dx, dy;

		bool breakFromLoop = false;

		olc::vf2d retVal(-10000000000, -10000000000);

		if (rot != 2 * PI && rot > PI) {

			intery = float(int(rayPos.y) / cubeSize * cubeSize) - 0.001;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = -cubeSize;
			dx = -cubeSize / tanf(rot);

		}
		else if (rot < PI) {

			intery = int(rayPos.y) / cubeSize * cubeSize + cubeSize;
			interx = rayPos.x - (rayPos.y - intery) / tanf(rot);

			dy = cubeSize;
			dx = cubeSize / tanf(rot);

		}

		if (rot == 0 || rot == PI) {
			breakFromLoop = true;
		}

		int maxIterations = 100;

		while (!breakFromLoop) {

			int pointx = interx / cubeSize;
			int pointy = intery / cubeSize;

			if (!curRoom.pointInRoom(pointx, pointy) || --maxIterations <= 0)
				break;

			if (curRoom.getBlock(pointx, pointy).type != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {

					shortestLength = newLenght;
					retVal = olc::vf2d(interx, intery);
					side = 0;

				}

				break;

			}

			interx += dx;
			intery += dy;
		}

		interx = 0, intery = 0;
		dx = 0, dy = 0;

		breakFromLoop = false;

		if (rot > PI / 2 && rot < 3 * PI / 2) {

			interx = float(int(rayPos.x) / cubeSize * cubeSize) - 0.001;
			intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

			dx = -cubeSize;
			dy = -cubeSize * tanf(rot);

		}
		else if (rot < PI / 2 || rot > 3 * PI / 2) {

			interx = int(rayPos.x) / cubeSize * cubeSize + cubeSize;
			intery = rayPos.y - (rayPos.x - interx) * tanf(rot);

			dx = cubeSize;
			dy = cubeSize * tanf(rot);

		}

		if (rot == PI / 2 || rot == rot > 3 * PI / 2) {
			breakFromLoop = true;
		}

		maxIterations = 100;

		while (!breakFromLoop) {

			int pointx = (int)interx / cubeSize;
			int pointy = (int)intery / cubeSize;

			if (!curRoom.pointInRoom(pointx, pointy) || --maxIterations <= 0)
				break;

			if (curRoom.getBlock(pointx, pointy).type != blockTypes::NONE) {

				float delX2 = (interx - rayPos.x) * (interx - rayPos.x);
				float delY2 = (intery - rayPos.y) * (intery - rayPos.y);

				float newLenght = sqrtf(delX2 + delY2);

				if (newLenght < shortestLength) {

					shortestLength = newLenght;
					retVal = olc::vf2d(interx, intery);
					side = 1;

				}

				break;

			}

			interx += dx;
			intery += dy;

		}

		return { shortestLength, {retVal,  side} };

	}

	/*************************************** HELPER FUNCTIONS ***************************************/

	bool fixCollisionOnRadius() {

		float posX, posY;

		for (int k = -1; k <= 1; k++) {
			for (int j = -1; j <= 1; j++) {

				posX = player.position.x + player.radius * (j == 0 ? k : k * sqrtof2);
				posY = player.position.y + player.radius * (k == 0 ? j : j * sqrtof2);
				
				if (room.getBlockSizeAdjusted(posX, posY, cubeSize).type != blockTypes::NONE)
					return true;

			}
		}

		return false;

	}

	void fixPlayerToWallCollision(float moveX, float moveY) {

		if (fixCollisionOnRadius()) {
			player.position.x -= moveX;
		}

		if (fixCollisionOnRadius()) {
			player.position.x += moveX;
			player.position.y -= moveY;
		}

		if (fixCollisionOnRadius()) {
			player.position.x -= moveX;
		}

	}

	float fixAngle(float Angle) {

		if (Angle < 0)
			Angle += 2 * PI;

		if (Angle > 2 * PI)
			Angle -= 2 * PI;

		return Angle;

	}

	void shootGun() {

		player.gunState = SHOOTING;

		shotLastFrame = true;

		if (guns[player.gunID].type == PISTOL) {

			PlaySound(TEXT("Resources/Sounds/PistolShoot.wav"), NULL, SND_ASYNC);

			bullets.push_back(Bullet(player.position.x, player.position.y, 7.0f, 8, 8));
			bullets[bullets.size() - 1].direction = olc::vf2d(cosf(player.roation), sinf(player.roation));
			bullets[bullets.size() - 1].textureID = guns[player.gunID].bulletID;
			bullets[bullets.size() - 1].speed = 1500.0f;
			bullets[bullets.size() - 1].damage = 35.0f;

		}
		else if (guns[player.gunID].type == SHOTGUN) {
			
			int numOfBullets = 5;
			
			PlaySound(TEXT("Resources/Sounds/ShotgunShoot.wav"), NULL, SND_ASYNC);

				
			while (numOfBullets--) {

				bullets.push_back(Bullet(player.position.x, player.position.y, 7.0f, 4, 4));
				bullets[bullets.size() - 1].direction = olc::vf2d(cosf(player.roation + DEG * (rand() % 30 - 15)), sinf(player.roation + DEG * (rand() % 30 - 15)));
				bullets[bullets.size() - 1].zDirection =  (rand() % 10 - 5) / 100.0f;
				bullets[bullets.size() - 1].textureID = guns[player.gunID].bulletID;


			}

		}
		else if (guns[player.gunID].type == AUTOMATIC) {

			PlaySound(TEXT("Resources/Sounds/EnergyShoot.wav"), NULL, SND_ASYNC);

			bullets.push_back(Bullet(player.position.x, player.position.y, 7.0f, 20, 20));
			bullets[bullets.size() - 1].direction = olc::vf2d(cosf(player.roation), sinf(player.roation));
			bullets[bullets.size() - 1].textureID = guns[player.gunID].bulletID;

		}

	}

	void drawGun(int gunTextureID, int textureID) {

		Texture& curText = gunTextures[gunTextureID][textureID];

		int xStart = ScreenWidth() / 2 - guns[player.gunID].width / 2;
		int xEnd = ScreenWidth() / 2 + guns[player.gunID].width / 2;

		int yStart = ScreenHeight() - guns[player.gunID].height;
		int yEnd = ScreenHeight();

		if (xEnd == xStart || yEnd == yStart)
			return;

		float texDiffX = curText.imageWidth / float(xEnd - xStart);
		float texDiffY = curText.imageHeight / float(yStart - yEnd);

		for (int y = yStart; y <= yEnd; y++) {
			for (int x = xStart; x <= xEnd; x++) {

				int xTexCord = (float)(x - xStart) * texDiffX;
				int yTexCord = (float)(yStart - y) * texDiffY;

				if(curText.getPixel(xTexCord, yTexCord).a != 0)
					Draw(x, y, curText.getPixel(xTexCord, yTexCord));

			}
		}

	}

	void drawHealthBar() {

		int barWidth = 100;
		int barHeight = 10;
			
		int barStart = 5;

		int startX = ScreenWidth() / 2 - barWidth / 2;
		int endX = ScreenWidth() / 2 + barWidth / 2;

		int startY = barStart;
		int endY = barStart + barHeight;

		for (int y = barStart - 1; y <= barStart + barHeight + 1; y++) {
			for (int x = ScreenWidth() / 2 - barWidth / 2 - 1; x <= ScreenWidth() / 2 + barWidth / 2 + 1; x++) {

				if ((x == startX - 1 && y == startY - 1) || (x == startX - 1 && y == endY + 1)
					|| (x == endX + 1 && y == startY - 1) || (x == endX + 1 && y == endY + 1)) {
					continue;
				}

				if (x == startX - 1 || x == endX + 1 || y == startY - 1 || y == endY + 1
					|| (x == startX && y == startY) || (x == startX && y == endY)
					|| (x == endX && y == startY) || (x == endX && y == endY)) {

					Draw(x, y, olc::VERY_DARK_GREY);

					continue;

				}

				int pointOfHealthX = x - (ScreenWidth() / 2 - barWidth / 2);

				bool shouldDrawRed = (pointOfHealthX <= barWidth * player.health / player.maxHealth);
				int subOfPointOfHealth = pointOfHealthX - barWidth * player.health / player.maxHealth;

				if (!shouldDrawRed) shouldDrawRed = (rand() % 10 == 9 && subOfPointOfHealth <= 1);

				if (shouldDrawRed)
					Draw(x, y, olc::Pixel(255 - rand() % 150, 0, 0));
				else Draw(x, y, olc::DARK_GREY);

			}
		}
			
	}

	int mouseButtonCollision(GUIButton& button) {
		
		int retVal = 0;

		if (GetMousePos().x >= button.position.x - button.width / 2 && GetMousePos().x <= button.position.x + button.width / 2
			&& GetMousePos().y >= button.position.y - button.height / 2 && GetMousePos().y <= button.position.y + button.height / 2) {

			retVal = 1;

			if (GetMouse(0).bHeld) {
				retVal = 2;
			}

			if (GetMouse(0).bReleased) {
				if(button.isActive)
					PlaySound(TEXT("Resources/Sounds/ButtonClick.wav"), NULL, SND_ASYNC);
				retVal = 3;
			}

		}

		return retVal;

	}

	void drawButton(GUIButton& button, bool darkenButton = false, bool isClicking = false, bool extraPixel = false) {

		int startX = button.position.x - button.width / 2;
		int endX = button.position.x + button.width / 2 - 1 + extraPixel;

		int startY = button.position.y - button.height / 2;
		int endY = button.position.y + button.height / 2;

		for (int x = startX; x <= endX; x++) {
			for (int y = startY; y <= endY; y++) {
				
				int xPos = x - (button.position.x - button.width / 2);
				int yPos = y - (button.position.y - button.height / 2);

				float xPix = (float)xPos * (float)button.image.imageWidth / (float)button.width;
				float yPix = (float)yPos * (float)button.image.imageHeight / (float)button.height;

				olc::Pixel curColor = button.image.getPixel(xPix, yPix);

				bool isXborder = (x == startX || x == endX);
				bool isYborder = (y == startY || y == endY);

				if (isClicking && (isXborder || isYborder)) {
					curColor = olc::RED;
				}

				if (darkenButton) {
					curColor.r = curColor.r * 3 / 4;
					curColor.g = curColor.g * 3 / 4;
					curColor.b = curColor.b * 3 / 4;
				}
				
				Draw(x, y, curColor);
					
			}
		}

	}

	void levelPreviewButtonLogic(GUIButton& button, int levelNum) {

		int curButtonCollision = mouseButtonCollision(button);

		if (curButtonCollision == 0) {
			drawButton(button, false, false, true);
		}
		else if (curButtonCollision == 1 && button.isActive) {
			drawButton(button, true, false, true);
		}
		else if (curButtonCollision == 2 && button.isActive) {
			drawButton(button, false, true, true);
		}
		else if (curButtonCollision == 3 && button.isActive) {

			drawButton(button, false, true, true);
			
			if (levelNum == 0) {
				loadLevel1();
			}
			if (levelNum == 1) {
				loadLevel2();
			}
			if (levelNum == 2) {
				loadLevel3();
			}
			if (levelNum == 3) {
				loadLevel4();
			}
			if (levelNum == 4) {
				loadLevel5();
			}

			GUIScreenIsBeingRendered = false;

		}

	}

	void loadGuns() {

		guns.push_back(Gun());

		guns[0].width = (float)ScreenWidth() / 4.0f;
		guns[0].height = (float)ScreenHeight() / 4.0f;

		guns[0].shootingStateNum = 0.5f;

		guns[0].type = PISTOL;

		guns[0].bulletID = 2;
		guns[0].textureID = 0;

		gunTextures.push_back(std::vector<Texture>(4));

		gunTextures[0][0] = Texture("Resources/guns/pistolAnim1.png");
		gunTextures[0][1] = Texture("Resources/guns/pistolAnim2.png");
		gunTextures[0][2] = Texture("Resources/guns/pistolAnim3.png");
		gunTextures[0][3] = Texture("Resources/guns/pistolAnim4.png");

		guns.push_back(Gun());

		guns[1].width = (float)ScreenWidth() / 4.0f;
		guns[1].height = (float)ScreenHeight() / 4.0f;

		guns[1].shootingStateNum = 0.7f;

		guns[1].type = SHOTGUN;

		guns[1].bulletID = 2;
		guns[1].textureID = 1;

		gunTextures.push_back(std::vector<Texture>(4));

		gunTextures[1][0] = Texture("Resources/guns/shotGunAnim1.png");
		gunTextures[1][1] = Texture("Resources/guns/shotGunAnim2.png");
		gunTextures[1][2] = Texture("Resources/guns/shotGunAnim3.png");
		gunTextures[1][3] = Texture("Resources/guns/shotGunAnim4.png");

		guns.push_back(Gun());

		guns[2].width = (float)ScreenWidth() / 4.0f;
		guns[2].height = (float)ScreenHeight() / 4.0f;

		guns[2].type = AUTOMATIC;

		guns[2].bulletID = 0;
		guns[2].textureID = 2;

		gunTextures.push_back(std::vector<Texture>(4));

		gunTextures[2][0] = Texture("Resources/guns/gunAnim1.png");
		gunTextures[2][1] = Texture("Resources/guns/gunAnim2.png");
		gunTextures[2][2] = Texture("Resources/guns/gunAnim3.png");
		gunTextures[2][3] = Texture("Resources/guns/gunAnim4.png");

	}

 	/*************************************** IMPORTANT FUNCTIONS ***************************************/

	void loadData() {

		std::fstream saveFile;
		saveFile.open("Resources/Save.txt");

		if (saveFile.is_open()) {

			std::streambuf* backup;

			backup = std::cin.rdbuf();

			std::cin.rdbuf(saveFile.rdbuf());

			std::cin >> maxLevelReachable;

			for (int k = 0; k < 3; k++) 
				std::cin >> gunUnlocked[k];

			std::cin.rdbuf(backup);

		}

		saveFile.close();

	}

	void saveData() {

		std::fstream saveFile;
		saveFile.open("Resources/Save.txt");

		if (saveFile.is_open()) {

			std::streambuf *backup;

			backup = std::cout.rdbuf();

			std::cout.rdbuf(saveFile.rdbuf());

			std::cout << maxLevelReachable;

			std::cout << '\n';

			for (int k = 0; k < 3; k++)
				std::cout << gunUnlocked[k] << ' ';

			std::cout.rdbuf(backup);

		}


	}

	void getInputs(float fElapsedTime) {

		// Moving

		if (GetKey(olc::A).bHeld) {

			player.roation -= DEG * 150 * fElapsedTime;

			if (player.roation < 0)
				player.roation += 2 * PI;

		}

		if (GetKey(olc::D).bHeld) {

			player.roation += DEG * 150 * fElapsedTime;

			if (player.roation > 2 * PI)
				player.roation -= 2 * PI;

		}

		if (GetKey(olc::W).bHeld) {

			float moveX = player.speed * cosf(player.roation) * fElapsedTime;
			float moveY = player.speed * sinf(player.roation) * fElapsedTime;

			player.position.x += moveX;
			player.position.y += moveY;

			fixPlayerToWallCollision(moveX, moveY);
		}

		if (GetKey(olc::S).bHeld) {

			float moveX = -player.speed * cosf(player.roation) * fElapsedTime;
			float moveY = -player.speed * sinf(player.roation) * fElapsedTime;

			player.position.x += moveX;
			player.position.y += moveY;

			fixPlayerToWallCollision(moveX, moveY);

		}

		if (GetKey(olc::K1).bPressed) {
			if (gunUnlocked[0]) {
				player.gunID = 0;
			}
		}

		if (GetKey(olc::K2).bPressed) {
			if (gunUnlocked[1]) {
				player.gunID = 1;
			}
		}

		if (GetKey(olc::K3).bPressed) {
			if (gunUnlocked[2]) {
				player.gunID = 2;
			}
		}

		if (GetKey(olc::ESCAPE).bPressed) {
			PauseScreenIsBeingRendered = true;
		}

		// Shooting

		if (GetKey(olc::SPACE).bHeld && coolingStateNumCur != guns[player.gunID].coolingStateNum) {

			if(shootingStateNumCur <= 0)
				shootingStateNumCur = guns[player.gunID].shootingStateNum;

			if(fireRateExpired == 0)
				shootGun();

			fireRateExpired = (fireRateExpired + globalDeltaTime);

			if (fireRateExpired >= guns[player.gunID].shootingStateNum)
				fireRateExpired = 0;

		}
		else if (shotLastFrame) {
			
			player.gunState = COOLING;
			shotLastFrame = false;

			coolingStateNumCur = guns[player.gunID].coolingStateNum;

			fireRateExpired = 0;

		}
		else {
			player.gunState = STATIC;
			fireRateExpired = 0;
		}
		
		// Interacting

		if (GetKey(olc::E).bPressed) {

			olc::vf2d forPos = olc::vf2d(player.position.x + cubeSize / 3.0f * cosf(player.roation), player.position.y + cubeSize / 3.0f * sinf(player.roation));
			
			Block& curBlock = room.getBlockSizeAdjusted(forPos.x, forPos.y, cubeSize);

			if (curBlock.type == DOOR || curBlock.type == ENDDOOR) {
				PlaySound(TEXT("Resources/Sounds/DoorSound.wav"), NULL, SND_ASYNC);
				curBlock.isOpen = true;
			}

		}

	}

	void renderScene(int fov, int numberOfRays) {

		float curDeg = DEG * -(fov / 2) + player.roation;

		for (int x = 0; x < numberOfRays; x++) {

			float useDeg = fixAngle(curDeg);
			float degBet = fixAngle(player.roation - useDeg);

			float curDist; bool side;
			olc::vf2d curInter;

			std::pair <float, std::pair<olc::vf2d, bool>> retVal = getRayDistance(room, useDeg);
			curDist = retVal.first, curInter = retVal.second.first, side = retVal.second.second;

			depthBuffer[x] = curDist;

			float curLightMultiplier = (!side ? 1.0f : wallShade);

			curDist *= cosf(degBet);

			float lineHeight = (cubeSize * ScreenHeight()) / curDist;
			float unclippedLineHeight = lineHeight;

			lineHeight = std::min(lineHeight, (float)ScreenHeight());

			olc::Pixel curColor((float)255 * curLightMultiplier, (float)255 * curLightMultiplier, (float)255 * curLightMultiplier);

			Block& curWall = room.getBlockSizeAdjusted(curInter.x, curInter.y, cubeSize);

			if (curWall.wallTextureID != -1) {

				float texSizeDiffX = (float)cubeSize / (float)room.getWallTexture(curWall.wallTextureID).imageWidth;
				float texSizeDiffY = (float)cubeSize / (float)room.getWallTexture(curWall.wallTextureID).imageHeight;

				float ty_step = ((float)cubeSize / unclippedLineHeight) / texSizeDiffX;
				float ty_offset = (unclippedLineHeight > ScreenHeight() ? (unclippedLineHeight - ScreenHeight()) / 2.0 : 0);

				float ty = ty_step * ty_offset;
				float tx;

				if (!side) {
					tx = ((int)(curInter.x / 2.0f) % cubeSize) / texSizeDiffY;
					if (useDeg > PI) tx = (float)room.getWallTexture(curWall.wallTextureID).imageWidth - tx - 1;
				}
				else {
					tx = ((int)(curInter.y / 2.0f) % cubeSize) / texSizeDiffY;
					if (useDeg > PI / 2 && useDeg < 3 * PI / 2) tx = (float)room.getWallTexture(curWall.wallTextureID).imageWidth - tx - 1;
				}

				for (int y = (ScreenHeight() - lineHeight) / 2; y <= (ScreenHeight() + lineHeight) / 2; y++) {

					int texWidth = room.getWallTexture(curWall.wallTextureID).imageWidth;
					int texHeight = room.getWallTexture(curWall.wallTextureID).imageHeight;

					olc::Pixel curColor = room.getWallTexture(curWall.wallTextureID).getPixel(std::max(0, (int)(tx * 2.0f) % texWidth), std::max(0, (int)(ty) % texHeight));

					curColor.r *= curLightMultiplier;
					curColor.g *= curLightMultiplier;
					curColor.b *= curLightMultiplier;

					if (curColor.a != 0)
						Draw(x, y, curColor);

					ty += ty_step;

				}

			}
			else DrawLine(olc::vf2d(x, (ScreenHeight() - lineHeight) / 2), olc::vf2d(x, (ScreenHeight() + lineHeight) / 2), curColor);

			for (int y = (ScreenHeight() + lineHeight) / 2 + 1; y < ScreenHeight(); y++) {

				float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

				float px = player.position.x;
				float py = player.position.y;

				float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

				float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
				float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

				int mpX = tx / (cubeSize / 2.0);
				int mpY = ty / (cubeSize / 2.0);

				Block& curFloorWall = room.getBlock(mpX, mpY);

				int curImgWidth = room.getFloorTexture(curFloorWall.floorTextureID).imageWidth;
				int curImgHeight = room.getFloorTexture(curFloorWall.floorTextureID).imageHeight;

				tx /= (float)cubeSize / (float)curImgWidth;
				ty /= (float)cubeSize / (float)curImgHeight;

				tx = ((int)tx & (curImgWidth - 1));
				ty = ((int)ty & (curImgHeight - 1));

				olc::Pixel curColor = room.getFloorTexture(curWall.floorTextureID).getPixel((int)tx, (int)ty);

				if (curFloorWall.hasCeil) {

					curColor.r *= floorShade;
					curColor.g *= floorShade;
					curColor.b *= floorShade;

				}

				if (curColor.a != 0)
					Draw(x, y, curColor);
	
			}

			for (int y = (ScreenHeight() + lineHeight) / 2; y <= ScreenHeight(); y++) {

				float dy = y - (ScreenHeight() / 2.0), raFix = cosf(degBet);

				float px = player.position.x;
				float py = player.position.y;

				float magicNum = 158.0f * ((float)ScreenHeight() / 320.0f);

				float tx = px / 2 + cos(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;
				float ty = py / 2 + sin(useDeg) * magicNum * (cubeSize / 2.0) / dy / raFix;

				int mpX = tx / (cubeSize / 2.0);
				int mpY = ty / (cubeSize / 2.0);

				Block& curCeilWall = room.getBlock(mpX, mpY);

				if (curCeilWall.hasCeil == false)
					continue;

				int curImgWidth = room.getCeilTexture(curCeilWall.ceilTextureID).imageWidth;
				int curImgHeight = room.getCeilTexture(curCeilWall.ceilTextureID).imageHeight;

				tx /= (float)cubeSize / (float)curImgWidth;
				ty /= (float)cubeSize / (float)curImgHeight;

				tx = ((int)tx & (curImgWidth - 1));
				ty = ((int)ty & (curImgHeight - 1));

				olc::Pixel curColor = room.getCeilTexture(curWall.ceilTextureID).getPixel((int)tx, (int)ty);

				curColor.r *= ceilShade;
				curColor.g *= ceilShade;
				curColor.b *= ceilShade;

				if(curColor.a != 0)
					Draw(x, ScreenHeight() - y, curColor);
			
	
			}

			curDeg += DEG * ((float)fov / (float)numberOfRays);

		}

	}

	void drawSky() {
		
		float playerRotInDegs = float(player.roation / (2 * PI) * 360.0f) / 2;

		for (int y = ScreenHeight(); y >= 0; y--) {
			for (int x = 0; x < ScreenWidth(); x++) {

				float xRotated = playerRotInDegs * (ScreenWidth() / 128.0f * 2.0f) + (float(x) / 2);
				if (xRotated < 0) { xRotated += 120; } xRotated = (int)xRotated % 120;

				float tx = xRotated * skyTexture.imageWidth / (float)ScreenWidth() * 2.15f;
				float ty = y * skyTexture.imageHeight / (float)ScreenHeight() * 2.15f / 2;

				Draw(x, y, skyTexture.getPixel((int)tx, (int)ty));

			}
		}

	}

	void drawSprite(Sprite &curSprite) {

		float dx = curSprite.x - player.position.x;
		float dy = curSprite.y - player.position.y;
		float dz = curSprite.z;

		const float minDist = cubeSize / 2.9f;

		if (dx * dx + dy * dy < minDist * minDist)
			return;

		olc::vf2d curForwardVec = olc::vf2d(cos(player.roation), sin(player.roation));
		olc::vf2d curVecDir = olc::vf2d(-dx, -dy);

		float dotProd = curForwardVec.dot(curVecDir);

		if (dotProd > 0)
			return;

		float csns = cosf(-(player.roation));
		float sns = sinf(-(player.roation));

		float rt1 = dy * csns + dx * sns;
		float rt2 = dx * csns - dy * sns;

		dx = rt1;
		dy = rt2;

		float magicNum = 108.0f / 128.0f * (float)ScreenWidth();

		dx = (dx * magicNum / dy) + (ScreenHeight() / 2.0f);
		dy = (dz * magicNum / dy) + (ScreenWidth() / 2.0f);

		olc::Pixel curCol;

		int tx;
		int ty;

		int txW = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth;
		int txH = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageHeight;

		float ScaleMult = (80.0f / 256.0f * (float)ScreenWidth()) / rt2;

		float heightScaled = curSprite.height * ScaleMult;
		float widthScaled = curSprite.width * ScaleMult;

		if (widthScaled > ScreenWidth() * 10 || heightScaled > ScreenHeight() * 10) {
			return;
		}


		for (int y = 0; y <= heightScaled; y++) {

			if (dy - y < 0 || dy - y > ScreenHeight())
				continue;

			for (int x = 0; x <= widthScaled; x++) {

				int xPlace = dx + x - widthScaled / 2;

				if (xPlace < 0 || xPlace >= ScreenWidth() || rt2 > depthBuffer[xPlace])
					continue;

				// depthBuffer[xPlace] = rt2;

				tx = (float)x * (float)room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth / widthScaled;
				ty = (float)y * (float)room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).imageWidth / widthScaled;

				curCol = room.getSpriteTexture(curSprite.textureID, curSprite.textureSectionID).getPixel(txW - tx, txH - ty);

				if (curCol.a != 0)
					Draw(xPlace, dy - y, curCol);

			}
		}

	}

	void renderGui() {

		if (shootingStateNumCur > 0) {

			if (shootingStateNumCur >= guns[player.gunID].shootingStateNum / 2) {
				drawGun(guns[player.gunID].textureID, 1);
			}
			else drawGun(guns[player.gunID].textureID, 2);

			shootingStateNumCur -= globalDeltaTime;

		}
		else if (coolingStateNumCur > 0) {

			drawGun(guns[player.gunID].textureID, 3);

			coolingStateNumCur -= globalDeltaTime;

		}
		else if (player.gunState == STATIC) {

			drawGun(guns[player.gunID].textureID, 0);

		}

		drawHealthBar();

	}

	void roomLogic() {

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				Block& curBlock = room.getBlock(x, y);

				if (curBlock.isDoor) {
					curBlock.doorCicle(player.position, cubeSize, olc::vi2d(x, y), globalDeltaTime);
				}
				else if (curBlock.isEndDoor && curBlock.isOpen) {
					
					maxLevelReachable = std::min(curLevel + 1, maxLevelNum);
					
					std::string curImageFilePath = "Resources/Screen/Level" + std::to_string(maxLevelReachable) + "Preview.png";

					levelPreviews[maxLevelReachable].image = Texture(curImageFilePath.c_str());
					levelPreviews[maxLevelReachable].isActive = true;

					GUIScreenIsBeingRendered = true;

					sprites.clear();
					enemies.clear();
					bullets.clear();
					gunTextures.clear();
					guns.clear();

					saveData();
				
				}

			}
		}

	}

	void drawStartScreen() {

		for (int y = 0; y < ScreenHeight(); y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				Draw(x, y, startScreenImage.getPixel(x, y));
			}
		}

		if (ScreenPage == 0) {

			int startButtonCollision = mouseButtonCollision(startButton);
			int exitButtonCollision = mouseButtonCollision(exitButton);

			if (startButtonCollision == 0) {
				drawButton(startButton);
			}
			else if (startButtonCollision == 1) {
				drawButton(startButton, true);
			}
			else if (startButtonCollision == 2) {
				drawButton(startButton, false, true);
			}
			else if (startButtonCollision == 3) {
				drawButton(startButton, false, true);
				ScreenPage = 1;
				return;
			}

			if (exitButtonCollision == 0) {
				drawButton(exitButton);
			}
			else if (exitButtonCollision == 1) {
				drawButton(exitButton, true);
			}
			else if (exitButtonCollision == 2) {
				drawButton(exitButton, false, true);
			}
			else if (exitButtonCollision == 3) {
				drawButton(exitButton, false, true);
				exit(0);
			}
			
		}

		if (ScreenPage == 1) {

			if (GetKey(olc::ESCAPE).bPressed) {
				drawButton(backButton, false, true);
				ScreenPage = 0;
			}

			int backButtonCollision = mouseButtonCollision(backButton);

			if (backButtonCollision == 0) {
				drawButton(backButton);
			}
			else if (backButtonCollision == 1) {
				drawButton(backButton, true);
			}
			else if (backButtonCollision == 2) {
				drawButton(backButton, false, true);
			}
			else if (backButtonCollision == 3) {
				drawButton(backButton, false, true);
				ScreenPage = 0;
			}

			levelPreviewButtonLogic(levelPreviews[0], 0);
			levelPreviewButtonLogic(levelPreviews[1], 1);
			levelPreviewButtonLogic(levelPreviews[2], 2);
			levelPreviewButtonLogic(levelPreviews[3], 3);
			levelPreviewButtonLogic(levelPreviews[4], 4);


		}

	}

	void drawPausedScreen() {

		for (int y = 0; y < ScreenHeight(); y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				Draw(x, y, startScreenImage.getPixel(x, y));
			}
		}

		int continueButtonCollision = mouseButtonCollision(continueButton);
		int mainMenuButtonCollision = mouseButtonCollision(mainMenuButton);

		if (continueButtonCollision == 0) {
			drawButton(continueButton);
		}
		else if (continueButtonCollision == 1) {
			drawButton(continueButton, true);
		}
		else if (continueButtonCollision == 2) {
			drawButton(continueButton, false, true);
		}
		else if (continueButtonCollision == 3) {
			drawButton(continueButton, false, true);
			PauseScreenIsBeingRendered = false;
		}

		if (mainMenuButtonCollision == 0) {
			drawButton(mainMenuButton);
		}
		else if (mainMenuButtonCollision == 1) {
			drawButton(mainMenuButton, true);
		}
		else if (mainMenuButtonCollision == 2) {
			drawButton(mainMenuButton, false, true);
		}
		else if (mainMenuButtonCollision == 3) {
			drawButton(mainMenuButton, false, true);
			PauseScreenIsBeingRendered = false;			
			ScreenPage = 0;
			GAMEOVER();
		}

	}

	void GAMEOVER() {

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		GUIScreenIsBeingRendered = true;

	}

	void loadLevel1() {

		player = Player();

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		roomWidth = level1::roomWidth;
		roomHeight = level1::roomHeight;

		floorShade = 0.7f;
		wallShade = 0.7f;
		ceilShade = 0.7f;

		curLevel = 0;

		// room

		room = Room(roomWidth, roomHeight);

		// textures

		room.addWallTexture("Resources/Level1/wall.png");
		room.addFloorTexture("Resources/Level1/floor.png");
		room.addCeilTexture("Resources/Level1/ceiling.png");

		room.addWallTexture("Resources/Level1/door.png");

		skyTexture = Texture("Resources/Level1/sky.bmp");

		room.addSpriteTexture("Resources/Level1/enemy.png", 1);
		room.addSpriteTexture("Resources/Level1/enemyShoot.png", 1);
		
		room.addSpriteTexture("Resources/Level1/enemyDead.png", 0);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);
		room.addSpriteTexture("Resources/Level1/enemyBullet.png", 2);
		room.addSpriteTexture("Resources/shotgunShell.png", 2);

		room.addSpriteTexture("Resources/HealthPickUp.png", 0);
		
		// fill room		

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				if (level1::level[y * roomWidth + x] == 1) {

					room.changeBlock(x, y, blockTypes::WALL);
					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

				}
				else if (level1::level[y * roomWidth + x] == 2) {

					room.changeBlock(x, y, blockTypes::DOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isDoor = true;

				}
				else if (level1::level[y * roomWidth + x] == 3) {

					room.changeBlock(x, y, blockTypes::ENDDOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isEndDoor = true;
				
				}
				else {

					if (level1::level[y * roomWidth + x] == -1) {
						player.position = olc::vf2d(x * cubeSize + cubeSize / 2 + 0.001f, y * cubeSize + cubeSize / 2 + 0.001f);
					}
					else if (level1::level[y * roomWidth + x] == -2) {
						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 2, cubeSize * 2, room.getWidth(), room.getHeight()));
						enemies[enemies.size() - 1].textureID = 0;
						enemies[enemies.size() - 1].staticTextureID = 0;
						enemies[enemies.size() - 1].shootTextureID = 1;
						enemies[enemies.size() - 1].deadTextureID = 0;
						enemies[enemies.size() - 1].bulletTextureID = 1;
					}
					else if (level1::level[y * roomWidth + x] == -3) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 1;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH2;
					}

					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 15) room.getBlock(x, y).hasCeil = false;

				}

			}
		}

		// mapData

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		// guns

		player.gunID = 0;

		loadGuns();

		// render variables

		fov = 60;
		numberOfRays = ScreenWidth();

	}

	void loadLevel2() {

		player = Player();

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		roomWidth = level2::roomWidth;
		roomHeight = level2::roomHeight;

		floorShade = 0.85f;
		wallShade = 0.85f;
		ceilShade = 0.85f;

		curLevel = 1;

		// room

		room = Room(roomWidth, roomHeight);

		// textures

		room.addWallTexture("Resources/Level2/wall.png");
		room.addFloorTexture("Resources/Level2/floor.png");
		room.addCeilTexture("Resources/Level2/ceiling.png");

		room.addWallTexture("Resources/Level2/door.png");

		skyTexture = Texture("Resources/Level2/sky.bmp");

		room.addSpriteTexture("Resources/Level2/enemy.png", 1);
		room.addSpriteTexture("Resources/Level2/enemyShoot.png", 1);

		room.addSpriteTexture("Resources/Level2/enemyDead.png", 0);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);
		room.addSpriteTexture("Resources/Level2/enemyBullet.png", 2);
		room.addSpriteTexture("Resources/shotgunShell.png", 2);

		room.addSpriteTexture("Resources/HealthPickUp.png", 0);
		room.addSpriteTexture("Resources/gunSprite.png", 0);

		// fill room		

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				if (level2::level[y * roomWidth + x] == 1) {

					room.changeBlock(x, y, blockTypes::WALL);
					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

				}
				else if (level2::level[y * roomWidth + x] == 2) {

					room.changeBlock(x, y, blockTypes::DOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isDoor = true;

				}
				else if (level2::level[y * roomWidth + x] == 3) {

					room.changeBlock(x, y, blockTypes::ENDDOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isEndDoor = true;

				}
				else {

					if (level2::level[y * roomWidth + x] == -1) {
						player.position = olc::vf2d(x * cubeSize + cubeSize / 2 + 0.001f, y * cubeSize + cubeSize / 2 + 0.001f);
					}
					else if (level2::level[y * roomWidth + x] == -2) {
						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 2, cubeSize * 2, room.getWidth(), room.getHeight()));
						enemies[enemies.size() - 1].textureID = 0;
						enemies[enemies.size() - 1].staticTextureID = 0;
						enemies[enemies.size() - 1].shootTextureID = 1;
						enemies[enemies.size() - 1].deadTextureID = 0;
						enemies[enemies.size() - 1].bulletTextureID = 1;
					}
					else if (level2::level[y * roomWidth + x] == -3) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 1;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH2;
					}
					else if (level2::level[y * roomWidth + x] == -5) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 2;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::GUN2;
					}

					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 14) room.getBlock(x, y).hasCeil = false;

				}

			}
		}

		// mapData

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		// guns

		player.gunID = 0;

		loadGuns();

		// render variables

		fov = 60;
		numberOfRays = ScreenWidth();

	}

	void loadLevel3() {

		player = Player();

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		roomWidth = level3::roomWidth;
		roomHeight = level3::roomHeight;

		floorShade = 0.7f;
		wallShade = 0.7f;
		ceilShade = 0.7f;

		curLevel = 2;

		// room

		room = Room(roomWidth, roomHeight);

		// textures

		room.addWallTexture("Resources/Level3/wall.png");
		room.addFloorTexture("Resources/Level3/floor.png");
		room.addCeilTexture("Resources/Level3/ceiling.png");

		room.addWallTexture("Resources/Level3/door.png");

		skyTexture = Texture("Resources/Level3/sky.bmp");

		room.addSpriteTexture("Resources/Level3/enemy.png", 1);
		room.addSpriteTexture("Resources/Level3/enemyShoot.png", 1);

		room.addSpriteTexture("Resources/Level3/enemyDead.png", 0);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);
		room.addSpriteTexture("Resources/Level3/enemyBullet.png", 2);
		room.addSpriteTexture("Resources/shotgunShell.png", 2);

		room.addSpriteTexture("Resources/HealthPickUp.png", 0);

		// fill room		

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				if (level3::level[y * roomWidth + x] == 1) {

					room.changeBlock(x, y, blockTypes::WALL);
					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

				}
				else if (level3::level[y * roomWidth + x] == 2) {

					room.changeBlock(x, y, blockTypes::DOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isDoor = true;

				}
				else if (level3::level[y * roomWidth + x] == 3) {

					room.changeBlock(x, y, blockTypes::ENDDOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isEndDoor = true;

				}
				else {

					if (level3::level[y * roomWidth + x] == -1) {
						player.position = olc::vf2d(x * cubeSize + cubeSize / 2 + 0.001f, y * cubeSize + cubeSize / 2 + 0.001f);
					}
					else if (level3::level[y * roomWidth + x] == -2) {
						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 2, cubeSize * 2, room.getWidth(), room.getHeight()));
						enemies[enemies.size() - 1].textureID = 0;
						enemies[enemies.size() - 1].staticTextureID = 0;
						enemies[enemies.size() - 1].shootTextureID = 1;
						enemies[enemies.size() - 1].deadTextureID = 0;
						enemies[enemies.size() - 1].bulletTextureID = 1;
					}
					else if (level3::level[y * roomWidth + x] == -3) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 1;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH2;
					}

					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 54) room.getBlock(x, y).hasCeil = false;

				}

			}
		}

		// mapData

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		// guns

		player.gunID = 0;

		loadGuns();

		// render variables

		fov = 60;
		numberOfRays = ScreenWidth();

	}

	void loadLevel4() {

		player = Player();

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		roomWidth = level4::roomWidth;
		roomHeight = level4::roomHeight;

		floorShade = 0.7f;
		wallShade = 0.7f;
		ceilShade = 0.7f;

		curLevel = 3;

		// room

		room = Room(roomWidth, roomHeight);

		// textures

		room.addWallTexture("Resources/Level4/wall.png");
		room.addFloorTexture("Resources/Level4/floor.png");
		room.addCeilTexture("Resources/Level4/ceiling.png");

		room.addWallTexture("Resources/Level4/door.jpg");

		room.addWallTexture("Resources/Level4/wallWindow.png");

		skyTexture = Texture("Resources/Level4/sky.bmp");

		room.addSpriteTexture("Resources/Level4/enemy.png", 1);
		room.addSpriteTexture("Resources/Level4/enemyShoot.png", 1);

		room.addSpriteTexture("Resources/Level4/enemyAlien.png", 1);
		room.addSpriteTexture("Resources/Level4/enemyAlienShoot.png", 1);

		room.addSpriteTexture("Resources/Level4/enemyDead.png", 0);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);
		room.addSpriteTexture("Resources/Level4/enemyBullet.png", 2);
		room.addSpriteTexture("Resources/shotgunShell.png", 2);

		room.addSpriteTexture("Resources/Level4/enemyAlienBullet.png", 2);

		room.addSpriteTexture("Resources/HealthPickUp.png", 0);
		room.addSpriteTexture("Resources/Level4/enemyAlienDead.png", 0);

		room.addWallTexture("Resources/Level4/planetBorder.png");
		room.addFloorTexture("Resources/Level4/planetGround.png");

		room.addSpriteTexture("Resources/gunSprite.png", 0);

		// fill room		

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				if (level4::level[y * roomWidth + x] == 1) {

					room.changeBlock(x, y, blockTypes::WALL);
					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if ((x == 0 || x == room.getWidth() - 1) && y % 4 == 0)
						room.changeWallTexture(x, y, 2);

					if (y > 70) {

						if ((x == 0 || x == room.getWidth() - 1))
							room.changeWallTexture(x, y, 3);
		
						room.changeFloorTexture(x, y, 1);
					
					}


				}
				else if (level4::level[y * roomWidth + x] == 2) {

					room.changeBlock(x, y, blockTypes::DOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 70) {
						room.changeFloorTexture(x, y, 1);
					}

					room.getBlock(x, y).isDoor = true;

				}
				else if (level4::level[y * roomWidth + x] == 3) {

					room.changeBlock(x, y, blockTypes::ENDDOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 70) {
						room.changeFloorTexture(x, y, 1);
					}

					room.getBlock(x, y).isEndDoor = true;

				}
				else {

					if (level4::level[y * roomWidth + x] == -1) {
						player.position = olc::vf2d(x * cubeSize + cubeSize / 2 + 0.001f, y * cubeSize + cubeSize / 2 + 0.001f);
					}
					else if (level4::level[y * roomWidth + x] == -2) {
						
						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 2, cubeSize * 2, room.getWidth(), room.getHeight()));
						
						enemies[enemies.size() - 1].textureID = 0;
						enemies[enemies.size() - 1].staticTextureID = 0;
						enemies[enemies.size() - 1].shootTextureID = 1;
						enemies[enemies.size() - 1].deadTextureID = 0;

						enemies[enemies.size() - 1].bulletTextureID = 1;

						if (y > 70) {

							enemies[enemies.size() - 1].textureID = 2;
							enemies[enemies.size() - 1].staticTextureID = 2;
							enemies[enemies.size() - 1].shootTextureID = 3;
							enemies[enemies.size() - 1].deadTextureID = 2;

							enemies[enemies.size() - 1].bulletTextureID = 3;

						}

					}
					else if (level4::level[y * roomWidth + x] == -3) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 1;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH2;
					}
					else if (level4::level[y * roomWidth + x] == -5) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 3;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::GUN3;
					}

					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 70) room.getBlock(x, y).hasCeil = false;

				}

			}
		}

		// mapData

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		// guns

		player.gunID = 0;

		loadGuns();

		// render variables

		fov = 60;
		numberOfRays = ScreenWidth();

	}

	void loadLevel5() {

		player = Player();

		sprites.clear();
		enemies.clear();
		bullets.clear();
		gunTextures.clear();
		guns.clear();

		roomWidth = level5::roomWidth;
		roomHeight = level5::roomHeight;

		floorShade = 0.7f;
		wallShade = 0.7f;
		ceilShade = 0.7f;

		curLevel = 4;

		// room

		room = Room(roomWidth, roomHeight);

		// textures

		room.addWallTexture("Resources/Level5/wall.png");
		room.addFloorTexture("Resources/Level5/floor.png");
		room.addCeilTexture("Resources/Level5/ceiling.png");

		room.addWallTexture("Resources/Level5/door.png");

		skyTexture = Texture("Resources/Level5/sky.bmp");

		room.addSpriteTexture("Resources/Level5/enemy.png", 1);
		room.addSpriteTexture("Resources/Level5/enemyShoot.png", 1);

		room.addSpriteTexture("Resources/Level5/boss.png", 1);
		room.addSpriteTexture("Resources/Level5/bossShoot.png", 1);

		room.addSpriteTexture("Resources/Level5/enemyDead.png", 0);

		room.addSpriteTexture("Resources/EnergyBall.png", 2);
		room.addSpriteTexture("Resources/Level5/enemyBullet.png", 2);
		room.addSpriteTexture("Resources/shotgunShell.png", 2);

		room.addSpriteTexture("Resources/HealthPickUp.png", 0);

		room.addSpriteTexture("Resources/Level5/bossDead.png", 0);

		// fill room		

		for (int y = 0; y < roomHeight; y++) {
			for (int x = 0; x < roomWidth; x++) {

				if (level5::level[y * roomWidth + x] == 1) {

					room.changeBlock(x, y, blockTypes::WALL);
					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

				}
				else if (level5::level[y * roomWidth + x] == 2) {

					room.changeBlock(x, y, blockTypes::DOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isDoor = true;

				}
				else if (level5::level[y * roomWidth + x] == 3) {

					room.changeBlock(x, y, blockTypes::ENDDOOR);
					room.changeWallTexture(x, y, 1);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					room.getBlock(x, y).isEndDoor = true;

				}
				else {

					if (level5::level[y * roomWidth + x] == -1) {
						player.position = olc::vf2d(x * cubeSize + cubeSize / 2 + 0.001f, y * cubeSize + cubeSize / 2 + 0.001f);
					}
					else if (level5::level[y * roomWidth + x] == -2) {
						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 2, cubeSize * 2, room.getWidth(), room.getHeight()));
						enemies[enemies.size() - 1].textureID = 0;
						enemies[enemies.size() - 1].staticTextureID = 0;
						enemies[enemies.size() - 1].shootTextureID = 1;
						enemies[enemies.size() - 1].deadTextureID = 0;
						enemies[enemies.size() - 1].bulletTextureID = 1;
					}
					else if (level5::level[y * roomWidth + x] == -3) {
						sprites.push_back(Sprite(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize / 2, cubeSize / 2));
						sprites[sprites.size() - 1].textureID = 1;
						sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH2;
					}
					else if (level5::level[y * roomWidth + x] == -4) {

						enemies.push_back(Enemy(x * cubeSize + cubeSize / 2, y * cubeSize + cubeSize / 2, 30, cubeSize * 8, cubeSize * 8, room.getWidth(), room.getHeight()));

						enemies[enemies.size() - 1].health = 5000.0f;
						enemies[enemies.size() - 1].speed = 180.0f;

						enemies[enemies.size() - 1].entityRadius = 100.0f;

						enemies[enemies.size() - 1].fireRate = 0.25f;

						enemies[enemies.size() - 1].textureID = 2;
						enemies[enemies.size() - 1].staticTextureID = 2;
						enemies[enemies.size() - 1].shootTextureID = 3;
						enemies[enemies.size() - 1].deadTextureID = 2;
						enemies[enemies.size() - 1].bulletTextureID = 1;
						enemies[enemies.size() - 1].isBoss = true;

					}


					room.changeWallTexture(x, y, 0);
					room.changeFloorTexture(x, y, 0);
					room.changeCeilTexture(x, y, 0);

					if (y > 54) room.getBlock(x, y).hasCeil = false;

				}

			}
		}

		// mapData

		mapData.cubeSize = cubeSize;

		mapData.width = room.getWidth();
		mapData.height = room.getHeight();

		mapData.map = &room.getMap();

		// guns

		player.gunID = 0;

		loadGuns();

		// render variables

		fov = 60;
		numberOfRays = ScreenWidth();

	}

	void loadButtons() {

		startButton.width = 100;
		startButton.height = 25;
		startButton.isActive = true;
		startButton.position = olc::vi2d(ScreenWidth() / 2, 2 * ScreenHeight() / 5);
		startButton.image = Texture("Resources/Screen/playButton.png");

		exitButton.width = 100;
		exitButton.height = 25;
		exitButton.isActive = true;
		exitButton.position = olc::vi2d(ScreenWidth() / 2, 3 * ScreenHeight() / 5);
		exitButton.image = Texture("Resources/Screen/exitButton.png");


		backButton.width = 50;
		backButton.height = 15;
		backButton.isActive = true;
		backButton.position = olc::vi2d(backButton.width / 2 + 5, backButton.height / 2 + 5);
		backButton.image = Texture("Resources/Screen/backButton.png");

		continueButton.width = 100;
		continueButton.height = 25;
		continueButton.isActive = true;
		continueButton.position = olc::vi2d(ScreenWidth() / 2, 2 * ScreenHeight() / 5);
		continueButton.image = Texture("Resources/Screen/continueButton.png");

		mainMenuButton.width = 100;
		mainMenuButton.height = 25;
		mainMenuButton.isActive = true;
		mainMenuButton.position = olc::vi2d(ScreenWidth() / 2, 3 * ScreenHeight() / 5);
		mainMenuButton.image = Texture("Resources/Screen/mainMenuButton.png");

		levelPreviews[0].width = 50;
		levelPreviews[0].height = 60;
		levelPreviews[0].position = olc::vi2d(ScreenWidth() * 22 / 100, ScreenHeight() / 4);
		levelPreviews[0].image = Texture("Resources/Screen/Level0Preview.png");

		levelPreviews[0].isActive = true;

		levelPreviews[1].width = 50;
		levelPreviews[1].height = 60;
		levelPreviews[1].position = olc::vi2d(ScreenWidth() * 50 / 100, ScreenHeight() / 4);
		
		if (maxLevelReachable >= 1) {
			levelPreviews[1].image = Texture("Resources/Screen/Level1Preview.png");
			levelPreviews[1].isActive = true;
		}
		else {
			levelPreviews[1].image = Texture("Resources/Screen/LevelLock.png");
			levelPreviews[1].isActive = false;
		}

		levelPreviews[2].width = 50;
		levelPreviews[2].height = 60;
		levelPreviews[2].position = olc::vi2d( ScreenWidth() * 77 / 100, ScreenHeight() / 4);

		if (maxLevelReachable >= 2) {
			levelPreviews[2].image = Texture("Resources/Screen/Level2Preview.png");
			levelPreviews[2].isActive = true;
		}
		else {
			levelPreviews[2].image = Texture("Resources/Screen/LevelLock.png");
			levelPreviews[2].isActive = false;
		}

		levelPreviews[3].width = 50;
		levelPreviews[3].height = 60;
		levelPreviews[3].position = olc::vi2d(ScreenWidth() * 30 / 100, ScreenHeight() * 3 / 4);

		if (maxLevelReachable >= 3) {
			levelPreviews[3].image = Texture("Resources/Screen/Level3Preview.png");
			levelPreviews[3].isActive = true;
		}
		else {
			levelPreviews[3].image = Texture("Resources/Screen/LevelLock.png");
			levelPreviews[3].isActive = false;
		}

		levelPreviews[4].width = 50;
		levelPreviews[4].height = 60;
		levelPreviews[4].position = olc::vi2d(ScreenWidth() * 70 / 100, ScreenHeight() * 3 / 4);

		if (maxLevelReachable >= 4) {
			levelPreviews[4].image = Texture("Resources/Screen/Level4Preview.png");
			levelPreviews[4].isActive = true;
		}
		else {
			levelPreviews[4].image = Texture("Resources/Screen/LevelLock.png");
			levelPreviews[4].isActive = false;
		}


	}

public:

	bool OnUserCreate() override {

		startScreenImage = Texture("Resources/Screen/startScreen.png");

		loadData();
		loadButtons();

		return true;
	
	}

	bool OnUserUpdate(float fElapsedTime) override {

		if (GUIScreenIsBeingRendered) {
			drawStartScreen();
			return true;
		}

		if (PauseScreenIsBeingRendered) {
			drawPausedScreen();
			return true;
		}

		globalDeltaTime = fElapsedTime;

		getInputs(fElapsedTime);

		std::vector <SpriteDist> spriteDists;

		roomLogic();

		if (GUIScreenIsBeingRendered) {
			return true;
		}

		if (PauseScreenIsBeingRendered) {
			return true;
		}

		//Clear(olc::Pixel(135, 206, 235));

		drawSky();
		renderScene(fov, numberOfRays);

		for (int k = 0; k < enemies.size(); k++) {

			bool shootBullet = enemies[k].enemyCicle(player, mapData, fElapsedTime);
			spriteDists.push_back(SpriteDist(1, k, enemies[k].x - player.position.x, enemies[k].y - player.position.y));
			
			if (shootBullet) {

				bullets.push_back(Bullet(enemies[k].x, enemies[k].y, 7.0f, 20, 20, 1));

				olc::vf2d ETP = olc::vf2d(player.position.x - enemies[k].x, player.position.y - enemies[k].y);
				float ETPDist = sqrtf(ETP.x * ETP.x + ETP.y * ETP.y);
				
				bullets[bullets.size() - 1].direction = olc::vf2d(ETP.x / ETPDist, ETP.y / ETPDist);
				bullets[bullets.size() - 1].textureID = enemies[k].bulletTextureID;
				
				if (enemies[k].isBoss) {
					bullets[bullets.size() - 1].damage = 15.0f;
					bullets[bullets.size() - 1].speed = 1000.0f;
				}
				else {
					bullets[bullets.size() - 1].damage = 10.0f;
					bullets[bullets.size() - 1].speed = 500.0f;
				}

			}

		}

		for (int k = 0; k < bullets.size(); k++) {

			bool getDestroyed = bullets[k].bulletCicle(player, enemies, mapData, fElapsedTime);

			if (getDestroyed) {

				bullets.erase(bullets.begin() + k);
				k--;

			}
			else {
				spriteDists.push_back(SpriteDist(2, k, bullets[k].x - player.position.x, bullets[k].y - player.position.y));
				//	drawSprite(bullets[k]);
			}

		}

		for (int k = 0; k < enemies.size(); k++) {
			
			if (enemies[k].health <= 0) {

				sprites.push_back(Sprite(enemies[k].x, enemies[k].y, enemies[k].z, enemies[k].width, enemies[k].height));
				sprites[sprites.size() - 1].textureSectionID = 0;
				sprites[sprites.size() - 1].textureID = enemies[k].deadTextureID;
				sprites[sprites.size() - 1].powerUpType = powerUpTypes::HEALTH1;

				enemies.erase(enemies.begin() + k);
				k--;

			}

		}

		for (int k = 0; k < sprites.size(); k++) {

			if (sprites[k].powerUpType != powerUpTypes::NOPWR) {

				float pToSdx = player.position.x - sprites[k].x;
				float pToSdy = player.position.y - sprites[k].y;

				if (player.entityRadius * player.entityRadius * 4 >= pToSdx * pToSdx + pToSdy * pToSdy) {
					
					if (player.health != player.maxHealth) {

						PlaySound(TEXT("Resources/Sounds/pickup.wav"), NULL, SND_ASYNC);

						if (sprites[k].powerUpType == powerUpTypes::HEALTH1) {
							player.health += 5;
						}

						if (sprites[k].powerUpType == powerUpTypes::HEALTH2) {
							player.health += 20;
						}

						if (sprites[k].powerUpType == powerUpTypes::HEALTH3) {
							player.health += 30;
						}

						player.health = std::min(player.maxHealth, player.health);

					}

					if (sprites[k].powerUpType == powerUpTypes::GUN2) {
						PlaySound(TEXT("Resources/Sounds/pickup.wav"), NULL, SND_ASYNC);
						gunUnlocked[1] = true;
						player.gunID = 1;
					}

					if (sprites[k].powerUpType == powerUpTypes::GUN3) {
						PlaySound(TEXT("Resources/Sounds/pickup.wav"), NULL, SND_ASYNC);
						gunUnlocked[2] = true;
						player.gunID = 2;
					}

					sprites.erase(sprites.begin() + k);
					k--;
					continue;

				}

			}

			spriteDists.push_back(SpriteDist(0, k, sprites[k].x - player.position.x, sprites[k].y - player.position.y));
		
		}

		std::sort(spriteDists.begin(), spriteDists.end());
		std::reverse(spriteDists.begin(), spriteDists.end());

		int itr = 0;

		for (const auto& i : spriteDists) {

			if (i.type == 0) {
				drawSprite(sprites[i.idx]);
		//		std::cout << ++itr << ' ';
			}
			else if (i.type == 1) {
				drawSprite(enemies[i.idx]);
			}
			else if (i.type == 2) {
				drawSprite(bullets[i.idx]);
			}

		}

		renderGui();

		if (player.health <= 0) {
			GAMEOVER();
		}

		return true;
	}
};

void hideConsole() {
	HWND Stealth;
	AllocConsole();
	Stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(Stealth, 0);
}

int main() {

	hideConsole();

	Example demo;

	if (demo.Construct(screenWidth, screenheight, 5, 5))
		demo.Start();

	return 0;

}
