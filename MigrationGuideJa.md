# sj3-2.0.1.21からsj3-2.0.1.23への移行について #

## 変更点および対処方法 ##

以下の点が変更点と対処方法です。

  * ユーザ"`_sj3`"がインストール時に必要になりました。インストール前に追加してください。
  * sj2servの実行ファイルからsetuid flagsが外されました。一般ユーザがsj3servを起動することは出来なくなりました。
  * ソケットの場所が"/tmp/sj3sock"から"/usr/local/var/sj3/run/sj3serv.socket"に変更されました。以前のsj3libからは参照できなくなりますので、sj3libを利用するソフトウェア(kinput2等)を使っている場合、新しいsj3libをリンクし直してください。
  * 設定ファイルが"/usr/local/lib/sj3/serverrc"から"/usr/local/etc/sj3/sj3serv.lua"に変更されました。設定の詳細はSj3servSettingsJaを参照してください。
  * sj3servはネットワーク接続しなくなりました。もし、egg等のネットワーク接続が必要なクライアントから利用する場合、sj3.set\_serverの"strict\_auth"を"false"にした後、sj3servとsj3proxyを起動してください。
  * 辞書の場所が変更されました。"/usr/local/lib/sj3/dict"から"/usr/local/var/sj3/dict"に変更されました。また、辞書ディレクトリは"`_sj3`"のみ閲覧/編集可能になります。

## 辞書の移行例 ##

すでにカスタマイズ済みのsj3main.dicがある場合、

```
# mkdir /usr/local/var/sj3/run                      # ソケットのディレクトリ作成
# rm -fr /usr/local/var/sj3/dict                    # デフォルトインストールの辞書を削除
# mv /usr/local/lib/sj3/dict /usr/local/var/sj3     # 辞書を移動
# find /usr/local/var/sj3 | xargs chown _sj3:_sj3   # 所有者を_sj3に変更
# find /usr/local/var/sj3/dict/user -type d | xargs chmod 700 # すべてのディレクトリをdrwx------に変更
# find /usr/local/var/sj3/dict -type f | xargs chmod 600      # すべてのファイルを-rw-------に変更
# chown root:wheel /usr/local/var/sj3/dict/sj3main.dic # sj3main.dicの所有者をrootに変更
# chmod 644 /usr/local/var/sj3/dict/sj3main.dic        # sj3main.dicを-rw-r--r--に変更
```