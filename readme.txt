世界初、ソレノイドが喋る

Youtube: 
  https://www.youtube.com/watch?v=Zf2zZbRWV-c
  https://www.youtube.com/watch?v=IiyOQ7kBb5U

Document: https://github.com/uecken/DoLeMiFa-SoLeNoiDo/blob/main/document/%E3%83%89%E3%83%AC%E3%83%9F%E3%83%95%E3%82%A1%E3%82%BD%E3%83%AC%E3%83%8E%E3%82%A4%E3%83%89%E8%B3%87%E6%96%99.pdf


[動作環境]
・Platform Ioで DoLeMiFa SoLeNoiDo.code-workspace を開きコンパイルし、USB接続したM5StickCに書き込むと動作します。
・配線はDocumentを参照。


[AI]
・(済)Duty10%以上でスイッチングしないような保護機能追加
・音量を大きくするための施策
・手持ちソレノイド(CBS10290170,CB10370150)以外、また24V電源以外での動作検証。

(Option:)
・基板作成
　・3.5mmコネクタ、スイッチング回路、アナログPWM変調モジュール追加
　・可能ならXIAO ESP32C3で行う(A0~3ピンからPWM主鬱力するとノイズが発生したのでソフトで対策する)
・LTSpiceでバネやソレノイドの電磁力による機構を加味した統合電気シミュレータ