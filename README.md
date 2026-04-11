# <img width="32" height="32" alt="appicon" src="https://github.com/user-attachments/assets/e2f67aa5-9f24-45f9-8626-609fa0cda5ce" /> ScreenOff  <img width="32" height="32" alt="activeicon" src="https://github.com/user-attachments/assets/85500ec8-71e1-428a-919c-be7e0f623022" />
A lightweight Windows tray application that turns off the display while keeping media playing. Designed for laptops with Modern Standby (S0 sleep) where turning off the display normally pauses video/audio.



## How it works:
Temporarily sets display timeout to 1 second\
Overrides video wake locks\
Reverts changes when user returns

## Usage:
1. Download ScreenOff.exe from Releases and run as Administrator*
2. Double click tray icon, icon changes color
3. Stay idle for 1 second, screen turns off
4. Move mouse or press any key to wake system up

*Without Administrator rights, video playback (YouTube, etc.) will keep the screen on. Run as Admin to override this.
