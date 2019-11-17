#include "ev3.h"
#include <unistd.h>

bool fileExists(char fileName[]) {
    return access(fileName, R_OK) != -1;
}

int main () {
    InitEV3();

    // Play C2 (whole note) at volume 50
    PlayToneEx(TONE_C2, NOTE_WHOLE, 50);

    // Play system sound "double beep" at volume 50
    PlaySoundEx(SOUND_DOUBLE_BEEP, 50);

    // Plays an audio file if it exists (you need to copy it into the robot
    // or put it into an sd card and then update the path).
    // Refer to the documentation to see the list of supported formats
    char audioFile[] = "path/to/file.wav";
    if (fileExists(audioFile)) {
        PlayFileEx(audioFile, 50, false);
    }

    FreeEV3();
    return 0;
}