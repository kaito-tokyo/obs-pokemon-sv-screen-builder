# OBSポケモンSVスクリーンビルダー

## 概要

「OBSポケモンSVスクリーンビルダー」はポケモンSVの配信をイイ感じにするためのOBSのプラグインです。以下のような特徴があります：

- 導入がインストーラーを実行するだけで、設定も数クリックするだけなので簡単！
- プレイヤーが何の操作もしなくても全自動でパーティー情報や対戦時間が更新されます！
- 負荷が少ないので60FPSで配信しても配信がカクカクしません！
- 自分の配信のテーマに合わせてCSSでカスタマイズができます！

OBSのバージョン29以上が必要です。Windows、macOS、Linuxの全てのプラットフォームで動作します。

## 画面の例

<img src="https://user-images.githubusercontent.com/1067855/218967358-68312e37-166b-4120-890a-8cd8d1df7d49.png" width="600">

## ダウンロード

https://github.com/umireon/obs-pokemon-sv-screen-builder/releases/latest

- Windowsの方はobs-pokemon-sv-screen-builder-x.x.x-windows-x64-Installer.exeをダウンロードして実行してください。
- Macの方はobs-pokemon-sv-screen-builder-x.x.x-macos-universal.pkgをダウンロードして実行してください。
- Linuxの方は自力でビルドしてください。Ubuntuを利用している方にはobs-pokemon-sv-screen-builder-x.x.x-x86_64-linux-gnu.debが提供されています。


## 設定方法

あらかじめ、Switchの画面を映している映像キャプチャデバイスのソースをシーンに追加しておく必要があります。映像キャプチャデバイスのシーン上での名前はメモしておいてください。

OBSポケモンSVスクリーンビルダーをインストールすると、「ポケモンSVスクリーンビルダー」というソースがOBSのシーンに追加できるようになります。

<img width="600" src="https://user-images.githubusercontent.com/1067855/218970534-de84022c-5013-4b3b-a22d-50ec1b3f7328.png">

ポケモンSVスクリーンビルダー追加時にプロパティが開きますので、まずは「デフォルトレイアウトのソースを追加」ボタンをクリックしてください。次に、ゲーム画面ソースの欄に先ほどメモしたゲーム画面の写っているソースの名前を指定してください。

<img width="600" src="https://user-images.githubusercontent.com/1067855/218971023-39bafc81-7970-42d8-9108-636de5fb0c24.png">

これにより現在のシーンに「対戦時間」「相手のランク」「自分の選出」「相手のパーティー」というソースが追加されます。これで、ゲーム本編で試合を開始すればそれぞれのソースに試合の情報が表示されるようになります。
