#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <conio.h>   
using namespace std;

// ##########移动（撤回）###############
const int STACK_MAX = 200;
struct Position {
    int x, y;
};

//##################神秘人物?！###################
const string Kitty[4] = {
	"^   ^    --------------",
	"(^ w ^) < try your best!",
	"/|   |\\   --------------",
	"  | | \\"
}; 
struct PathStack {
    Position data[STACK_MAX];
    int top = -1;

    void push(int x, int y) {
        if (top < STACK_MAX - 1) {
            top++;
            data[top].x = x;
            data[top].y = y;
        }
    }

    bool pop(Position &p) {
        if (top >= 0) {
            p = data[top];
            top--;
            return true;
        }
        return false;
    }

    bool empty() const { return top == -1; }
};

// ###################寻路 #################
const int QUEUE_MAX = 400;
struct BFSQueue {
    Position data[QUEUE_MAX];
    int front = 0, rear = 0;

    void enqueue(int x, int y) {
        data[rear] = {x, y};// =  data[rear].x = x......
        rear = (rear + 1) % QUEUE_MAX;
    }

    Position dequeue() {
        Position p = data[front];
        front = (front + 1) % QUEUE_MAX;
        return p;
    }

    bool empty() const { return front == rear; }
};

// ################ 地图 =#################
const int ROWS = 9, COLS = 20;
string map[ROWS] = {
    "####################",
    "#  S   ##  K   ####",
    "#      ##      ####",
    "#  ##  ##  ##  ####",
    "#  ##      ##  ####",
    "#  ######  ##  ####",
    "#          ##  ####",
    "##########  ##  ####",
    "#  K       ##  E###"
};
int playerX = 1, playerY = 2;   // S位置
int keys = 0;
const int TOTAL_KEYS = 2;
PathStack pathStack;
vector<string> msgLog;   // 提示信息

// ################### 屏幕显示 #################
void render() {
    system("cls");  // 清屏 
    cout << "  迷宫探索 （栈+队列)      注视中-w-\n";
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == playerY && j == playerX)
                cout << '@';
            else if (map[i][j] == 'S' || map[i][j] == 'E')
                cout << ' ';
            else
                cout << map[i][j];
        }
        cout << "        ";
        if(i < 4) cout << Kitty[i];
        cout << '\n';
    }
    cout << "  钥匙: " << keys << "/" << TOTAL_KEYS;
    cout << "  你是“@” |按下WASD移动，U撤回，H提示，Q退出\n";
    if (!msgLog.empty()) {
        cout << "  ---\n";
        for (const string &msg : msgLog) cout << "  " << msg << '\n';
        msgLog.clear();
    }
}

// ################### 移动逻辑 ###############
void tryMove(int dx, int dy) {
    int nx = playerX + dx, ny = playerY + dy;
    if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) return;
    char cell = map[ny][nx];
    if (cell == '#') return;
    // 钥匙
    if (cell == 'K') {
        keys++;
        map[ny][nx] = ' ';
        msgLog.push_back("钥匙++ ！");
    }
    // 记录当前位置到栈
    pathStack.push(playerX, playerY);
    playerX = nx;
    playerY = ny;
}
// 撤回 
void undoMove() {
    Position p;
    if (pathStack.pop(p)) {
        playerX = p.x;
        playerY = p.y;
        msgLog.push_back("已撤回上一步");
    } else {
        msgLog.push_back("没有可撤回的步骤");
    }
}

// ############### BFS 寻路提示 ################
void showHint() {
    bool visited[ROWS][COLS] = {false};
    int parentX[ROWS][COLS], parentY[ROWS][COLS];
    BFSQueue q;

    q.enqueue(playerX, playerY);
    visited[playerY][playerX] = true;

    int dirs[4][2] = {{0,1},{0,-1},{1,0},{-1,0}};
    int endX = -1, endY = -1;

    while (!q.empty()) {
        Position cur = q.dequeue();
        if (map[cur.y][cur.x] == 'E') {
            endX = cur.x; endY = cur.y;
            break;
        }
        for (auto &d : dirs) {
            int nx = cur.x + d[0], ny = cur.y + d[1];
            if (nx < 0 || nx >= COLS || ny < 0 || ny >= ROWS) continue;
            if (!visited[ny][nx] && map[ny][nx] != '#') {
                visited[ny][nx] = true;
                parentX[ny][nx] = cur.x;
                parentY[ny][nx] = cur.y;
                q.enqueue(nx, ny);
            }
        }
    }

    if (endX != -1) {
        // 回溯
        Position backStack[QUEUE_MAX];
        int backTop = -1;
        int cx = endX, cy = endY;
        while (!(cx == playerX && cy == playerY)) {
            backStack[++backTop] = {cx, cy};
            int px = parentX[cy][cx];
            int py = parentY[cy][cx];
            cx = px; cy = py;
        }
        msgLog.push_back("BFS最短路径（从起点到终点）：");
        string pathStr;
        while (backTop >= 0) {
            pathStr += "(" + to_string(backStack[backTop].x) + "," +
                       to_string(backStack[backTop].y) + ") ";
            backTop--;
        }
        msgLog.push_back(pathStr);
    } else {
        msgLog.push_back("无法到达终点！");
    }
}

// ################# main主循环 ############
int main() {
    pathStack.push(playerX, playerY); // 初始位置入栈
    char cmd;

    while (true) {
        render();
        if (map[playerY][playerX] == 'E') {
            cout << "\n  恭喜！你成功逃出了迷宫！\n";
            break;
        }

        cmd = _getch();  //。 
        cmd = tolower(cmd);

        switch (cmd) {
            case 'w': tryMove(0, -1); break;
            case 's': tryMove(0, 1); break;
            case 'a': tryMove(-1, 0); break;
            case 'd': tryMove(1, 0); break;
            case 'u': undoMove(); break;
            case 'h': showHint(); break;
            case 'q': return 0;
        }
    }
    return 0;
}
