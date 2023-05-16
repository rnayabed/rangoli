# Supported Keyboards

**Rangoli currently only supports models with vendor ID `0x258a`.**

It looks like the protocol used for keyboards with VID other than `0x258a` is totally different. Thankfully, for `0x0c45` boards, the reporter of that [issue](https://github.com/rnayabed/rangoli/issues/13) has provided packet captures with detailed context, so you are free to reverse engineer the protocol yourself. At the time of writing this, I really find myself unmotivated to work on it especially because I find it much more obfuscated than the protocol used for `0x258a` boards. Thankfully, a vast majority of the models in the market are `0x258a` protocol based and Rangoli supports most of the RK keyboards in the market.

Note that some keyboards have similar names but [totally different Vendor and Product IDs](https://github.com/rnayabed/rangoli/issues/14).

If you cannot find your keyboard in this list, or it is listed here yet you are still unable to configure it, please create an [issue](https://github.com/rnayabed/rangoli/issues/new) to bring it to my attention.

| Name                           | PID  |
| ------------------------------ | ---- |
| RK-H81RGB                      | 0102 |
| RK68 N ISO Return Bluetooth    | 0103 |
| RK68RGB N Wired                | 010a |
| RK84RGB N Wired                | 010d |
| RK987RGB 2.4G N                | 01ff |
| RK100                          | 002a |
| RK-68Plus                      | 0048 |
| RK-G68                         | 0049 |
| RK61RGB Wired                  | 004a |
| RK100RGB Wireless              | 004b |
| RK87RGB                        | 004c |
| RK71RGB Wired                  | 004d |
| RK71                           | 004e |
| G87RGB Wired                   | 004f |
| RK837                          | 0052 |
| RK960                          | 0053 |
| RK987 Single Color             | 0054 |
| RK-61                          | 0055 |
| RK100RGB                       | 0056 |
| RK857 Single                   | 0057 |
| RK SINK104                     | 0058 |
| RK84RGB                        | 0059 |
| RK71RGB                        | 005a |
| RK987RGB                       | 005b |
| RK-G68RGB-UK                   | 005c |
| RK857                          | 005d |
| RK68RGB Bluetooth Dual Mode    | 005e |
| RK61                           | 0060 |
| RK68RGB Wired                  | 0064 |
| RK61RGB ISO Return             | 0065 |
| RK61ISO Return                 | 0066 |
| RK100PRO                       | 0067 |
| RK987RGB Wired                 | 0068 |
| RK71RGB Wireless               | 0069 |
| RK71 Bluetooth Dual Mode       | 006a |
| RK87(882)                      | 006b |
| RK68Plus                       | 006f |
| RK98 RGB                       | 0070 |
| RK87(882) Wired                | 0074 |
| G68RGB ISO Return              | 0075 |
| RK87                           | 0078 |
| RK68Pro                        | 0079 |
| RK61 RGB Bluetooth Dual Mode   | 007a |
| RK896                          | 007b |
| RK87T RGBWired                 | 007c |
| G87RGB Wired                   | 007d |
| RK96 RGB                       | 0089 |
| RK68 Bluetooth Dual Mode       | 008b |
| RK61RGB ISO ReturnWired        | 008c |
| RK68 a switch                  | 008e |
| RK920RGB                       | 008f |
| RK89RGB                        | 009c |
| RK92RGB N                      | 009e |
| 68RGB Wired                    | 00a2 |
| RK96                           | 00a4 |
| RK71RGB N                      | 00a5 |
| RK932                          | 00a6 |
| RK68ISO Return                 | 00a9 |
| RK750-100RGB                   | 00aa |
| RK98Wired-100KRGB              | 00b3 |
| RK71RGB N BT                   | 00b5 |
| RK61 Bluetooth Dual Mode       | 00b8 |
| RK857 Single                   | 00c0 |
| RK98-100KRGB N                 | 00c1 |
| K87RGB N                       | 00c3 |
| RK87RGB N Wired                | 00c4 |
| RK858 (61)                     | 00c6 |
| RK84N RGB                      | 00c8 |
| RK61 N RGB Bluetooth Dual Mode | 00c9 |
| RK89                           | 00cd |
| RK838 RGB                      | 00d6 |
| RK98Wired-100K N RGB           | 00d8 |
| RK61plus RGB N                 | 00da |
| RK883RGB N Wired               | 00de |
| RK100RGB N                     | 00e0 |
| RK61RGB N                      | 00e1 |
| RKG68RGB N                     | 00e7 |
| RK71RGB N Wired                | 00ea |
| RK61RGB N ISO Return           | 00ec |
| RK61 N RGB Bluetooth Dual Mode | 00ed |
| RK-H87RGB                      | 00ef |
| RK84Pro N RGB                  | 00f3 |
| RK84 RGB ISO Return            | 00f4 |
| RK-R87RGB Wired                | 00f6 |
| RK-R87 Wired                   | 00f7 |
| RK61plus RGB                   | 00f8 |
| RK987RGB N                     | 00f9 |
| RK68RGB N Bluetooth Dual Mode  | 00ff |
