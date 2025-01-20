实现 AES-128 CBC 工作模式的加密和解密。<br>
输入数据由以下部分组成：<br>
uint8_t 工作模式和加密/解密<br>
0x01 表示 CBC 模式加密<br>
0x81 表示 CBC 模式解密<br>
最高位为 0 表示加密，1 表示解密<br>
uint8_t[16] 密钥<br>
uint8_t[16] IV<br>
uint32_t 明文/密文长度<br>
uint8_t[] 明文/密文内容<br>
