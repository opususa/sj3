ここではsj3servの設定について述べます。

# 設定ファイルの場所 #

設定ファイルは${prefix}/etc/sj3/sj3serv.luaです。
$prefixはconfigureの指定によりますが、デフォルトで"/usr/local"になっています。

雛型のファイル"sj3serv.lua.example"が${prefix}/share/examples/sj3以下にありますので、
これをコピーしてカスタマイズすればいいでしょう。

# 書式 #

設定ファイルはluaで記述します。
文法は[lua](http://www.lua.org/)のページを参照してください。

# 設定項目 #

以下の関数が設定用に用意されています。
現在のところ、引数はすべてテーブル渡しです。

  * sj3.set\_server()
  * sj3.set\_domain()
  * sj3.append\_opendict()
  * sj3.set\_log()
  * sj3.set\_debug()
  * sj3.set\_error()
  * sj3.append\_allowuser()

# sj3.set\_server #

sj3.set\_server([_settings_])

sj3.set\_serverはsj3servの基本的な設定を行う関数です。
テーブルに設定可能なアイテムは以下のものです。

  * max\_client: 整数値
> サーバへ接続の上限値
  * user: 文字列
> sj3serv起動後に変更されるユーザ名。sj3servはrootで起動後、このユーザへ権限が降格されます。
  * chroot: boolean
> chrootするかどうか。セキュリティ保護を付与するため、trueに設定することを\*強く推奨します**。
  * chroot\_dir: 文字列
> sj3serv起動後の、chrootする先のディレクトリ名。
  * dict\_dir: 文字列
> 辞書のディレクトリ名。chroot後に辞書を開くので、chroot=true指定時には"/"の位置が変わるので注意してください。
  * strict\_auth: boolean
> 接続時しているクライアントのユーザが、指定された辞書のユーザ名に一致するかどうかチェックします。sj3proxyをしない限りはtrueに設定することを推奨します。**

# sj3.set\_domain #

sj3.set\_domain([_settings_])

クライアントの通信に使用されるUNIXドメインソケットの設定を行います。

  * socket\_name: 文字列
> ソケットファイルの位置を指定します。

# sj3.append\_opendict #

sj3.append\_opendict([_settings_])

デフォルトでオープンされる辞書を追加します。

  * file: 文字列
> 辞書名を指定します。

# sj3.set\_log #

sj3.set\_log([_settings_])

ロギング関連の設定を行います。

  * file: 文字列
> ログの出力先ファイルを指定します。

# sj3.set\_debug #

sj3.set\_debug([_settings_])

デバッグ関連の設定を行います。

  * file: 文字列
> デバッグログの出力先ファイルを指定します。

  * level: 整数値
> デバッグレベルを指定します。

# sj3.set\_error #

sj3.set\_error([_settings_])

エラー出力関連の設定を行います。

  * file: 文字列
> エラーファイルの出力先を指定します。