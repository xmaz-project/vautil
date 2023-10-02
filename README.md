# vautil

## �c�[��

* [kbdchk](CHKTOOL/KBDCHK.TXT): �L�[�{�[�h���̓`�F�b�N�c�[��


## ���|�W�g���̏��ݒ�

* �t�@�C��
    * �����R�[�h: CP932
    * ���s: CR+LF
* �R�~�b�g���b�Z�[�W
    * �����R�[�h: UTF-8
    * ���s: LF

���l

* �R�~�b�g���b�Z�[�W�̕����R�[�h
    * �t�@�C���ƍ��킹 CP932 �Ƃ����ق��� git �̕����R�[�h�֘A�ݒ肪�V���v���ɂȂ�B�������ACP932 ���� GitHub ��ŉ����Ă��܂����Ƃ�����B

### Git Bash (Windows) �ݒ��

* �[���E�B���h�E�̕����R�[�h: SJIS
    * �^�C�g���o�[�E�N���b�N > Options > Text > Character set
* shell
    * `LANG=ja_JP.SJIS`
* git
    ```
    git config core.autocrlf false
    git config pager.log "iconv -f utf-8 -t cp932 | LESSCHARSET=dos less"
    git config core.pager "LESSCHARSET=dos less"
    ```

����

* pager �Ƃ��� less ���Ăяo���ꂽ�Ƃ��� CP932 ���\���ł���悤 LESSCHARSET ���w��B
* �R�~�b�g���b�Z�[�W�� UTF-8 �ŏo�͂���邽�߁Agit log �̂� CP932 �ɕϊ��B
* ���̐ݒ�ł����̏ꍇ�͕�����������B
    * git show: �قȂ镶���R�[�h�����݁B�R�~�b�g���b�Z�[�W(UTF-8)�͉�����B�t�@�C��(CP932)�͐���B

���l

* �R�~�b�g���O�̕\���́A `git config i18n.logOutputEncoding cp932` �ł� OK �����Agit rebase ���ɉ������肪����B

