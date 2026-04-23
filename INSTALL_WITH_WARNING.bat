@echo off
echo ========================================
echo Selection Logger インストーラー
echo ========================================
echo.
echo 注意：このソフトウェアはコード署名されていません。
echo Windows Defenderが警告を表示する場合があります。
echo.
echo 続行するには「Y」を入力してください。
echo キャンセルするには他のキーを押してください。
echo.

choice /c YN /m "続行しますか？"
if errorlevel 2 goto cancel
if errorlevel 1 goto install

:install
echo.
echo 管理者権限を確認しています...
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo エラー：管理者権限が必要です。
    echo このバッチファイルを右クリックして「管理者として実行」してください。
    pause
    exit /b 1
)

echo インストールを開始します...
echo.

set INSTALL_DIR=C:\Program Files\SelectionLogger

echo 1. インストールディレクトリを作成: %INSTALL_DIR%
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"

echo 2. 実行ファイルをコピー...
copy "%~dp0selection-logger.exe" "%INSTALL_DIR%\" >nul

echo 3. Windowsサービスをインストール...
cd /d "%INSTALL_DIR%"
selection-logger.exe install

echo 4. サービスを開始...
selection-logger.exe start

echo 5. ステータスを確認...
selection-logger.exe status

echo.
echo ========================================
echo インストール完了！
echo ========================================
echo.
echo ログファイル: Documents\SelectionLogs\
echo コマンド: selection-logger.exe [status|stop|start]
echo.
echo アンインストールするには uninstall.bat を実行してください。
echo.
pause
exit /b 0

:cancel
echo.
echo インストールをキャンセルしました。
pause
exit /b 0