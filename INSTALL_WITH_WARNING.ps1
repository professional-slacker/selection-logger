# Selection Logger Installer with Warning
# Run as Administrator

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Selection Logger インストーラー" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "重要：コード署名について" -ForegroundColor Yellow
Write-Host "このソフトウェアはコード署名されていません。" -ForegroundColor Yellow
Write-Host "Windows DefenderやSmartScreenが警告を表示することがあります。" -ForegroundColor Yellow
Write-Host ""

Write-Host "警告が表示された場合の対処法：" -ForegroundColor Yellow
Write-Host "1. 「詳細情報」をクリック" -ForegroundColor Yellow
Write-Host "2. 「実行」をクリック" -ForegroundColor Yellow
Write-Host "または：" -ForegroundColor Yellow
Write-Host "1. ファイルを右クリック →「プロパティ」" -ForegroundColor Yellow
Write-Host "2. 「全般」タブ →「許可する」にチェック" -ForegroundColor Yellow
Write-Host "3. 「OK」をクリック" -ForegroundColor Yellow
Write-Host ""

$response = Read-Host "続行しますか？ (Y/N)"
if ($response -ne "Y" -and $response -ne "y") {
    Write-Host "インストールをキャンセルしました。" -ForegroundColor Red
    pause
    exit
}

# Check for admin rights
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
if (-not $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    Write-Host "エラー：管理者権限が必要です。" -ForegroundColor Red
    Write-Host "このスクリプトを右クリックして「管理者として実行」してください。" -ForegroundColor Yellow
    pause
    exit 1
}

Write-Host "管理者権限を確認しました。" -ForegroundColor Green
Write-Host ""

$InstallPath = "C:\Program Files\SelectionLogger"

Write-Host "1. インストールディレクトリを作成: $InstallPath" -ForegroundColor Cyan
if (-not (Test-Path $InstallPath)) {
    New-Item -ItemType Directory -Path $InstallPath -Force | Out-Null
}

Write-Host "2. 実行ファイルをコピー..." -ForegroundColor Cyan
Copy-Item "selection-logger.exe" -Destination $InstallPath -Force

Write-Host "3. Windowsサービスをインストール..." -ForegroundColor Cyan
Start-Process -FilePath "$InstallPath\selection-logger.exe" -ArgumentList "install" -Wait -NoNewWindow

Write-Host "4. サービスを開始..." -ForegroundColor Cyan
Start-Process -FilePath "$InstallPath\selection-logger.exe" -ArgumentList "start" -Wait -NoNewWindow

Write-Host "5. ステータスを確認..." -ForegroundColor Cyan
Start-Process -FilePath "$InstallPath\selection-logger.exe" -ArgumentList "status" -Wait -NoNewWindow

Write-Host ""
Write-Host "========================================" -ForegroundColor Green
Write-Host "インストール完了！" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Green
Write-Host ""
Write-Host "ログファイル: Documents\SelectionLogs\" -ForegroundColor Cyan
Write-Host "コマンド: selection-logger.exe [status|stop|start|--help]" -ForegroundColor Cyan
Write-Host ""
Write-Host "アンインストール: uninstall.bat を実行" -ForegroundColor Cyan
Write-Host ""
pause