# Migration from sj3-2.0.1.21 to sj3-2.0.1.23 #

## Changes ##

  * New user "`_sj3`" is required. Add new user "`_sj3`".
  * Placement of UNIX domain socket file is changed from "/tmp/sj3sock" to "/usr/local/var/sj3/run/sj3serv.socket". Old version of sj3lib cannot access this file. If you use software of using sj3lib (like kinput2, etc.), please relink new sj3lib.
  * Configuration file "/usr/local/lib/sj3/serverrc" is changed to "/usr/local/etc/sj3/sj3serv.lua". Detail of setting is Sj3servSettings.
  * sj3serv does not listen internet. If you want to use client of internet socket only use (like egg, etc.), edit entry of sj3serv.lua, "strict\_auth" to "false". And then start sj3serv and sj3proxy.
  * Placement of dictionary is changed from "/usr/local/lib/sj3/dict" to "/usr/local/var/sj3/dict". And directory/file of dictionary can be edited by "`_sj3`" user only.

## Dictionary migration ##

If you have customized sj3main.dic,
# mkdir /usr/local/var/sj3/run                      # make directory of socket
# rm -fr /usr/local/var/sj3/dict                    # delete default dictionary
# mv /usr/local/lib/sj3/dict /usr/local/var/sj3     # move dictionary
# find /usr/local/var/sj3 | xargs chown _sj3:_sj3   # change owner to _sj3
# find /usr/local/var/sj3/dict/user -type d | xargs chmod 700 # change dictionary permission to drwx------
# find /usr/local/var/sj3/dict -type f | xargs chmod 600      # change file permission to -rw-------
# chown root:wheel /usr/local/var/sj3/dict/sj3main.dic # change owner of sj3main.dic to root
# chmod 644 /usr/local/var/sj3/dict/sj3main.dic        # change permission of sj3main.dic to -rw-r--r--```