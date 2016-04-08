# 動作確認済みのクライアント #

|クライアント名|接続方法|備考|
|:------|:---|:-|
|sj3proxy|UNIXドメインソケット|sj3に同梱|
|sj3dic |sj3lib|sj3に同梱|
|sj3stat|sj3lib|sj3に同梱|
|sj3demo|sj3lib|sj3に同梱|
|kinput2|sj3lib|文節学習時にクラッシュするバグが存在:[patch](http://www.tk2.nmt.ne.jp/~artrec/ar-sj3.shtml)|
|emacs-tamago|inet|sj3proxyを介して接続|
|emcws  |inet|sj3proxyを介して接続|
|uim-sj3|sj3lib|uim-1.5.0-alpha以降でサポート。辞書登録機能未実装|

注: sj3libを使用して接続するタイプのクライアントは、ホスト名に""を指定することでUNIXドメインソケットに直接接続することができます(kinput2ならば環境変数SJ3SERVに""を指定する、等)。それ以外の接続にはsj3proxyの起動が必要です。