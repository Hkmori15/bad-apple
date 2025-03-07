## Bad-apple

Bad-apple in ASCII format which output in terminal 🍎

## Requirements 📜:

**For building and running you need:**

- Compiler with support for C++23
- CMake with version 3.15 or higher
- If dependencies don't install automatically (cmake), you need install it manually:
- [SDL2](https://github.com/libsdl-org/SDL/releases/tag/release-2.28.5) Only version 2.28.5, why? See file **CMakeLists.txt in 16 line** ❗
- Ffmpeg for extracting frames from **bad_apple.mp4**

## Installing/Building ⭐:

1. Clone the repo: 

```bash
git clone https://github.com/Hkmori15/bad-apple.git
cd/z bad_apple
```
2. Create directory frames in root project:

```bash
mkdir frames
```
3. Extract frames from video **bad_apple.mp4:**

```bash
ffmpeg -i bad_apple.mp4 -vf scale=80:-1 frames/frame%04d.png
```
4. Build project with CMake:

```bash
mkdir build
cd/z build
cmake ..
cmake --build .
```

## Usage 💿:

1. Move to **build** directory:

```bash
cd/z build
```
2. Run:

```bash
./bad_apple
```
3. Enjoy! Animation will be represent how ASCII-art and sound with SDL2 :D
