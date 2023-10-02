# vautil

## ツール

* [kbdchk](CHKTOOL/KBDCHK.TXT): キーボード入力チェックツール


## リポジトリの諸設定

* ファイル
    * 文字コード: CP932
    * 改行: CR+LF
* コミットメッセージ
    * 文字コード: UTF-8
    * 改行: LF

備考

* コミットメッセージの文字コード
    * ファイルと合わせ CP932 としたほうが git の文字コード関連設定がシンプルになる。しかし、CP932 だと GitHub 上で化けてしまうことがある。

### Git Bash (Windows) 設定例

* 端末ウィンドウの文字コード: SJIS
    * タイトルバー右クリック > Options > Text > Character set
* shell
    * `LANG=ja_JP.SJIS`
* git
    ```
    git config core.autocrlf false
    git config pager.log "iconv -f utf-8 -t cp932 | LESSCHARSET=dos less"
    git config core.pager "LESSCHARSET=dos less"
    ```

説明

* pager として less が呼び出されたときに CP932 が表示できるよう LESSCHARSET を指定。
* コミットメッセージは UTF-8 で出力されるため、git log のみ CP932 に変換。
* この設定でも次の場合は文字化けする。
    * git show: 異なる文字コードが混在。コミットメッセージ(UTF-8)は化ける。ファイル(CP932)は正常。

備考

* コミットログの表示は、 `git config i18n.logOutputEncoding cp932` でも OK だが、git rebase 時に化ける問題がある。

