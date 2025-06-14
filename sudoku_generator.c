#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define N 9
#define PUZZLE_COUNT 30
#define EASY 0
#define MEDIUM 1
#define HARD 2

// 產生完整解
int isValid(int board[N][N], int row, int col, int num) {
    for (int x = 0; x < N; x++)
        if (board[row][x] == num || board[x][col] == num)
            return 0;
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            if (board[i + startRow][j + startCol] == num)
                return 0;
    return 1;
}

int fillBoard(int board[N][N], int pos) {
    if (pos == N*N) return 1;
    int row = pos / N, col = pos % N;
    if (board[row][col] != 0) return fillBoard(board, pos+1);
    int nums[N];
    for (int i=0;i<N;i++) nums[i]=i+1;
    for (int i=N-1;i>0;i--) {
        int j = rand() % (i+1);
        int t = nums[i]; nums[i]=nums[j]; nums[j]=t;
    }
    for (int i=0;i<N;i++) {
        if (isValid(board, row, col, nums[i])) {
            board[row][col]=nums[i];
            if (fillBoard(board, pos+1)) return 1;
            board[row][col]=0;
        }
    }
    return 0;
}

// 解題計數（用於唯一解驗證）
int solveCount;
int solveSudoku(int board[N][N], int pos) {
    if (pos == N*N) { solveCount++; return solveCount>1?1:0; }
    int row = pos/N, col = pos%N;
    if (board[row][col]!=0) return solveSudoku(board,pos+1);
    for (int num=1;num<=N;num++) {
        if (isValid(board,row,col,num)) {
            board[row][col]=num;
            if (solveSudoku(board,pos+1)) return 1;
            board[row][col]=0;
        }
    }
    return 0;
}

// 隨機挖空，並驗證唯一解
void makePuzzle(int src[N][N], int dest[N][N], int blanks) {
    memcpy(dest, src, sizeof(int)*N*N);
    int tries = 0;
    while (tries < N*N*2 && blanks > 0) {
        int r = rand()%N, c = rand()%N;
        if (dest[r][c]==0) { tries++; continue; }
        int backup = dest[r][c];
        dest[r][c]=0;
        int tmp[N][N]; memcpy(tmp, dest, sizeof(tmp));
        solveCount=0;
        solveSudoku(tmp,0);
        if (solveCount!=1) dest[r][c]=backup; // 不是唯一解就還原
        else blanks--;
        tries++;
    }
}

void printPuzzle(int board[N][N]) {
    for (int i=0;i<N;i++) {
        for (int j=0;j<N;j++) printf("%d ", board[i][j]);
        printf("\n");
    }
}

int main() {
    srand(time(NULL));
    int easy=0, medium=0, hard=0;
    int easyList[PUZZLE_COUNT][N][N], mediumList[PUZZLE_COUNT][N][N], hardList[PUZZLE_COUNT][N][N];
    for (int p=0; p<PUZZLE_COUNT; ) {
        int full[N][N]={0}, puzzle[N][N];
        fillBoard(full,0);
        int blanks = 30 + rand()%21; // 30~50
        makePuzzle(full, puzzle, blanks);
        // 計算空格
        int cnt=0;
        for (int i=0;i<N;i++) for (int j=0;j<N;j++) if (puzzle[i][j]==0) cnt++;
        // 分難度
        int level;
        if (cnt<=36) level=EASY;
        else if (cnt<=43) level=MEDIUM;
        else level=HARD;
        // 檢查重複
        int dup=0;
        for (int k=0;k<p;k++) {
            int same=1;
            for (int i=0;i<N;i++) for (int j=0;j<N;j++) if (puzzle[i][j]!=easyList[k][i][j] && puzzle[i][j]!=mediumList[k][i][j] && puzzle[i][j]!=hardList[k][i][j]) { same=0; break; }
            if (same) { dup=1; break; }
        }
        if (dup) continue;
        // 存入
        if (level==EASY && easy<PUZZLE_COUNT/3) { memcpy(easyList[easy++],puzzle,sizeof(puzzle)); p++; }
        else if (level==MEDIUM && medium<PUZZLE_COUNT/3) { memcpy(mediumList[medium++],puzzle,sizeof(puzzle)); p++; }
        else if (level==HARD && hard<PUZZLE_COUNT/3) { memcpy(hardList[hard++],puzzle,sizeof(puzzle)); p++; }
    }
    // 輸出
    printf("\n=== EASY ===\n");
    for (int i=0;i<PUZZLE_COUNT/3;i++) printPuzzle(easyList[i]), printf("\n");
    printf("\n=== MEDIUM ===\n");
    for (int i=0;i<PUZZLE_COUNT/3;i++) printPuzzle(mediumList[i]), printf("\n");
    printf("\n=== HARD ===\n");
    for (int i=0;i<PUZZLE_COUNT/3;i++) printPuzzle(hardList[i]), printf("\n");
    return 0;
}
