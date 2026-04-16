#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cmath>

#include <chrono>
#include <thread>



using namespace std;
using namespace std::chrono;

// --- Global input queue ---
#include <atomic>
#include <queue>

#ifdef _WIN32
#include <conio.h> // Windows non-blocking input
#else
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#endif

std::queue<char> inputQueue;
std::atomic<bool> running(true);

char getInputNonBlocking() {
    #ifdef _WIN32
    if (_kbhit()) return _getch(); // Returns key immediately
    return 0;
    #else
    struct termios oldt, newt;
    char ch = 0;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);       // Raw mode
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // Non-blocking read
    read(STDIN_FILENO, &ch, 1);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
    #endif
}

// Thread to read input
void inputThread() {
    while (running) {
        char c = getInputNonBlocking();
        if (c != 0) {
            inputQueue.push(c);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // prevent CPU hog
    }
}
int xResolution = 480;
int yResolution = 320;
int xStartingPoint;
int yEndingPoint;

class Tetromino
{
public:
    uint8_t key;
    bool pieceSet[4][16];
    uint8_t squareSprite[64];
};

const Tetromino Bar = {
    0,
    {
        {
            0,0,0,0,
            1,1,1,1,
            0,0,0,0,
            0,0,0,0},

            {
                0,0,1,0,
                0,0,1,0,
                0,0,1,0,
                0,0,1,0},

                {
                    0,0,0,0,
                    1,1,1,1,
                    0,0,0,0,
                    0,0,0,0},

                    {
                        0,0,1,0,
                        0,0,1,0,
                        0,0,1,0,
                        0,0,1,0}
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};
const Tetromino L = {
    1,
    {
        {
            0,0,0,0,
            1,1,1,0,
            1,0,0,0,
            0,0,0,0},

            {
                1,1,0,0,
                0,1,0,0,
                0,1,0,0,
                0,0,0,0},

                {
                    0,0,0,0,
                    0,0,1,0,
                    1,1,1,0,
                    0,0,0,0},

                    {
                        0,1,0,0,
                        0,1,0,0,
                        0,1,1,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,2,1,1,1,1,1,0,
        0,2,2,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};
const Tetromino Lm = {
    2,
    {
        {
            0,0,0,0,
            1,1,1,0,
            0,0,1,0,
            0,0,0,0},

            {
                0,1,1,0,
                0,1,0,0,
                0,1,0,0,
                0,0,0,0},

                {
                    1,0,0,0,
                    1,1,1,0,
                    0,0,0,0,
                    0,0,0,0},

                    {
                        0,1,0,0,
                        0,1,0,0,
                        1,1,0,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,2,2,0,1,0,
        0,1,0,2,2,0,1,0,
        0,2,0,0,0,0,1,0,
        0,2,2,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};

const Tetromino T = {
    3,
    {
        {
            0,0,0,0,
            1,1,1,0,
            0,1,0,0,
            0,0,0,0},

            {
                0,1,0,0,
                0,1,1,0,
                0,1,0,0,
                0,0,0,0},

                {
                    0,1,0,0,
                    1,1,1,0,
                    0,0,0,0,
                    0,0,0,0},

                    {
                        0,1,0,0,
                        1,1,0,0,
                        0,1,0,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,2,2,0,1,0,
        0,1,0,2,2,0,1,0,
        0,2,0,0,0,0,1,0,
        0,2,2,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};
const Tetromino Z = {
    4,
    {
        {
            0,0,0,0,
            1,1,0,0,
            0,1,1,0,
            0,0,0,0},

            {
                0,1,0,0,
                1,1,0,0,
                1,0,0,0,
                0,0,0,0},

                {
                    0,0,0,0,
                    1,1,0,0,
                    0,1,1,0,
                    0,0,0,0},

                    {
                        0,1,0,0,
                        1,1,0,0,
                        1,0,0,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,2,2,0,
        0,1,1,1,1,1,2,0,
        0,1,1,0,0,1,2,0,
        0,1,1,0,0,1,1,0,
        0,1,1,1,1,1,1,0,
        0,1,1,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};

const Tetromino Zm = {
    5,
    {
        {
            0,0,0,0,
            0,1,1,0,
            1,1,0,0,
            0,0,0,0},

            {
                1,0,0,0,
                1,1,0,0,
                0,1,0,0,
                0,0,0,0},

                {
                    0,0,0,0,
                    0,1,1,0,
                    1,1,0,0,
                    0,0,0,0},

                    {
                        1,0,0,0,
                        1,1,0,0,
                        0,1,0,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,2,2,0,1,0,
        0,1,0,2,2,0,1,0,
        0,1,0,0,0,0,2,0,
        0,1,1,1,1,2,2,0,
        0,0,0,0,0,0,0,0
    }
};
const Tetromino S = {
    6,
    {
        {
            0,0,0,0,
            0,1,1,0,
            0,1,1,0,
            0,0,0,0},

            {
                0,0,0,0,
                0,1,1,0,
                0,1,1,0,
                0,0,0,0},

                {
                    0,0,0,0,
                    0,1,1,0,
                    0,1,1,0,
                    0,0,0,0},

                    {
                        0,0,0,0,
                        0,1,1,0,
                        0,1,1,0,
                        0,0,0,0},
    },

    {
        0,0,0,0,0,0,0,0,
        0,1,1,1,1,1,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,0,
        0,1,0,0,0,0,1,0,
        0,1,1,1,1,1,1,0,
        0,0,0,0,0,0,0,0
    }
};
Tetromino Tetrominos[7];

uint8_t FPS = 60;
uint8_t tetrisMap[10][18];

int8_t xPos = 3;
int8_t yPos = 0;
int8_t rotation = 0;
uint8_t frameFallCount = 30;

uint8_t currentIndex;
uint8_t nextIndex;


unsigned long lastTime = 0;
float frameCount = 0;
float dropFrameSpeed;
int score = 0;
uint8_t level = 1;
int linesPlayed=0;
int linesForLevel = 10;

void clearMap()
{
    for (uint8_t y = 0; y < size(tetrisMap[0]); y++)          // height
        for (uint8_t x = 0; x < size(tetrisMap); x++)       // width
        {
            tetrisMap[x][y] = 0;
        }
}

void SetPiece(uint8_t &index)
{
    index = rand() % 7;
}

void displayMap()
{
    for (uint8_t y = 0; y<size(tetrisMap[0]); y++)
    {
        for (uint8_t x = 0; x<size(tetrisMap); x++)
        {
            if (tetrisMap[x][y] > 0)
            {
                cout << '#' ;
            }
            else
            {
                cout << ".";
            }

            //cout << to_string(tetrisMap[x][y]);

        }

        cout << endl;
    }

    cout << "Score: " << score << endl;
    cout << "Level: " << to_string(level) << endl<< endl;
    cout << "Next Piece:" << endl;
    for (uint8_t y = 0; y<4; y++)
    {
        for (uint8_t x = 0; x<4; x++)
        {
            if (Tetrominos[nextIndex].pieceSet[0][x+y*4] > 0)
            {
                cout << '#' ;
            }
            else
            {
                cout << ".";
            }

        }
        cout << endl;
    }
}
bool IsPostionOutMap(int8_t x, int8_t y)
{
    return (x < 0 || x >= size(tetrisMap) || y < 0 || y >= size(tetrisMap[0]));
}

void DisplayCell(uint8_t xGrid, uint8_t yGrid, Tetromino piece)
{
    uint8_t xx = xGrid*8+xStartingPoint;
    uint8_t yy = yGrid*8+yEndingPoint;

    for (uint8_t y = 0; y<8; y++)
        for (uint8_t x = 0; x<8; x++)
        {
            switch(piece.squareSprite[x+y*8])
            {

            }

        }
}
void ClearCell(uint8_t xGrid, uint8_t yGrid)
{
    uint8_t xx = xGrid*8+xStartingPoint;
    uint8_t yy = yGrid*8+yEndingPoint;

    for (uint8_t y = 0; y<8; y++)
        for (uint8_t x = 0; x<8; x++)
        {

        }
}
void RenderPieceToMap(Tetromino piece, uint8_t blockIndex)
{
    for (uint8_t y = 0; y<4; y++)
        for (uint8_t x = 0; x<4; x++)
        {
            if (IsPostionOutMap(x+xPos,y+yPos) || tetrisMap[x+xPos][y+yPos] > 2)
                continue;
            uint8_t cell = piece.pieceSet[rotation][x+y*4] * blockIndex;

            // if (cell == 2)
            //     DisplayCell(x+xPos,y+yPos, piece);
            // else if (tetrisMap[x+xPos][y+yPos] == 2)
            //     ClearCell(x+xPos,y+yPos);

            tetrisMap[x+xPos][y+yPos] = cell;//2 is moving piece

        }
}
bool newPiece=false;


void UpdateRegion(uint8_t switching, uint8_t switchTo, int8_t xOffset, int8_t yOffset)
{
    for (uint8_t y = 0; y<4; y++)
        for (uint8_t x = 0; x<4; x++)
        {
            if (IsPostionOutMap(x+xOffset+xPos, y+yOffset+yPos))
                continue;
            if (tetrisMap[x+xOffset+xPos][y+yOffset+yPos] == switching)
            {
                //ClearCell(x+xOffset+xPos,y+yOffset+yPos);//I Only use it to change Old moving piece to nothing
                tetrisMap[x+xOffset+xPos][y+yOffset+yPos] = switchTo;

            }

        }
}

bool IsOverlap(Tetromino piece)
{
    for (uint8_t y = 0; y<4; y++)
        for (uint8_t x = 0; x<4; x++)
        {
            if (piece.pieceSet[rotation][x+y*4] == 1)
            {
                if (IsPostionOutMap(x+xPos, y+yPos) || tetrisMap[x+xPos][y+yPos]>2)
                {
                    return true;
                }

            }
        }
        return false;
}





void LevelUp()
{
    level++;
    dropFrameSpeed = pow(0.8f-((level-1)*0.007f),level-1)*60;
    linesForLevel = 10*level;
    linesPlayed = 0;
}
void AddScore(int Lines)
{
    uint8_t lineScore = 0;
    switch(Lines)
    {
        case(0): return;
        case(1): lineScore = 4;  break;
        case(2): lineScore = 10; break;
        case(3): lineScore = 30; break;
        case(4): lineScore = 120;break;
    }

    score += lineScore * 10 * (level+1);
    linesPlayed += Lines;
    if (linesPlayed >= linesForLevel)
    {
        LevelUp();
    }
}
void ClearIfLines()
{
    bool isLine = true;
    int Lines = 0;
    for (uint8_t y = 0; y<4; y++)
    {
        for (uint8_t x = 0; x<10; x++)
        {
            if (tetrisMap[x][y+yPos] < 3)
            {
                isLine = false;
                break;
            }
        }
        if (isLine)
        {
            for (uint8_t x = 0; x < 10; x++)
                tetrisMap[x][y + yPos] = 0;

            for (uint8_t yy = y + yPos; yy > 0; yy--)
            {
                for (uint8_t x = 0; x < 10; x++)
                {
                    if (tetrisMap[x][yy] != tetrisMap[x][yy - 1])
                    {
                        if (tetrisMap[x][yy-1] == 0)
                        {
                            ClearCell(x, yy);
                        }
                        else
                        {
                            DisplayCell(x,yy, Tetrominos[tetrisMap[x][yy - 1] - 3]);
                        }
                        tetrisMap[x][yy] = tetrisMap[x][yy - 1];
                    }
                }
            }


            for (uint8_t x = 0; x < 10; x++)
            {
                if (tetrisMap[x][0] != 0)
                {
                    tetrisMap[x][0] = 0;
                    ClearCell(x, 0);
                }
            }


            y--;

            Lines++;
        }
        isLine = true;

        AddScore(Lines);
    }
}


void PlacePiece()
{

    RenderPieceToMap(Tetrominos[currentIndex], Tetrominos[currentIndex].key+3);
    ClearIfLines();

    rotation = 0;
    frameCount = 0;
    xPos = 3;
    yPos =0;


    currentIndex = nextIndex;
    SetPiece(nextIndex);
    RenderPieceToMap(Tetrominos[currentIndex], 2);


    if (IsOverlap(Tetrominos[currentIndex]))
    {

    }
    score++;
}

void GoDown()
{

    UpdateRegion(2, 0, 0, 0);
    yPos++;
    if (IsOverlap(Tetrominos[currentIndex]))
    {
        yPos--;
        PlacePiece();
    }


}


void Loop()
{
    frameCount ++;

    if (frameCount > dropFrameSpeed)
    {
        frameCount -= dropFrameSpeed;
        GoDown();
    }

    #ifdef _WIN32
        system("echo \033[1;1H");
    #else
        system("clear");
    #endif

    RenderPieceToMap(Tetrominos[currentIndex],2);
    displayMap();

}




void MoveHorizontal(int8_t direction)
{
    xPos += direction;
    if (IsOverlap(Tetrominos[currentIndex]))
    {
        xPos -= direction;
        return;
    }

    UpdateRegion(2, 0, -direction, 0);
}

void RotateCCW()
{
    int8_t backRotation = rotation;
    rotation++;
    rotation = rotation%4;
    if (IsOverlap(Tetrominos[currentIndex]))
    {
        rotation = backRotation;
        return;
    }


    UpdateRegion(2, 0, 0, 0);
}

void RotateCW()
{
    int8_t backRotation = rotation;
    rotation--;
    if (rotation < 0)
        rotation = 3;
    if (IsOverlap(Tetrominos[currentIndex]))
    {
        rotation = backRotation;
        return;
    }


    UpdateRegion(2, 0, 0, 0);

}

int main()
{
    Tetrominos[0] = Bar;
    Tetrominos[1] = L;
    Tetrominos[2] = Lm;
    Tetrominos[3] = T;
    Tetrominos[4] = Z;
    Tetrominos[5] = Zm;
    Tetrominos[6] = S;

    thread t(inputThread);

    srand(time(0));
    clearMap();
    displayMap();


    SetPiece(currentIndex);
    SetPiece(nextIndex);

    dropFrameSpeed = pow(0.8f-((level-1)*0.007f),level-1)*60;
    //addPieceToMap(Tetrominos[currentIndex]);

    RenderPieceToMap(Tetrominos[currentIndex],2);

    //cout << Tetrominos[currentIndex].pieceSet[1];
    const auto frameDuration = milliseconds(1000 / FPS);

    while (true) {
        auto frameStart = high_resolution_clock::now();
        // --- update logic ---
        frameCount++;
        while (!inputQueue.empty()) {
            char c = inputQueue.front();
            inputQueue.pop();

            if (c == 'a') MoveHorizontal(-1);
            if (c == 'd') MoveHorizontal(1);
            if (c == 's') GoDown();
            if (c == 'e') RotateCW();
            if (c == 'q') RotateCCW();
            if (c == 'p') { running = false; t.join(); return 0; }
        }

        Loop();

        auto frameEnd = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(frameEnd - frameStart);

        if (elapsed < frameDuration)
            this_thread::sleep_for(frameDuration - elapsed); // maintain FPS
    }
    running = false;
    t.join();
    return 0;
}
