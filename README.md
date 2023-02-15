# OBSポケモンSVスクリーンビルダー

## 概要

「OBSポケモンSVスクリーンビルダー」はポケモンSVの配信をイイ感じにするためのOBSのプラグインです。以下のような特徴があります：

- 導入がインストーラーを起動するだけで、設定も数クリックするだけなので簡単！
- プレイヤーが何の操作もしなくても全自動でパーティー情報が更新されます！
- 負荷が少ないので60FPSで配信しても配信がカクカクしません！

OBSのバージョン28以上が必要です。Windows、macOS、Linuxの全てのプラットフォームで動作します。

## 画面の例

<img src="https://user-images.githubusercontent.com/1067855/218967358-68312e37-166b-4120-890a-8cd8d1df7d49.png" width="600">

左下に自分の選出が選出した順に並びます。右端に相手のポケモンが並びます。タイマーは自分で好きな位置に配置できます。

## ダウンロード

https://github.com/umireon/obs-pokemon-sv-screen-builder/releases/latest

- Windowsの方はobs-pokemon-sv-screen-builder-x.x.x-windows-x64-Installer.exeをダウンロードして実行します。
- Macの方はobs-pokemon-sv-screen-builder-x.x.x-macos-universal.pkgをダウンロードして下の方に記載してある手順に従ってインストールします。
- Linuxの方はobs-pokemon-sv-screen-builder-x.x.x-linux-x86_64.debをダウンロードしてインストールします。

## 設定方法

OBSポケモンSVスクリーンビルダーをインストールすると、「ポケモンSVスクリーンビルダー」というソースがOBSのシーンに追加できるようになります。

<img width="600" src="https://user-images.githubusercontent.com/1067855/218970534-de84022c-5013-4b3b-a22d-50ec1b3f7328.png">

ポケモンSVスクリーンビルダーのプロパティを開き、「ゲーム画面ソース」にはSwitchの画面を映している映像キャプチャデバイスのソースを、「タイマーソース」には試合時間を表示するためのテキストソースを指定します。

<img width="600" src="https://user-images.githubusercontent.com/1067855/218971023-39bafc81-7970-42d8-9108-636de5fb0c24.png">

これで、あとはSwitchの画面を上の画像のように少し縮小してから試合を開始すれば、自動的に自分の選出と相手のパーティーが切り抜かれて表示され、試合が開始されると自動的に20分のカウントダウンが始まります。

## Macのインストールの手順

Macでインストールする際には、インストーラーに対して少し操作をする必要があります。

<img width="600" src="https://user-images.githubusercontent.com/1067855/218972052-62adffa5-17fc-4572-b194-fa69dc1218f6.png">

この画面になったら「インストール先を変更...」をクリックします。

<img width="735" src="https://user-images.githubusercontent.com/1067855/218972292-231bc8d1-5dc5-4e13-b70c-95fdd5ed7886.png">

このまま次へを押して、セットアップを完了させればOBSポケモンSVスクリーンビルダーが利用できるようになります。
