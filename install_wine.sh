#!/bin/bash

echo "Wine 再インストールスクリプト (Debian用)"
echo "========================================"
echo ""
echo "Purpose: Set up Wine environment for testing Windows builds of selection-logger"
echo "Usage: Run this script to clean install Wine for cross-platform testing"
echo "Test Script: Use test_wine_auto.sh for actual testing"
echo ""

# 既存のWineを削除
echo "1. 既存のWineを削除..."
sudo apt remove --purge wine* winehq* -y 2>/dev/null || true
sudo apt autoremove -y

# 古い設定を削除
echo "2. 古い設定を削除..."
rm -rf ~/.wine ~/.wine32 ~/.local/share/applications/wine ~/.local/share/wine 2>/dev/null || true

# システム更新
echo "3. システムを更新..."
sudo apt update
sudo apt upgrade -y

# 依存関係をインストール (Debian用)
echo "4. 依存関係をインストール..."
sudo apt install -y wget gnupg ca-certificates

# 32-bitサポートを有効化
echo "5. 32-bitサポートを有効化..."
sudo dpkg --add-architecture i386

# WineHQリポジトリを追加 (オプション)
echo "6. WineHQリポジトリを追加..."
wget -O - https://dl.winehq.org/wine-builds/winehq.key 2>/dev/null | sudo apt-key add - 2>/dev/null || true
echo "deb https://dl.winehq.org/wine-builds/debian/ bullseye main" | sudo tee /etc/apt/sources.list.d/winehq.list 2>/dev/null || true

# パッケージリストを更新
sudo apt update

# Wineをインストール (標準リポジトリから)
echo "7. Wineをインストール..."
sudo apt install -y wine wine32 wine64

# Wine環境を初期化
echo "8. Wine環境を初期化..."
wineboot 2>/dev/null || true

# テスト
echo "9. テスト..."
wine --version 2>/dev/null || echo "Wineのバージョン確認に失敗"

echo ""
echo "インストール完了！"
echo "テストコマンド: wine selection-logger.exe --help"