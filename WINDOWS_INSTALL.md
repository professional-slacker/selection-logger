# Windows インストール手順

## 重要：コード署名について
このソフトウェアはコード署名されていません。Windows DefenderやSmartScreenが警告を表示することがあります。

## インストール方法

### 方法1：手動インストール（推奨）

1. **ファイルをダウンロード**
   - `selection-logger.exe` をダウンロード

2. **警告を回避する**（必要な場合）：
   - ファイルを右クリック →「プロパティ」
   - 「全般」タブの下部にある「許可する」にチェック
   - 「OK」をクリック

3. **管理者としてコマンドプロンプトを開く**：
   - Windowsキー + X →「Windows PowerShell (管理者)」または「コマンド プロンプト (管理者)」

4. **インストールコマンドを実行**：
   ```cmd
   # インストールディレクトリを作成
   mkdir "C:\Program Files\SelectionLogger"
   
   # 実行ファイルをコピー（ダウンロードした場所に応じてパスを変更）
   copy "C:\Users\あなたのユーザー名\Downloads\selection-logger.exe" "C:\Program Files\SelectionLogger\"
   
   # サービスとしてインストール
   cd "C:\Program Files\SelectionLogger"
   selection-logger.exe install
   
   # サービスを開始
   selection-logger.exe start
   
   # ステータス確認
   selection-logger.exe status
   ```

### 方法2：バッチファイルを使用

1. `install.bat` をダウンロード
2. 右クリック →「管理者として実行」

### 方法3：PowerShellを使用

1. `install.ps1` をダウンロード
2. PowerShellを管理者として開く
3. 以下のコマンドを実行：
   ```powershell
   Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
   .\install.ps1
   ```

## アンインストール方法

### 手動アンインストール：
```cmd
# 管理者としてコマンドプロンプトを開く
cd "C:\Program Files\SelectionLogger"
selection-logger.exe stop
selection-logger.exe uninstall
cd \
rmdir /s "C:\Program Files\SelectionLogger"
```

### バッチファイルを使用：
- `uninstall.bat` を管理者として実行

## トラブルシューティング

### 警告が表示される場合：
```
Windows によって PC が保護されました
```
1. 「詳細情報」をクリック
2. 「実行」をクリック

または：
1. ファイルを右クリック →「プロパティ」
2. 「全般」タブ →「許可する」にチェック
3. 「OK」をクリック

### サービスが起動しない場合：
```cmd
# 管理者としてコマンドプロンプトを開く
cd "C:\Program Files\SelectionLogger"
selection-logger.exe status
selection-logger.exe stop
selection-logger.exe start
```

### ログの場所：
- `C:\Users\あなたのユーザー名\Documents\SelectionLogs\`
- 例：`2025-04.txt`（年月ごとのファイル）

## コマンド一覧

```cmd
selection-logger.exe install     # サービスをインストール
selection-logger.exe uninstall   # サービスをアンインストール
selection-logger.exe start       # サービスを開始
selection-logger.exe stop        # サービスを停止
selection-logger.exe status      # サービス状態を確認
selection-logger.exe run         # コンソールモードで実行（テスト用）
selection-logger.exe --help      # ヘルプを表示
```

## セキュリティ確認

ファイルの整合性を確認するには：
```powershell
Get-FileHash -Algorithm SHA256 selection-logger.exe
```

期待されるハッシュ値：
```
（実際のハッシュ値をここに記載）
```

## 注意事項

1. **管理者権限が必要**：サービスインストールには管理者権限が必要です
2. **ファイアウォール**：追加のファイアウォール設定は不要です
3. **ネットワークアクセス**：インターネット接続は不要です
4. **データ収集**：個人データを収集したり送信したりしません

## サポート

問題がある場合は：
1. ログファイルを確認：`Documents\SelectionLogs\`
2. コマンド：`selection-logger.exe status`
3. GitHub Issuesで報告