/*
 * Copyright (C) 2016 Verizon. All Rights Reserved.
 */

package com.verizon.dmclientupdate;

/**
 * Preparing wap push package0 byte array according to document
 * OMA-TS-DM_Notification-V1_2-20070209-A.pdf
 * (Appendix C. Example of Trigger Message from Server)
 */

public class WapPushPackage0Builder {

    private byte[] mWapPushData;

    private final byte[] mWapPushSMSHeader = {
        (byte) 0x06,                // User-Data-Header (UDHL) Length = 6 bytes
        (byte) 0x05,                // UDH IE identifier: Port numbers
        (byte) 0x04,                // UDH port number IE length
        (byte) 0x0B, (byte) 0x84,   // Destination port
        (byte) 0xC0, (byte) 0x02,   // Originating port
        (byte) 0x01,                // Transaction ID / Push ID
        (byte) 0x06,                // PDU type (push)
        (byte) 0x03,                // Headerslength (content type+headers)
        (byte) 0xC4,                // Content type code
        (byte) 0xAF,                // X-WAP-Application-ID
        (byte) 0x87                 // Id for urn: x-wap-application:syncml.dm
    };

    private final byte[] mWapPushSMSMessageBody = {
        (byte) 0x98, (byte) 0xce, (byte) 0x7f, (byte) 0xb3, (byte) 0xf4,
        (byte) 0x85, (byte) 0xd4, (byte) 0xff, (byte) 0xf0, (byte) 0x20,
        (byte) 0x51, (byte) 0x8f, (byte) 0x5e, (byte) 0x87, (byte) 0xc7,
        (byte) 0x1b,                 // 128-bit digest value (dummy)
        (byte) 0x02, (byte) 0xc8, (byte) 0x00, (byte) 0x00,
        (byte) 0x00,                 // Version, UI-Mode, Initiator, Future DM use
        (byte) 0x00, (byte) 0x01,    // SessionID
        (byte) 0x0f,                 // Server Identifier length
        (byte) 0x63, (byte) 0x6f, (byte) 0x6d, (byte) 0x2e, (byte) 0x76,
        (byte) 0x7a, (byte) 0x77, (byte) 0x64, (byte) 0x6d, (byte) 0x73,
        (byte) 0x65, (byte) 0x72, (byte) 0x76, (byte) 0x65,
        (byte) 0x72,                 // Server Identifier ("com.vzwdmserver")
        (byte) 0x31, (byte) 0x30, (byte) 0x31, (byte) 0x30,
        (byte) 0x31, (byte) 0x30, (byte) 0x31, (byte) 0x30,
        (byte) 0x31, (byte) 0x30, (byte) 0x31, (byte) 0x30,
        (byte) 0x31,                 // 1010101010101
    };

    public byte[] prepareWapPushPackage0() {
        byte[] array = new byte[mWapPushSMSMessageBody.length];
        System.arraycopy(mWapPushSMSMessageBody, 0, array, 0, mWapPushSMSMessageBody.length);
        return array;
    }

    public int decodeTransactionId() {
        return mWapPushSMSHeader[7] & 0xFF;
    }

    public int decodePDUType() {
        return mWapPushSMSHeader[8] & 0xFF;
    }
}