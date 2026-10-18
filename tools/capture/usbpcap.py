# Minimal pcapng + USBPcap parser, no Wireshark needed. Run as a script to print
# every host-to-device control transfer (SET_REPORT) in a capture with its data:
#
#   python3 -I usbpcap.py capture.pcapng
#
# Rangoli's keyboards are configured with SET_REPORT feature reports
# (req 21/09, report 0x0a), so the interesting lines start with "req21/09".
# See docs/rk96/CAPTURING.md.
import struct, sys

def blocks(data):
    off = 0
    le = '<'
    while off + 12 <= len(data):
        btype, blen = struct.unpack_from(le + 'II', data, off)
        if btype == 0x0A0D0D0A:
            magic = data[off+8:off+12]
            le = '<' if magic == b'\x4d\x3c\x2b\x1a' else '>'
            blen = struct.unpack_from(le + 'I', data, off + 4)[0]
        yield btype, data[off+8:off+blen-4], le
        off += blen

def packets(path):
    data = open(path, 'rb').read()
    n = 0
    for btype, body, le in blocks(data):
        if btype != 6: continue
        iface, tsh, tsl, caplen, origlen = struct.unpack_from(le + 'IIIII', body, 0)
        pkt = body[20:20+caplen]
        ts = ((tsh << 32) | tsl) / 1e6
        n += 1
        yield n, ts, pkt

def usbpcap(pkt):
    hlen, irp, status, func, info, bus, dev, ep, xfer, dlen = struct.unpack_from('<HQIHBHHBBI', pkt, 0)
    return dict(hlen=hlen, info=info, bus=bus, dev=dev, ep=ep, xfer=xfer, dlen=dlen, func=func, status=status), pkt[hlen:]

if __name__ == '__main__':
    path = sys.argv[1]
    t0 = None
    for n, ts, pkt in packets(path):
        h, payload = usbpcap(pkt)
        if t0 is None: t0 = ts
        # control transfers: xfer 2; setup stage carries 8 byte setup + data (OUT)
        if h['xfer'] == 2 and len(payload) >= 8 and h['info'] & 1 == 0:
            setup = payload[:8]
            bm, breq, wval, widx, wlen = struct.unpack('<BBHHH', setup)
            data = payload[8:]
            print(f"{n:6d} {ts-t0:9.3f} dev{h['dev']} req{bm:02x}/{breq:02x} val{wval:04x} idx{widx} len{wlen} {data.hex()}")
