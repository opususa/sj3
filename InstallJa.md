# セットアップ #

コマンドラインから

```
./configure
```

を入力することでセットアップします。
どのようなオプションがあるかを知るには、

```
 ./configure --help
```

と入力してください。

# コンパイル #

```
 make
```

でビルドを開始します。

# インストール #

**インストールするには、ユーザ"`_sj3`"が必要です。事前に追加しておいてください。**

rootになって、

```
 make install
```

でインストールが開始します。

## ソケットファイルのディレクトリ作成 ##

sj3servはクライアントとの通信にUNIX domain socketを使用しています。
ここではその場所を作成します。
ディレクトリの所有者は"`_sj3:_sj3`"にしておきます。

```
 mkdir /usr/local/var/sj3/run
 chown _sj3:_sj3 /usr/local/var/sj3/run
```

## 設定ファイルの設置 ##

sj3servの設定ファイルは"/usr/local/etc/sj3/sj3serv.lua"です。
"/usr/local/share/examples/sj3proxy.lua.example"からコピーして編集しましょう。
sj3proxy.lua.exampleは一般的なデスクトップユーザ用の設定です。
コピーするだけでほとんど問題なくsj3servを使用できるはずです。

```
 cp /usr/local/share/examples/sj3proxy.lua.example /usr/local/etc/sj3/sj3serv.lua
```
