# win11_theme_changer

## About

This app was made as a fun little project to accompany WinDynamicDesktop. I used sunrise and sunset times to match up windows light/dark theme as close as possible with the day/night wallpaper.

## Setup

Compile the application and put `tc_config.cfg` in the same directory as the executable and edit the coordinates in the config file.
To make the application work you have to schedule it in windows task scheduler.
Create a new task, add 2 triggers:
Trigger 1 is basic `At log on` trigger,
Trigger 2 is an `On an event` trigger with these parameters:
```
-   Log:        System
-   Source:     Power-Troubleshooter
-   Event ID:   1
```
Make sure that `Start the task only if the computer is on AC power` under the Conditions tab is unchecked.