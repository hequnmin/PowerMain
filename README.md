# PowerMain
Blueway ATE PowerMain

 - ESP32 简易串口通讯服务，通讯指令解析。

## 特征
 - [x] 支持ESP32;
 - [x] Uart定义, RX/TX 引脚自定义, 通讯监听;
 - [x] 接收事件回调，应答发送后事件回调;
 - [x] 请求结构体/应答结构体的序列化/反序列化;


## 应用与实例


## API

 ### Command Struct & JSON
 #### 请求关键字
  - REQUEST_KEY_ERROR,          // 请求类型错误
  - REQUEST_KEY_BASIC,          // 基本请求
  - REQUEST_KEY_INFO,           // <获取系统信息>请求
  - REQUEST_KEY_RESET,          // <复位/清零>请求
  - REQUEST_KEY_CONFIG,         
  - REQUEST_KEY_CUSTOM,         // <自定义>请求
  - REQUEST_KEY_SETVOL,         // 电压设置
  - REQUEST_KEY_GETVOL,         // 电压读数
  - REQUEST_KEY_UART,           // UART通讯

 #### 请求/应答格式

 **REQUEST_BODY_BASIC 请求结构体格式**
 - int id           // 请求标识号
 - string cmd       // 请求关键字
 - [string err]     // 解析错误信息
 - JSON: ***{"id":123, "cmd":"xxxx"}***

 **RESPONSE_BODY_BASIC 应答基本格式**
 - int id           // 请求标识号
 - [string err]     // 错误信息，NULL或空：无错误/执行成功
 - Success JSON: ***{"id":123,...}*** - 执行成功；
 - Failed JSON: ***{"err":"error message.","id":123,...}*** - 执行错误；

 #### Struct & JSON 实例

 **REQUEST_BODY_BASIC 请求基本结构体**
 - int id           // 请求标识号
 - string cmd       // 请求关键字
 - [string err]     // 请求解析错误信息
 - REQUEST_KEY key  // 解析后关键字枚举，REQUEST_KEY_INFO,REQUEST_KEY_RESET,...
 - JSON: {"id":123, "cmd":"xxxx"}

 **RESPONSE_BODY_BASIC 应答基本格式**
 - int id           // 请求标识号
 - [string err]     // 错误信息
 - Success JSON: {"id":123,...} - 执行成功；
 - Failed JSON: {"err":"error message.", "id":123,...} - 执行错误；


 **1. INFO 系统信息**

 **REQUEST_BODY_INFO <系统信息>请求结构体**
 - int id                               // 请求标识号
 - char* cmd = "info"                  // 请求关键字
 - REQUEST_KEY key = REQUEST_KEY_INFO    
 - JSON: {"id":123, "cmd":"info"}

 **RESPONSE_BODY_INFO <系统信息>应答结构体**
 - int id                   // 请求标识号
 - char* chi               // 芯片信息
 - char* mac               // 物理地址
 - char* ver               // 嵌入式版本
 - int chn                  // 通道号
 - int cnn                  // 连接号
 - JSON: { "id":123, "chi":"esp32", "mac":"AA01", "ver": "1.2.3.4", "chn": 12, "cnn": 12  }

 **2. RESET 复位/清零**

 **REQUEST_BODY_RESET <复位/清零>请求结构体**
 - int id                   // 请求标识号
 - char* cmd = "reset"     // 请求关键字
 - REQUEST_KEY key = REQUEST_KEY_RESET
 - JSON: {"id":123, "cmd":"reset"}

 **RESPONSE_BODY_RESET <复位/清零>应答结构体**
 - int id                   // 请求标识号
 - JSON: { "id":123 }

 **3. CUSTOM 自定义** 

 **REQUEST_BODY_CUSTOM <自定义>请求结构体**
 - int id                   // 请求标识号
 - char* cmd = "custom"     // 请求关键字
 - unsigned char* data = NULL;     // 字节数组地址
 - int len = 0;           // 字节数组长度
 - REQUEST_KEY key = REQUEST_KEY_CUSTOM
 - JSON: {"id":123, "cmd":"custom", "data":"1234567890ABCDEF"}

 **RESPONSE_BODY_CUSTOM <自定义>应答结构体**
 - int id                   // 请求标识号
 - JSON: { "id":123, "data":"1234567890ABCDEF"}

**4. SETVOL 电压设置** 
**REQUEST_BODY_SETVOL <电压设置>请求结构体**
 - int id                   // 请求标识号
 - char* cmd = "setvol"     // 请求关键字
 - vector<double> vol;      // 电芯电压设置
 - REQUEST_KEY key = REQUEST_KEY_SETVOL
 - JSON: {"id":123, "cmd":"setvol", "vol":[3800,0,3800]}
 **RESPONSE_BODY_SETVOL <电压设置>应答结构体**
 - int id                   // 请求标识号
 - JSON: { "id":123}

**5. GETVOL 电压读数** 
**REQUEST_BODY_GETVOL <电压读数>请求结构体**
 - int id                   // 请求标识号
 - char* cmd = "getvol"     // 请求关键字
 - vector<bool> rel;        // 继电器开关
 - REQUEST_KEY key = REQUEST_KEY_GETVOL
 - JSON: {"id":123, "cmd":"getvol", "rel":[1,0,1]}
 **RESPONSE_BODY_GETVOL <电压读数>应答结构体**
 - int id                   // 请求标识号
 - vector<double> vol;      // 电芯电压读数
 - JSON: { "id":123, "vol":[3799.998, 3800.123, 3800.889]}

**6. Uart通讯**
**REQUEST_BODY_UART <Uart通讯>请求结构体**
 - int baud;           // 波特率
 - int vcc;            // Uart供电
 - int status;         // 是否保持连接
 - int delay;
 - unsigned char* data;
 - int len;
 - JSON: { "id": 1,"cmd": "uart","baud": 115200,"vcc": 3500,"status": 1,"delay": 200,"data":"F10123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFF2"}

 **RESPONSE_BODY_UART <Uart通讯>应答结构体**
 - unsigned char* data;
 - int len;
 - JSON: { "id":1, "data":"F10123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEFF2", "len":50 }

