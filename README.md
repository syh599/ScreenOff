# <img width="32" height="32" alt="appicon" src="https://github.com/user-attachments/assets/e2f67aa5-9f24-45f9-8626-609fa0cda5ce" /> ScreenOff  <img width="32" height="32" alt="activeicon" src="https://github.com/user-attachments/assets/85500ec8-71e1-428a-919c-be7e0f623022" />

A lightweight Windows tray application that turns off the display while keeping media playing. Designed for laptops with Modern Standby (S0 sleep) where turning off the display normally pauses video/audio, with a built-in sleep timer that can automatically pause playback after a set time, perfect for listening to podcasts, Youtube videos, or music before sleep.

## Features
- 🖥️ Turn display off while media continues playing
- ⏲️ Sleep timer to automatically pause media after a set time

## How it works:
- Temporarily sets display timeout to 1 second
- Overrides video wake locks
- Reverts changes when user returns

## Usage:
1. Download ScreenOff.exe from Releases and run
2. **Double-click** tray icon → icon changes color, screen will turn off after 1 second of inactivity
    - <img width="16" height="16" alt="inactive" src="https://github.com/user-attachments/assets/e2f67aa5-9f24-45f9-8626-609fa0cda5ce" /> **Default icon** - Inactive (normal display behavior)
    - <img width="16" height="16" alt="active" src="https://github.com/user-attachments/assets/85500ec8-71e1-428a-919c-be7e0f623022" /> **Active icon** - Active (screen will turn off after 1 second idle)
3. **Move mouse or press any key** to wake system and restore normal behavior
4. **Right-click** tray icon for additional options:
   - Set a sleep timer (15, 30, 45, 60 minutes, or custom)
   - Enable/disable run on startup
   - Exit application

## Sleep Timer
1. Set a time by right-clicking the tray icon, the menu should show: ✓ Sleep Timer
2. Sleep timer will be activated after double clicking icon
3. When the sleep timer expires, ScreenOff will automatically send a media pause command (Play/Pause).<br/>
- Sleep timer will be cancelled if user returns before it expires
- To control which media app receives the pause command:



<img width="94" height="198" alt="choose-media-source" src="https://github.com/user-attachments/assets/051783dd-b30d-438f-b19a-d271162caa6d" />


- Test the PAUSE command in the tray icon menu



## Notes
- Tested on Windows 11 23H2
- The application runs as a hidden window with a system tray icon only
- The application must be run as Administrator (to override display wake lock)
- Only one instance can run at a time
- All settings are automatically restored when waking the screen or exiting the app
