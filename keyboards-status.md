# Keyboards Status

Below you can check if your keyboard is supported.

**Rangoli currently only supports models with vendor ID `0x258a`.**

It looks like the protocol used for keyboards with VID other than `0x258a` is totally different. Thankfully, for `0x0c45` boards, the reporter of that [issue](https://github.com/rnayabed/rangoli/issues/13) has provided packet captures with detailed context, so you are free to reverse engineer the protocol yourself. At the time of writing this, I really find myself unmotivated to work on it especially because I find it much more obfuscated than the protocol used for `0x258a` boards. Thankfully, a vast majority of the models in the market are `0x258a` protocol based and Rangoli supports most of the RK keyboards in the market.

Note that some keyboards have similar names but [totally different Vendor and Product IDs](https://github.com/rnayabed/rangoli/issues/14).

If you cannot find your keyboard in this list, or it is listed here yet you are still unable to configure it, please create an [issue](https://github.com/rnayabed/rangoli/issues/new) to bring it to my attention.

| Name                           | PID  | Supported          | Notes |
| ------------------------------ | ---- | ------------------ | ----- |
| RK-H81RGB                      | 0102 | <center>❌</center> | #17   |
| RK68 N ISO Return Bluetooth    | 0103 | <center>✔</center> |       |
| RK68RGB N Wired                | 010a | <center>✔</center> |       |
| RK84RGB N Wired                | 010d | <center>✔</center> |       |
| RK987RGB 2.4G N                | 01ff | <center>✔</center> |       |
| RK100                          | 002a | <center>✔</center> |       |
| RK-68Plus                      | 0048 | <center>✔</center> |       |
| RK-G68                         | 0049 | <center>✔</center> |       |
| RK61RGB Wired                  | 004a | <center>✔</center> |       |
| RK100RGB Wireless              | 004b | <center>✔</center> |       |
| RK87RGB                        | 004c | <center>✔</center> |       |
| RK71RGB Wired                  | 004d | <center>✔</center> |       |
| RK71                           | 004e | <center>✔</center> |       |
| G87RGB Wired                   | 004f | <center>✔</center> |       |
| RK837                          | 0052 | <center>✔</center> |       |
| RK960                          | 0053 | <center>✔</center> |       |
| RK987 Single Color             | 0054 | <center>✔</center> |       |
| RK-61                          | 0055 | <center>✔</center> |       |
| RK100RGB                       | 0056 | <center>✔</center> |       |
| RK857 Single                   | 0057 | <center>❌</center> | #16   |
| RK SINK104                     | 0058 | <center>✔</center> |       |
| RK84RGB                        | 0059 | <center>✔</center> |       |
| RK71RGB                        | 005a | <center>✔</center> |       |
| RK987RGB                       | 005b | <center>✔</center> |       |
| RK-G68RGB-UK                   | 005c | <center>✔</center> |       |
| RK857                          | 005d | <center>✔</center> |       |
| RK68RGB Bluetooth Dual Mode    | 005e | <center>✔</center> |       |
| RK61                           | 0060 | <center>✔</center> |       |
| RK68RGB Wired                  | 0064 | <center>✔</center> |       |
| RK61RGB ISO Return             | 0065 | <center>❌</center> | #12   |
| RK61ISO Return                 | 0066 | <center>✔</center> |       |
| RK100PRO                       | 0067 | <center>✔</center> |       |
| RK987RGB Wired                 | 0068 | <center>✔</center> |       |
| RK71RGB Wireless               | 0069 | <center>✔</center> |       |
| RK71 Bluetooth Dual Mode       | 006a | <center>✔</center> |       |
| RK87(882)                      | 006b | <center>✔</center> |       |
| RK68Plus                       | 006f | <center>✔</center> |       |
| RK98 RGB                       | 0070 | <center>✔</center> |       |
| RK87(882) Wired                | 0074 | <center>✔</center> |       |
| G68RGB ISO Return              | 0075 | <center>✔</center> |       |
| RK87                           | 0078 | <center>✔</center> |       |
| RK68Pro                        | 0079 | <center>✔</center> |       |
| RK61 RGB Bluetooth Dual Mode   | 007a | <center>✔</center> |       |
| RK896                          | 007b | <center>✔</center> |       |
| RK87T RGBWired                 | 007c | <center>✔</center> |       |
| G87RGB Wired                   | 007d | <center>✔</center> |       |
| RK96 RGB                       | 0089 | <center>✔</center> |       |
| RK68 Bluetooth Dual Mode       | 008b | <center>✔</center> |       |
| RK61RGB ISO ReturnWired        | 008c | <center>✔</center> |       |
| RK68 a switch                  | 008e | <center>✔</center> |       |
| RK920RGB                       | 008f | <center>✔</center> |       |
| RK89RGB                        | 009c | <center>✔</center> |       |
| RK92RGB N                      | 009e | <center>✔</center> |       |
| 68RGB Wired                    | 00a2 | <center>✔</center> |       |
| RK96                           | 00a4 | <center>✔</center> |       |
| RK71RGB N                      | 00a5 | <center>✔</center> |       |
| RK932                          | 00a6 | <center>✔</center> |       |
| RK68ISO Return                 | 00a9 | <center>❌</center> | #11   |
| RK750-100RGB                   | 00aa | <center>✔</center> |       |
| RK98Wired-100KRGB              | 00b3 | <center>✔</center> |       |
| RK71RGB N BT                   | 00b5 | <center>✔</center> |       |
| RK61 Bluetooth Dual Mode       | 00b8 | <center>✔</center> |       |
| RK857 Single                   | 00c0 | <center>✔</center> |       |
| RK98-100KRGB N                 | 00c1 | <center>✔</center> |       |
| K87RGB N                       | 00c3 | <center>✔</center> |       |
| RK87RGB N Wired                | 00c4 | <center>✔</center> |       |
| RK858 (61)                     | 00c6 | <center>✔</center> |       |
| RK84N RGB                      | 00c8 | <center>✔</center> |       |
| RK61 N RGB Bluetooth Dual Mode | 00c9 | <center>✔</center> |       |
| RK89                           | 00cd | <center>✔</center> |       |
| RK838 RGB                      | 00d6 | <center>✔</center> |       |
| RK98Wired-100K N RGB           | 00d8 | <center>✔</center> |       |
| RK61plus RGB N                 | 00da | <center>✔</center> |       |
| RK883RGB N Wired               | 00de | <center>✔</center> |       |
| RK100RGB N                     | 00e0 | <center>✔</center> |       |
| RK61RGB N                      | 00e1 | <center>✔</center> |       |
| RKG68RGB N                     | 00e7 | <center>✔</center> |       |
| RK71RGB N Wired                | 00ea | <center>✔</center> |       |
| RK61RGB N ISO Return           | 00ec | <center>✔</center> |       |
| RK61 N RGB Bluetooth Dual Mode | 00ed | <center>✔</center> |       |
| RK-H87RGB                      | 00ef | <center>✔</center> |       |
| RK84Pro N RGB                  | 00f3 | <center>✔</center> |       |
| RK84 RGB ISO Return            | 00f4 | <center>✔</center> |       |
| RK-R87RGB Wired                | 00f6 | <center>✔</center> |       |
| RK-R87 Wired                   | 00f7 | <center>✔</center> |       |
| RK61plus RGB                   | 00f8 | <center>✔</center> |       |
| RK987RGB N                     | 00f9 | <center>✔</center> |       |
| RK68RGB N Bluetooth Dual Mode  | 00ff | <center>✔</center> |       |
