#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cstdint>

#include <chrono>
#include <thread>


#include <termios.h>
#include <unistd.h>
#include <sys/select.h>


using namespace std;
using namespace std::chrono;

termios oldt;
void initInput()
{
    termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO); // no Enter, no echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void restoreInput()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

bool kbhit()
{
    timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}
class Tetromino
{
    public:
        bool pieceSet[4][16];
        uint8_t squareSprite[64];
};

const Tetromino Bar = {

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

uint8_t FPS = 60;
uint8_t tetrisMap[10][18];

int8_t xPos = 3;
int8_t yPos = 0;
int8_t rotation = 0;
uint8_t frameFallCount = 30;

Tetromino currentPiece;
Tetromino nextPiece;

void clearMap()
{
    for (uint8_t y = 0; y < size(tetrisMap[0]); y++)          // height
    for (uint8_t x = 0; x < size(tetrisMap); x++)       // width
    {
        tetrisMap[y][x] = 0;
    }
}

void SetPiece(Tetromino*point)
{
    int random =  rand()%7;

    switch(random)
    {
        case(0):*point = Bar;
            break;
        case(1):*point = L;
            break;
        case(2):*point = Lm;
            break;
        case(3):*point = Z;
            break;
        case(4):*point = Zm;
            break;
        case(5):*point = T;
            break;
        case(6):*point = S;
            break;
    }
}

void displayMap()
{
    for (uint8_t y = 0; y<size(tetrisMap[0]); y++)
    {
        for (uint8_t x = 0; x<size(tetrisMap); x++)
        {
            cout << (tetrisMap[x][y] ? '#' : '.');
        }

        cout << endl;
    }
}
bool IsPostionOutMap(uint8_t x, uint8_t y)
{
    return (x < 0 || x >= size(tetrisMap) || y < 0 || y >= size(tetrisMap[0]));
}

void RenderPieceToMap(Tetromino piece, uint8_t blockIndex)
{
    for (uint8_t y = 0; y<4; y++)
    for (uint8_t x = 0; x<4; x++)
    {
        if (IsPostionOutMap(x+xPos,y+yPos) || tetrisMap[x+xPos][y+yPos] != 0)
           continue;
        tetrisMap[x+xPos][y+yPos] = piece.pieceSet[rotation][x+y*4] * blockIndex;//2 is moving piece
    }
}
uint8_t frameCount =0;
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
            if (IsPostionOutMap(x+xPos, y+yPos) || tetrisMap[x+xPos][y+yPos]==1)
            {
                return true;
            }
            
        }    
    }
    return false;
}

void ClearIfLines() 
{ 
    bool isLine = true; 
    for (uint8_t y = 0; y<4; y++) 
    { 
        for (uint8_t x = 0; x<10; x++) 
        { 
            if (tetrisMap[x][y+yPos] != 1) 
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
                    tetrisMap[x][yy] = tetrisMap[x][yy - 1];
            }


            for (uint8_t x = 0; x < 10; x++)
                tetrisMap[x][0] = 0;

            y--; 
        }
        isLine = true; 
    } 
}
void loop()
{
    ClearIfLines();
    system("clear");

    RenderPieceToMap(currentPiece,2);
    displayMap();
    
}
void PlacePiece()
{
    RenderPieceToMap(currentPiece, 1);
    ClearIfLines();

    rotation = 0;
    frameCount = 0;
    xPos = 3;
    yPos =0;
    

    currentPiece = nextPiece;
    SetPiece(&nextPiece);
    RenderPieceToMap(currentPiece, 2);


    if (IsOverlap(currentPiece))
    {
        
    }
    loop();
}

void GoDown()
{
    UpdateRegion(2, 0, 0, 0);
    yPos++;
    if (IsOverlap(currentPiece))
    {
        yPos--;
        PlacePiece();
    }
    
    
}

void MoveHorizontal(int8_t direction)
{
    xPos += direction;
    if (IsOverlap(currentPiece))
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
    if (IsOverlap(currentPiece))
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
    if (IsOverlap(currentPiece))
    {
        rotation = backRotation;
        return;
    }

    
    UpdateRegion(2, 0, 0, 0);

}

int main()
{
    srand(time(0));
    clearMap();
    displayMap();

    SetPiece(&currentPiece);
    SetPiece(&nextPiece);

    //addPieceToMap(currentPiece);

    //cout << currentPiece.pieceSet[1];
    const auto frameDuration = milliseconds(1000 / FPS);

    initInput();

    while (true) {
        auto frameStart = high_resolution_clock::now();
        // --- update logic ---
        frameCount++;
        if (kbhit())
        {
            char c;
            read(STDIN_FILENO, &c, 1);

            if (c == 'a') MoveHorizontal(-1);
            if (c == 'd') MoveHorizontal(1);
            if (c == 's') GoDown();
            if (c == 'e') RotateCW();
            if (c == 'q') RotateCCW();
            if (c == 'p') break; 
        }
        if (frameCount > frameFallCount)
        {
            GoDown();
            frameCount = 0;
        }
        
        loop();

        auto frameEnd = high_resolution_clock::now();
        auto elapsed = duration_cast<milliseconds>(frameEnd - frameStart);

        if (elapsed < frameDuration)
            this_thread::sleep_for(frameDuration - elapsed); // maintain FPS
    }
    cin.get();
    return 0;
}