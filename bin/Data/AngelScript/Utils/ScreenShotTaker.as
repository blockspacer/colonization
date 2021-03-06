#include "AngelScript/Utils/Constants.as"

class ScreenShotTaker : ScriptObject
{
    Array <int> keys_;
    String screenShotsFolder_;

    ScreenShotTaker ()
    {
        screenShotsFolder_ = DEFAULT_SCREEN_SHOTS_FOLDER;
        keys_.Push (KEY_PRINTSCREEN);
    }

    ~ScreenShotTaker ()
    {

    }

    void Start ()
    {
        if (not fileSystem.DirExists (screenShotsFolder_))
        {
            fileSystem.CreateDir (screenShotsFolder_);
        }
    }

    void Update (float timeStep)
    {
        // Check is all keys pressed.
        bool isKeysPressed = true;
        uint index = 0;
        while (index < keys_.length and isKeysPressed)
        {
            isKeysPressed = input.keyPress [keys_ [index]];
            index++;
        }

        // Take screen shot if all keys pressed.
        if (isKeysPressed)
        {
            Image @image = Image ();
            graphics.TakeScreenShot (image);
            // Find available name.
            index = 0;
            while (fileSystem.FileExists (screenShotsFolder_ + index + ".png"))
            {
                index++;
            }
            image.SavePNG (screenShotsFolder_ + index + ".png");
        }
    }

    void Stop ()
    {

    }
}
