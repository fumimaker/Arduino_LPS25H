# 使い方
'C'を送るとキャリブレーションを行い、現在大気圧を0mに設定する。平滑してるので5秒程度時間がかかる。連打すると連打した回数*5秒くらいかかるので注意。
返り値が'O'なら成功、'N'なら失敗。Nの場合はセンサのレジスタが読めてないのでセンサが刺さってない、SA0ピンが間違ってるなどの問題を疑う。

'G'を送るとキャリブレーションをした大気圧を0cmとして現在の高度を送信する。単位は[cm]。unsigned int 16bitで、Higher8bit + Lower8bitで送信される。アスキーではないので目では見えないことに注意。
例: 高度が0x4d2[cm]の場合、0x04 + 0xd2が送信される。

多分Arduino coreが走るマイコンならなんでも動く。LSP25Hが3.3V系なのでUnoやNano, microではレベル変換をする必要があることに注意。
