#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "sudoku_puzzles.h"

#define COLOR_RESET   "\033[0m"
#define COLOR_FRAME   "\033[1;36m" // 亮藍色
#define COLOR_ORIGIN  "\033[1;37m" // 亮白色
#define COLOR_INPUT   "\033[1;33m" // 亮黃色
#define COLOR_ROWNUM  "\033[1;32m" // 亮綠色

// 數獨盤面（0 表示空格）
int board[9][9] = {
    {0, 0, 0, 0, 0, 0, 0, 9, 0},
    {1, 9, 0, 4, 7, 0, 6, 0, 8},
    {0, 5, 2, 8, 1, 9, 4, 0, 7},
    {2, 0, 0, 0, 4, 8, 0, 0, 0},
    {0, 0, 9, 0, 0, 0, 5, 0, 0},
    {0, 0, 0, 7, 5, 0, 0, 0, 9},
    {9, 0, 7, 3, 6, 4, 1, 8, 0},
    {5, 0, 6, 0, 8, 1, 0, 7, 4},
    {0, 8, 0, 0, 0, 0, 0, 0, 0}
};

// 遊戲用全域變數
int player_board[9][9];
int answer_board[9][9];
int original_board[9][9];
int error_count = 0;

// 印出盤面
void print_board(int board[][9]) {
    // 新框架設計，確保對齊
    printf("\n    1  2  3   4  5  6   7  8  9\n");
    printf(COLOR_FRAME"  ╔═════════════╦═════════════╦═════════════╗"COLOR_RESET"\n");
    for (int i = 0; i < 9; i++) {
        printf(COLOR_ROWNUM"%2d"COLOR_RESET COLOR_FRAME"║"COLOR_RESET, i+1);
        for (int j = 0; j < 9; j++) {
            if (j != 0 && j % 3 == 0) printf(COLOR_FRAME"║"COLOR_RESET);
            if (board[i][j] == 0) printf("  _ ");
            else if (original_board[i][j] != 0) {
                printf(COLOR_ORIGIN" %2d "COLOR_RESET, board[i][j]);
            } else {
                printf(COLOR_INPUT" %2d "COLOR_RESET, board[i][j]);
            }
        }
        printf(COLOR_FRAME"║"COLOR_RESET"\n");
        if (i == 2 || i == 5) {
            printf(COLOR_FRAME"  ╠═════════════╬═════════════╬═════════════╣"COLOR_RESET"\n");
        }
    }
    printf(COLOR_FRAME"  ╚═════════════╩═════════════╩═════════════╝"COLOR_RESET"\n");
}

// 數字有效性檢查
int isValid(int number, int puzzle[][9], int row, int col) {
    int rowStart = (row / 3) * 3;
    int colStart = (col / 3) * 3;
    for (int i = 0; i < 9; i++) {
        if (puzzle[row][i] == number) return 0;
        if (puzzle[i][col] == number) return 0;
        if (puzzle[rowStart + (i / 3)][colStart + (i % 3)] == number) return 0;
    }
    return 1;
}

// 遞迴解數獨
int solve(int puzzle[][9], int pos) {
    if (pos == 81) return 1;
    int row = pos / 9, col = pos % 9;
    if (puzzle[row][col] != 0) return solve(puzzle, pos + 1);
    for (int num = 1; num <= 9; num++) {
        if (isValid(num, puzzle, row, col)) {
            puzzle[row][col] = num;
            if (solve(puzzle, pos + 1)) return 1;
            puzzle[row][col] = 0;
        }
    }
    return 0;
}

// 遊戲初始化
void init_game() {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            original_board[i][j] = board[i][j];
            player_board[i][j] = board[i][j];
            answer_board[i][j] = board[i][j];
        }
    }
    solve(answer_board, 0);
    error_count = 0;
}

// 檢查是否完成
int is_complete() {
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (player_board[i][j] == 0) return 0;
    return 1;
}

