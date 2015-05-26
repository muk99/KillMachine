KillMachine (AudioUnitEffect)
===
エレキギターの特殊奏法の一つである、スイッチング奏法を超高速で行うAudioUnitEffectです。非常に簡素なプログラムなのでこれを参考にしてAudioUnitを作ってください。

## Demo
Comming Soon

## Requirement
xcodeprojファイルなので、ビルドする際はXcodeが必要です。
また、GarageBand, AU LabなどのAudioUnitsが対応したソフトウェアで読み込むことで使えます。

## Usage
入力信号を指定された時間でON・OFFをひたすら繰り返すだけのエフェクターです。
+   `Dead Length` :
    音を切る(スイッチオフにする)にする時間の長さ
 
+   `Alive Length` :
    音を鳴らす(スイッチオンにする)にする時間の長さ

## Install
1. killmachine.xcodeprojをXcodeで開けてビルド。
2. killmachine.componentを/Users/$USER/Library/Audio/Plug-Ins/Componentsに移す。
3. Terminal.appで"auval -a"を用いて"aufx KILL MUKS  -  Muks: Kill Machine (Effect AU)"がリストに含まれているか確認。
4. 含まれていたら正しく読み込まれています。AudioUnitsに対応しているソフトウェアでお使いください。

## Other
中身を弄る際にデバッグをしたい方は以下の方法を用意しています。非常に簡単なものですが・・・。

* killmachine.cppに#define DEBUG_PATH ""がコメントアウトされているので、""の中にディレクトリ内にあるdebug.txtのパスを書き込んでコメントアウトを外してください。そうすると、初期状態では変数stateとDead LengthとAlive Lengthの合計値、またON・OFFの状態がdebug.txtに書き込まれます。
* 書き込む内容を変える際は、debugを利用してください。
* もしデバッグを外したい場合は先ほどの#define DEBUG_PATHをコメントアウトしてください。 

## Licence
Copyright &copy; 2015 muk
Distributed under the [MIT License][mit].
[MIT]: http://www.opensource.org/licenses/mit-license.php

## Author
muk:[よなかのすうがく](http://muk99.hateblo.jp/)


