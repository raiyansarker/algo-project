#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#ifdef _WIN32
#  include <curses.h>
#else
#  include <ncurses.h>
#endif

using namespace std;

const char *TITLE_ART[] = {
  " _______                                    _   _             _            ",
  "|__   __|                                  | | | |           | |           ",
  "   | |_ __ ___  __ _ ___ _   _ _ __ ___     | |_| |_   _ _ __ | |_ ___ _ __ ",
  "   | | '__/ _ \\/ _` / __| | | | '__/ _ \\    |  _  | | | | '_ \\| __/ _ \\ '__|",
  "   | | | |  __/ (_| \\__ \\ |_| | | |  __/    | | | | |_| | | | | ||  __/ |   ",
  "   |_|_|  \\___|\\__,_|___/\\__,_|_|  \\___|    |_| |_|\\__,_|_| |_|\\__\\___|_|   ",
};

const int TITLE_LINES = sizeof(TITLE_ART) / sizeof(TITLE_ART[0]);


const int GRID_ROWS = 7;
const int GRID_COLS = 9;

struct Cell {
  int value;
  bool isObstacle;
  bool isVisited;
};

/**
 * GRID of our game
 */
vector<vector<Cell>> grid(GRID_ROWS, vector<Cell>(GRID_COLS, Cell({
  .value = 0,
  .isObstacle = false,
  .isVisited = false,
})));

/**
 * Generate dynamic grid
 */
void generate_grid() {
  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      grid[i][j].value = rand() % 10;
      grid[i][j].isObstacle = false,
      grid[i][j].isVisited = false;

      if (i == 0 && j == 0 && i == GRID_ROWS - 1 && j == GRID_COLS - 1) continue;

      bool is_start = (i == 0 && j == 0);
      bool is_start_neighbor = (i == 0 && j == 1) || (i == 1 && j == 0);
      bool is_end = (i == GRID_ROWS - 1 && j == GRID_COLS - 1);
      bool is_end_neighbor = (i == GRID_ROWS - 1 && j == GRID_COLS - 2) || (i == GRID_ROWS - 2 && j == GRID_COLS - 1);

      if (is_start || is_start_neighbor || is_end || is_end_neighbor) continue;

      /**
       * 12% of the time, cells are obstacles
       */
      grid[i][j].isObstacle = ((double)rand() / RAND_MAX) < 0.12;
    }
  }
}

/**
 * Game state
 */
struct State {
  int player_row;
  int player_col;
  int score;
  int optimal_score;
  vector<pair<int, int>> optimal_path;
  bool game_over;
};

State state = {
  .player_row = 0,
  .player_col = 0,
  .score = 0,
  .optimal_score = 0,
  .optimal_path = vector<pair<int, int>>(),
  .game_over = false,
};

void center_print(int row, const string &text) {
  int term_h, term_w;
  getmaxyx(stdscr, term_h, term_w);
  int col = max(0, (term_w - (int)text.size()) / 2);
  mvprintw(row, col, "%s", text.c_str());
}

int reveal_count = -1;
void render() {
  clear();

  /**
   * get terminal dimensions
   */
  int term_h, term_w;
  getmaxyx(stdscr, term_h, term_w);

  /**
   * print title of the game
   */
  int row_cursor = 1;
  for (int i = 0; i < TITLE_LINES; i++) {
    int col = max(0, (term_w - (int)strlen(TITLE_ART[i])) / 2);

    attron(A_BOLD | COLOR_PAIR(2));
    mvprintw(row_cursor + i, col, "%s", TITLE_ART[i]);
    attroff(A_BOLD | COLOR_PAIR(2));
  }
  row_cursor += TITLE_LINES + 1;


  /**
   * print score details
   */
  char score_buf[128];

  if (!state.game_over) {
    snprintf(score_buf, sizeof(score_buf), "Your Score: %d", state.score);

    center_print(row_cursor, score_buf);
  } else {
    snprintf(score_buf, sizeof(score_buf), "Your Score : %d", state.score);

    center_print(row_cursor, score_buf);
  }

  /**
   * print grid
   */
  row_cursor += 2;

  int cell_width = 4;
  int grid_pixel_width = GRID_COLS * cell_width;
  int grid_left = max(0, (term_w - grid_pixel_width) / 2);
  int grid_start_row = row_cursor;

  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      char buff[8];

      if (grid[i][j].isObstacle) {
        snprintf(buff, sizeof(buff), " %-2s", "#");
      } else {
        snprintf(buff, sizeof(buff), " %-2d", grid[i][j].value);
      }

      int col_start = grid_left + cell_width * j;
      bool is_player = (i == state.player_row && j == state.player_col);
      bool highlight = is_player || grid[i][j].isVisited;
      bool is_optimal = false;

      for (int k = 0; k <= reveal_count && k < (int)state.optimal_path.size(); k++) {
        if (state.optimal_path[k].first == i && state.optimal_path[k].second == j) {
          is_optimal = true;
          break;
        }
      }

      if (is_optimal) attron(COLOR_PAIR(3));
      else if (highlight) attron(COLOR_PAIR(1));
      mvprintw(grid_start_row + i, col_start, "%-*s", cell_width, "");
      mvprintw(grid_start_row + i, col_start, "%s", buff);
      if (is_optimal) attroff(COLOR_PAIR(3));
      else if (highlight) attroff(COLOR_PAIR(1));
    }
  }

  row_cursor = grid_start_row + GRID_ROWS + 2;

  if (!state.game_over) {
    center_print(row_cursor++, "Use the DOWN and RIGHT arrow keys to move.");

    center_print(row_cursor++, "Collect treasure along the way and avoid obstacles (#).");

    center_print(row_cursor++, "Your trail is highlighted in blue.");

    row_cursor++;

    center_print(row_cursor++, "Press r to restart, or q to quit.");
  } else if (reveal_count < (int)state.optimal_path.size()) {
      center_print(row_cursor++, "Treasure secured!");
      center_print(row_cursor++, "Revealing the optimal path...");
  } else {
    if (state.score == state.optimal_score) {
      char message[160];

      snprintf(message, sizeof(message), "You got it! Your score of %d matches the optimal score.", state.score);

      attron(A_BOLD | COLOR_PAIR(3));

      center_print(row_cursor++, message);

      attroff(A_BOLD | COLOR_PAIR(3));
    } else {
      char message[160];

      snprintf(message, sizeof(message), "Your score is %d. The optimal score is %d.", state.score, state.optimal_score);

      center_print(row_cursor++, message);
    }
  }

  refresh();
}

