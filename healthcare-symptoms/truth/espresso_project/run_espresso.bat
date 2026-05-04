@echo off
setlocal enabledelayedexpansion

if not exist "espresso_project\models" mkdir "espresso_project\models"

echo Starting Espresso for 300 files...

for %%f in (espresso_project\*_train.pla) do (
    set "fullpath=%%f"
    set "filename=%%~nf"
    
    set "outname=!filename:_train=!"
    
    echo Processing: !filename!
    
    espresso.exe -type fr "%%f" > "espresso_project\models\!outname!.esp"
)

echo.
echo すべてのモデル(.esp)が espresso_project\models に作成されました。
pause