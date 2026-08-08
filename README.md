# Treasure Hunter

A terminal-based game built around the classic **grid dynamic programming** problem. Navigate a 7×9 grid from the top-left to the bottom-right, collecting treasure and avoiding obstacles — then watch the DP-optimal path animate itself so you can compare.

```
 _______                                    _   _             _
|__   __|                                  | | | |           | |
   | |_ __ ___  __ _ ___ _   _ _ __ ___     | |_| |_   _ _ __ | |_ ___ _ __
   | | '__/ _ \/ _` / __| | | | '__/ _ \    |  _  | | | | '_ \| __/ _ \ '__|
   | | | |  __/ (_| \__ \ |_| | | |  __/    | | | | |_| | | | | ||  __/ |
   |_|_|  \___|\_,_|___/\__,_|_|  \___|    |_| |_|\__,_|_| |_|\__\___|_|
```

## How It Works

The grid uses the classic DP recurrence:

```
dp[i][j] = grid[i][j] + max(dp[i-1][j], dp[i][j-1])
```

You can only move **right** or **down** — mirroring DP transitions exactly. When you reach the goal, the game reveals the optimal path so you can see how your route compares.

## Controls

| Key | Action |
|:----|:-------|
| `→` Right arrow | Move right |
| `↓` Down arrow  | Move down  |
| `r` | Restart with a new grid |
| `q` | Quit |

`#` cells are obstacles and cannot be entered.

---

## Building

### Linux

Install ncurses development headers:

```bash
# Debian / Ubuntu
sudo apt-get install build-essential libncurses-dev

# Fedora / RHEL
sudo dnf install gcc-c++ ncurses-devel

# Arch
sudo pacman -S gcc ncurses
```

Compile and run:

```bash
g++ -std=c++20 -O2 -Wall main.cpp -o game -lncurses
./game
```

---

### macOS

macOS ships with ncurses. No extra install needed.

```bash
g++ -std=c++20 -O2 -Wall main.cpp -o game -lncurses
./game
```

#### macOS Universal Binary (Intel + Apple Silicon)

```bash
g++ -std=c++20 -O2 -Wall -arch x86_64 -arch arm64 main.cpp -o game -lncurses
./game
```

---

### Windows (MSYS2 / MinGW-w64)

Windows does not ship with ncurses, so we build [PDCurses](https://github.com/wmcbrine/PDCurses) from source (this avoids ABI mismatch issues with pre-built MSYS2 packages).

**1. Install MSYS2** from https://www.msys2.org, then open the **MINGW64** shell and install the compiler:

```bash
pacman -S mingw-w64-x86_64-gcc make git
```

**2. Clone and build PDCurses:**

```bash
git clone --depth 1 https://github.com/wmcbrine/PDCurses.git pdcurses-src
cd pdcurses-src/wincon
make DLL=N WIDE=Y UTF8=Y
cd ../..
```

**3. Compile the game:**

```bash
g++ -std=c++20 -O2 -Wall \
  -DPDC_WIDE -DPDC_FORCE_UTF8 \
  -Ipdcurses-src \
  main.cpp -o game.exe \
  pdcurses-src/wincon/pdcurses.a
```

**4. Run:**

```bash
./game.exe
```

---

## Releases

Pre-built binaries for all platforms are published automatically on every tagged release via GitHub Actions.

| Platform | Binary |
|:---------|:-------|
| Linux x86\_64 | `game-linux-x86_64.tar.gz` |
| macOS Universal (Intel + Apple Silicon) | `game-macos-universal.tar.gz` |
| Windows x86\_64 | `game-windows-x86_64.zip` |

**To create a release**, push a version tag:

```bash
git tag v1.0.0
git push origin v1.0.0
```

GitHub Actions will build all three targets and publish them automatically to the Releases page.

---

## Algorithms

| Component | Algorithm |
|:----------|:----------|
| Optimal path calculation | Bottom-up DP (tabulation) |
| Path reconstruction | Backtracking via parent trace table |
| Obstacle generation | Uniform random, 12% density |

The DP runs in **O(rows × cols)** time and space. Path reconstruction is **O(rows + cols)**.