void animate_path() {
  reveal_count = 0;

  while (reveal_count < (int)state.optimal_path.size()) {
    render();

    napms(180);
    reveal_count++;
  }

  render();
}

void compute_path() {
  vector<vector<int>> dp(GRID_ROWS, vector<int>(GRID_COLS, -1));
  vector<vector<char>> trace(GRID_ROWS, vector<char>(GRID_COLS, 'X'));

  /**
   * calculate dp table
   * bottom up tabulation approach
   */
  dp[0][0] = grid[0][0].value;
  for (int i = 0; i < GRID_ROWS; i++) {
    for (int j = 0; j < GRID_COLS; j++) {
      if (i == 0 && j == 0) continue;
      if (grid[i][j].isObstacle) {
        dp[i][j] = -1;
        continue;
      }

      int best = -1;
      int parent = 'X';

      if (i > 0 && dp[i - 1][j] > best) {
        best = dp[i - 1][j];
        parent = 'u';
      }
      if (j > 0 && dp[i][j - 1] > best) {
        best = dp[i][j - 1];
        parent = 'l';
      }

      if (best == -1) continue;

      dp[i][j] = best + grid[i][j].value;
      trace[i][j] = parent;
    }
  }

  state.optimal_score = dp[GRID_ROWS - 1][GRID_COLS - 1];

  /**
   * write the optimal path
   */
  if (dp[GRID_ROWS - 1][GRID_COLS - 1] == -1) return;

  int i = GRID_ROWS - 1, j = GRID_COLS - 1;
  while (true) {
    state.optimal_path.push_back({i, j});

    char dir = trace[i][j];

    if (dir == 'u') {
      i--;
    } else if (dir == 'l') {
      j--;
    } else break;
  }

  reverse(state.optimal_path.begin(), state.optimal_path.end());
}

/**
 * setup initial game state and
 * compute necessary data
 */
void init_game() {
  state.player_row = 0;
  state.player_col = 0;
  state.score = 0;
  state.game_over = false;
  state.optimal_path.clear();

  reveal_count = -1;

  generate_grid();
  compute_path();

  grid[0][0].isVisited = true;
  state.score += grid[state.player_row][state.player_col].value;
}

int main() {
  srand(time(0));

  /**
   * ncurses setup
   */
  initscr();
  cbreak();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);

  start_color();
  // white text on blue background
  init_pair(1, COLOR_WHITE, COLOR_BLUE);
  // yellow text on black background
  init_pair(2, COLOR_YELLOW, COLOR_BLACK);
  // black text on green background
  init_pair(3, COLOR_BLACK, COLOR_GREEN);

  init_game();
  render();

  int ch;
  while ((ch = getch()) != 'q') {
    /**
     * restart the game at any moment when
     * 'r' is pressed
     */
    if (ch == 'r') {
      init_game();
      render();
      continue;
    }

    if (state.game_over) continue;

    bool moved = false;
    switch (ch) {
      case KEY_DOWN:
        if (state.player_row < GRID_ROWS - 1 && !grid[state.player_row + 1][state.player_col].isObstacle) {
          state.player_row++;
          moved = true;
        }
        break;
      case KEY_RIGHT:
        if (state.player_col < GRID_COLS - 1 && !grid[state.player_row][state.player_col + 1].isObstacle) {
          state.player_col++;
          moved = true;
        }
        break;

      default:
        break;
    }

    if (moved) {
      state.score += grid[state.player_row][state.player_col].value;
      grid[state.player_row][state.player_col].isVisited = true;

      if (state.player_row == GRID_ROWS - 1 && state.player_col == GRID_COLS - 1) {
        state.game_over = true;
        render();

        napms(500);

        /**
         * TODO: animate optimal path
         */
        animate_path();
      }
    }

    render();
  }

  endwin();
}