// 計算剩餘空格
int count_empty_cells(int board[][9]) {
    int cnt = 0;
    for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++)
            if (board[i][j] == 0) cnt++;
    return cnt;
}

// 處理玩家輸入
int handle_input() {
    int row, col, num;
    printf("請輸入 行 列 數字 (1-9)，或輸入 0 0 0 結束遊戲: ");
    int ret = scanf("%d %d %d", &row, &col, &num);
    while (ret != 3) {
        while (getchar() != '\n');
        printf("格式錯誤，請重新輸入 行 列 數字: ");
        ret = scanf("%d %d %d", &row, &col, &num);
    }
    if (row == 0 && col == 0 && num == 0) return -1;
    if (row < 1 || row > 9 || col < 1 || col > 9 || num < 1 || num > 9) {
        printf("輸入超出範圍！請輸入 1-9 之間的數字。\n");
        return 0;
    }
    row--; col--;
    if (original_board[row][col] != 0) {
        printf("該位置是原始數字，不能修改！\n");
        return 0;
    }
    if (player_board[row][col] != 0) {
        printf("該位置已經填過數字了！\n");
        return 0;
    }
    if (answer_board[row][col] == num) {
        player_board[row][col] = num;
        printf("正確！\n");
        return 1;
    } else {
        error_count++;
        printf("錯誤！錯誤次數：%d\n", error_count);
        return 1;
    }
}

// 根據難度與亂數選題，複製到 board
void pick_random_puzzle(int level) {
    int idx;
    if (level == 1) { // easy
        idx = rand() % EASY_COUNT;
        for (int i=0;i<9;i++) for (int j=0;j<9;j++) board[i][j]=easy_puzzles[idx][i][j];
    } else if (level == 2) { // medium
        idx = rand() % MEDIUM_COUNT;
        for (int i=0;i<9;i++) for (int j=0;j<9;j++) board[i][j]=medium_puzzles[idx][i][j];
    } else { // hard
        idx = rand() % HARD_COUNT;
        for (int i=0;i<9;i++) for (int j=0;j<9;j++) board[i][j]=hard_puzzles[idx][i][j];
    }
}

// 遊戲主流程
void play_game() {
    printf("=== 數獨遊戲 ===\n");
    printf("規則：輸入 行 列 數字 來填數字\n");
    printf("錯誤5次遊戲結束\n\n");
    init_game();
    time_t start_time = time(NULL); // 讀秒開始
    while (1) {
        printf("\n目前盤面：\n");
        print_board(player_board);
        printf("剩餘空格：%d\t錯誤次數：%d/5\n", count_empty_cells(player_board), error_count);
        int result = handle_input();
        if (result == -1) {
            printf("遊戲結束！\n");
            break;
        }
        if (is_complete()) {
            time_t end_time = time(NULL);
            printf("\n🎉 恭喜！你完成了數獨！\n");
            print_board(player_board);
            printf("\n本局花費時間：%ld 秒\n", (long)(end_time - start_time));
            break;
        }
        if (error_count >= 5) {
            time_t end_time = time(NULL);
            printf("\n💥 錯誤太多次，遊戲結束！\n");
            printf("正確答案：\n");
            print_board(answer_board);
            printf("\n本局花費時間：%ld 秒\n", (long)(end_time - start_time));
            break;
        }
    }
}

int main() {
    srand(time(NULL));
    while (1) {
        printf("請選擇難度 (1)簡單 (2)中等 (3)困難：");
        int level = 1;
        scanf("%d", &level);
        pick_random_puzzle(level);
        while (getchar() != '\n'); // 清空緩衝區
        play_game();
        printf("\n是否要重新開始？(y/n): ");
        char c;
        while (getchar() != '\n');
        c = getchar();
        if (c != 'y' && c != 'Y') break;
    }
    printf("感謝遊玩！\n");
    return 0;
}
